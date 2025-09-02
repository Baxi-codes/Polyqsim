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
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/DialectRegistry.h"
#include "mlir/IR/Operation.h"
#include "mlir/InitAllTranslations.h"
#include "mlir/Tools/mlir-translate/MlirTranslateMain.h"
#include "mlir/Tools/mlir-translate/Translation.h"
#include "llvm/MC/MCParser/AsmLexer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#include "Polyqsim/PolyqsimOpsDialect.cpp.inc"


int main(int argc, char **argv) {

  // TODO: Register standalone translations here.
  mlir::TranslateToMLIRRegistration toPolyqsimDialect(
      "to-polyqsim", "Translate to the Polyqsim dialect",
      [](llvm::SourceMgr &sm, mlir::MLIRContext *context) {
        
        context->getOrLoadDialect<mlir::polyqsim::PolyqsimDialect>();
        mlir::Operation* op = nullptr;
        return op;
      });

  return failed(
      mlir::mlirTranslateMain(argc, argv, "MLIR Translation Testing Tool"));
}
