#include "Polyqsim/Passes.h"
#include "Polyqsim/PolyqsimDialect.h"
#include "Polyqsim/PolyqsimOps.h"
#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Arith/Utils/Utils.h"
#include "mlir/Dialect/Complex/IR/Complex.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/TypeRange.h"
#include "mlir/IR/Value.h"
#include "mlir/IR/ValueRange.h"
#include "mlir/IR/Visitors.h"
#include "mlir/Support/LLVM.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <complex>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include "mlir/Transforms/GreedyPatternRewriteDriver.h"

namespace mlir {
class Pass;
namespace polyqsim {
#define GEN_PASS_DEF_POLYQSIMLOWERING
#include "Polyqsim/Passes.h.inc"
} // namespace polyqsim
} // namespace mlir

using namespace mlir;
using namespace mlir::polyqsim;
namespace {

std::vector<std::vector<std::vector<float>>>
getUnitarySingleQubit(std::string name, std::vector<float> params) {
  return {{{1, 0}, {0, 0}}, {{0, 0}, {1, 0}}};
}

struct ApplySingleQubitGateLowering
    : public mlir::OpRewritePattern<polyqsim::ApplyGateOp> {
  ApplySingleQubitGateLowering(MLIRContext *ctx, Value svmemref, int numQubits,
                    Type complexType)
      : OpRewritePattern(ctx), svmemref(svmemref), numQubits(numQubits),
        complexType(complexType) {}

  LogicalResult matchAndRewrite(polyqsim::ApplyGateOp op,
                                PatternRewriter &rewriter) const override {
    Location loc = op.getLoc();
    std::string gate = op->getAttrOfType<StringAttr>("gate").str();

    // Extract qubits
    std::vector<int> qubits;
    for (auto q :
         op->getAttrOfType<ArrayAttr>("qubits").getAsRange<IntegerAttr>()) {
      qubits.push_back(q.getInt());
    }

    // Extract params
    std::vector<float> params;
    for (auto p :
         op->getAttrOfType<ArrayAttr>("params").getAsRange<FloatAttr>()) {
      params.push_back(p.getValue().convertToFloat());
    }

    // Only handle single-qubit case for now
    if (qubits.size() != 1)
      return rewriter.notifyMatchFailure(op, "not a single qubit gate");

    int k = qubits[0];
    int n = numQubits;
    auto unitary = getUnitarySingleQubit(gate, params);

    // Use rewriter instead of plain builder
    auto outerLoop = rewriter.create<affine::AffineForOp>(
        loc, 0, 1 << n, 1 << (k + 1), ValueRange{},
        [&](OpBuilder &outerBuilder, Location outerLoc, Value g, ValueRange) {
          auto innerLoop = outerBuilder.create<affine::AffineForOp>(
              outerLoc, ValueRange{g}, outerBuilder.getDimIdentityMap(),
              ValueRange{g}, outerBuilder.getSingleDimShiftAffineMap(1 << k), 1,
              ValueRange{},
              [&](OpBuilder &innerBuilder, Location innerLoc, Value i,
                  ValueRange) {
                // Load old amplitudes
                Value oldAlphaI = innerBuilder.create<affine::AffineLoadOp>(
                    innerLoc, svmemref, ValueRange{i});
                Value oldAlphaIPlus2powk =
                    innerBuilder.create<affine::AffineLoadOp>(
                        innerLoc, svmemref,
                        innerBuilder.getSingleDimShiftAffineMap(1 << k),
                        ValueRange{i});

                // Constants from unitary
                Value q11 = innerBuilder.create<complex::ConstantOp>(
                    innerLoc, complexType,
                    innerBuilder.getF32ArrayAttr(unitary[0][0]));
                Value q12 = innerBuilder.create<complex::ConstantOp>(
                    innerLoc, complexType,
                    innerBuilder.getF32ArrayAttr(unitary[0][1]));
                Value q21 = innerBuilder.create<complex::ConstantOp>(
                    innerLoc, complexType,
                    innerBuilder.getF32ArrayAttr(unitary[1][0]));
                Value q22 = innerBuilder.create<complex::ConstantOp>(
                    innerLoc, complexType,
                    innerBuilder.getF32ArrayAttr(unitary[1][1]));

                // Matrix-vector multiply
                Value tmp1 = innerBuilder.create<complex::MulOp>(innerLoc, q11,
                                                                 oldAlphaI);
                Value tmp2 = innerBuilder.create<complex::MulOp>(
                    innerLoc, q12, oldAlphaIPlus2powk);
                Value newAlphaI =
                    innerBuilder.create<complex::AddOp>(innerLoc, tmp1, tmp2);

                Value tmp3 = innerBuilder.create<complex::MulOp>(innerLoc, q21,
                                                                 oldAlphaI);
                Value tmp4 = innerBuilder.create<complex::MulOp>(
                    innerLoc, q22, oldAlphaIPlus2powk);
                Value newAlphaIPlus2powk =
                    innerBuilder.create<complex::AddOp>(innerLoc, tmp3, tmp4);

                // Store results
                innerBuilder.create<affine::AffineStoreOp>(
                    innerLoc, newAlphaI, svmemref, ValueRange{i});
                innerBuilder.create<affine::AffineStoreOp>(
                    innerLoc, newAlphaIPlus2powk, svmemref,
                    innerBuilder.getSingleDimShiftAffineMap(1 << k),
                    ValueRange{i});

                innerBuilder.create<affine::AffineYieldOp>(innerLoc);
              });
          outerBuilder.create<affine::AffineYieldOp>(outerLoc);
        });

    // Remove the ApplyGateOp
    rewriter.eraseOp(op);
    return success();
  }

private:
  Value svmemref;
  int numQubits;
  Type complexType;
};

struct PolyqsimLowering
    : public mlir::polyqsim::impl::PolyqsimLoweringBase<PolyqsimLowering> {
  void runOnOperation() override {
    ModuleOp module = llvm::dyn_cast<ModuleOp>(getOperation());
    func::FuncOp mainFunc = module.lookupSymbol<func::FuncOp>("main");
    if (!mainFunc) {
      module.emitError("No main function found");
      return signalPassFailure();
    }
    size_t numQubits =
        cast<IntegerAttr>(mainFunc->getAttr("num_qubits")).getInt();

    // Statevector as memref global
    OpBuilder builder(module.getBodyRegion());

    ComplexType complexType = ComplexType::get(builder.getF32Type());
    auto statevectorType = MemRefType::get({1 << numQubits}, complexType);
    auto statevector = builder.create<memref::GlobalOp>(
        module.getLoc(),
        /*sym_name=*/"statevector",
        /*sym_visibility=*/builder.getStringAttr("private"),
        /*type=*/statevectorType,
        /*initial_value=*/Attribute(),
        /*constant=*/false,
        /*alignment=*/IntegerAttr());

    builder.setInsertionPointToStart(&mainFunc.getBody().front());
    auto svmemref = builder
                        .create<memref::GetGlobalOp>(
                            mainFunc->getLoc(), statevectorType, "statevector")
                        ->getResult(0);
    svmemref.dump();

    RewritePatternSet patterns(&getContext());
    patterns.add<ApplySingleQubitGateLowering>(&getContext(), svmemref, numQubits,
                                    complexType);
    if (failed(applyPatternsGreedily(module.getOperation(), std::move(patterns)))) {
      signalPassFailure();
    } 
  }
};
} // namespace

std::unique_ptr<mlir::Pass> polyqsim::createPolyqsimLoweringPass() {
  return std::make_unique<PolyqsimLowering>();
}