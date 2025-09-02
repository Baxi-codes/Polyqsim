//===- PolyqsimDialect.cpp - Polyqsim dialect ---------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "Polyqsim/PolyqsimDialect.h"
#include "Polyqsim/PolyqsimOps.h"

using namespace mlir;
using namespace mlir::polyqsim;

//===----------------------------------------------------------------------===//
// Polyqsim dialect.
//===----------------------------------------------------------------------===//

void PolyqsimDialect::initialize() {
  addOperations<
#define GET_OP_LIST
#include "Polyqsim/PolyqsimOps.cpp.inc"
      >();
}
