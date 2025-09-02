//===- PolyqsimDialect.td - Polyqsim dialect -----------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef POLYQSIM_DIALECT
#define POLYQSIM_DIALECT

include "mlir/IR/OpBase.td"

//===----------------------------------------------------------------------===//
// Polyqsim dialect definition.
//===----------------------------------------------------------------------===//

def Polyqsim_Dialect : Dialect {
    let name = "polyqsim";
    let summary = "A polyqsim out-of-tree MLIR dialect.";
    let description = [{
        This dialect is an example of an out-of-tree MLIR dialect designed to
        illustrate the basic setup required to develop MLIR-based tools without
        working inside of the LLVM source tree.
    }];
    let cppNamespace = "::mlir::polyqsim";
}

//===----------------------------------------------------------------------===//
// Base polyqsim operation definition.
//===----------------------------------------------------------------------===//

class Polyqsim_Op<string mnemonic, list<Trait> traits = []> :
        Op<Polyqsim_Dialect, mnemonic, traits>;

#endif // POLYQSIM_DIALECT
