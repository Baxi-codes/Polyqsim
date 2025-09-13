//===- polyqsim-opt.cpp ---------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "Polyqsim/Passes.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/InitAllDialects.h"
#include "mlir/InitAllPasses.h"
#include "mlir/Support/FileUtilities.h"
#include "mlir/Tools/mlir-opt/MlirOptMain.h"

#include "Polyqsim/PolyqsimDialect.h"
#include "Polyqsim/PolyqsimOpsDialect.cpp.inc"

int main(int argc, char **argv) {
  mlir::DialectRegistry registry;
  registry.insert<mlir::polyqsim::PolyqsimDialect>();
  registry.insert<mlir::func::FuncDialect>();
  registry.insert<mlir::arith::ArithDialect>();
  registry.insert<mlir::memref::MemRefDialect>();

  mlir::polyqsim::registerPolyqsimPasses();

  return mlir::asMainReturnCode(
      mlir::MlirOptMain(argc, argv, "Polyqsim optimizer driver\n", registry));
}
