//===- Passes.h - Include Tblgen pass defs ------------C++-===//
#ifndef POLYQSIM_PASSES_H
#define POLYQSIM_PASSES_H

#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/Dialect/Complex/IR/Complex.h"
#include "Polyqsim/PolyqsimOps.h"
#include "mlir/Pass/Pass.h"

namespace mlir {
class Pass;

namespace polyqsim {

std::unique_ptr<mlir::Pass> createPolyqsimLoweringPass();

/// Generate the code for registering passes.
#define GEN_PASS_REGISTRATION
#include "Polyqsim/Passes.h.inc"

} // namespace polyqsim

} // namespace mlir

#endif