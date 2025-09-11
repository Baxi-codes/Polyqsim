//===- PolyqsimOps.td - Polyqsim dialect ops -----------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef POLYQSIM_OPS
#define POLYQSIM_OPS

include "PolyqsimDialect.td"
include "mlir/Interfaces/InferTypeOpInterface.td"
include "mlir/Interfaces/SideEffectInterfaces.td"

def Polyqsim_FooOp : Polyqsim_Op<"foo", [Pure,
                                             SameOperandsAndResultType]> {
    let summary = "Illustrates how to define an operation.";
    let description = [{
        The `polyqsim.foo` operation illustrates how to define a new
        operation in a dialect. It uses an operation trait to declare that it
        has no side effects.

        This operation takes an integer argument and returns an integer.

        Example:

        ```mlir
        %0 = arith.constant 2 : i32
        // Apply the foo operation to %0
        %1 = polyqsim.foo %0 : i32
        ```
    }];

    let arguments = (ins I32:$input);
    let results = (outs I32:$res);

    let assemblyFormat = [{
        $input attr-dict `:` type($input)
    }];
}

def Polyqsim_ApplyGateOp : Polyqsim_Op<"apply_gate"> {
    let summary = "Apply a gate.";
    let description = [{
        Apply a quantum gate to qubits. All things go in attributes.
    }];

    let arguments = (ins );
    let results = (outs );
}

#endif // POLYQSIM_OPS