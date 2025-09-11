//===- standalone-translate.cpp ---------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This is a command line utility that translates a file from/to MLIR using one
// of the registered translations.
//
//===----------------------------------------------------------------------===//

#include "Polyqsim/PolyqsimDialect.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/DialectRegistry.h"
#include "mlir/IR/Location.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/ValueRange.h"
#include "mlir/InitAllTranslations.h"
#include "mlir/Tools/mlir-translate/MlirTranslateMain.h"
#include "mlir/Tools/mlir-translate/Translation.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCParser/AsmLexer.h"
#include "llvm/Support/LogicalResult.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>
#include <sstream>
#include <string>

#include "Polyqsim/PolyqsimOps.h"
#include "Polyqsim/PolyqsimOpsDialect.cpp.inc"

#include "pybridge/pybridge.h"

namespace {

llvm::FailureOr<mlir::Operation *>
parseSimplifiedQasm(std::stringstream &qasmStream, mlir::MLIRContext *context) {

  mlir::OpBuilder builder(context);
  mlir::ModuleOp module =
      builder.create<mlir::ModuleOp>(builder.getUnknownLoc());
  builder.setInsertionPointToStart(module.getBody());

  mlir::func::FuncOp mainFunc = builder.create<mlir::func::FuncOp>(
      module.getLoc(), "main",
      builder.getFunctionType({}, {builder.getI32Type()}));
  mainFunc.addEntryBlock();
  builder.setInsertionPointToStart(&mainFunc.getBody().front());
  // All lines in stream
  std::string line;
  while (std::getline(qasmStream, line)) {
    std::istringstream iss(line);
    std::string operation;
    iss >> operation;
    // Switch case for operation
    if (operation == "comment") {
      // Ignore comments
    } else if (operation == "num_qubits") {
      int numQubits;
      iss >> numQubits;
      mainFunc->setAttr("num_qubits", builder.getI32IntegerAttr(numQubits));
    } else {
      auto applyGateOp = builder.create<mlir::polyqsim::ApplyGateOp>(
          mainFunc.getLoc(), mlir::ValueRange{}, mlir::ValueRange{});
      applyGateOp->setAttr("gate", builder.getStringAttr(operation));
      std::string paramsListStart;
      iss >> paramsListStart;
      if (paramsListStart != "PARAMS") {
        llvm::errs() << "Expected PARAMS but got " << paramsListStart << "\n";
        return llvm::failure();
      }
      llvm::SmallVector<float, 4> params;
      std::string currentParam;
      while (iss >> currentParam) {
        if (currentParam == "QARGS") {
          break;
        }
        params.push_back(std::stof(currentParam));
      }
      llvm::SmallVector<int, 4> qubits;
      std::string currentQubit;
      while (iss >> currentQubit) {
        if (currentQubit == "END") {
          break;
        }
        qubits.push_back(std::stoi(currentQubit));
      }
      if (currentQubit != "END") {
        llvm::errs() << "Expected END but got " << currentQubit << "\n";
        return llvm::failure();
      }
      auto paramsAttr = builder.getF32ArrayAttr(params);
      auto qubitsAttr = builder.getI32ArrayAttr(qubits);
      applyGateOp->setAttr("params", paramsAttr);
      applyGateOp->setAttr("qubits", qubitsAttr);
    }
  }
  auto zero = builder.create<mlir::arith::ConstantIntOp>(mainFunc.getLoc(), builder.getIntegerType(32), 0);
  builder.create<mlir::func::ReturnOp>(mainFunc.getLoc(),
                                       zero.getResult());

  return module.getOperation();
}
} // namespace

int main(int argc, char **argv) {
  mlir::TranslateToMLIRRegistration toPolyqsimDialect(
      "to-polyqsim", "Translate to the Polyqsim dialect",
      [](llvm::SourceMgr &sm, mlir::MLIRContext *context) {
        context->loadAllAvailableDialects();
        llvm::StringRef buff =
            sm.getMemoryBuffer(sm.getMainFileID())->getBuffer();
        std::stringstream simplifiedQasm(simplifyQuantumCircuit(buff.str()));
        auto module = parseSimplifiedQasm(simplifiedQasm, context);
        if (llvm::failed(module)) {
          // return empty module on failure
          mlir::OpBuilder builder(context);
          return builder.create<mlir::ModuleOp>(builder.getUnknownLoc()).getOperation();
        }
        return module.value();
      }, [](mlir::DialectRegistry &registry) {
        registry.insert<mlir::func::FuncDialect>();
        registry.insert<mlir::arith::ArithDialect>();
        registry.insert<mlir::polyqsim::PolyqsimDialect>();
      });

  return failed(
      mlir::mlirTranslateMain(argc, argv, "MLIR Translation Testing Tool"));
}
