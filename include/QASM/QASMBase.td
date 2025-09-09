#ifndef QASM_QASM_TD
#define QASM_QASM_TD

include "mlir/IR/OpBase.td"
include "mlir/IR/Utils.td"
include "mlir/IR/AttrTypeBase.td"
include "mlir/IR/BuiltinTypeInterfaces.td"
include "mlir/IR/BuiltinTypes.td"
include "mlir/IR/CommonTypeConstraints.td"
include "mlir/Interfaces/FunctionInterfaces.td"
include "mlir/Interfaces/SideEffectInterfaces.td"

// The QASM dialect.
def QASMDialect : Dialect {
let name = "qasm";
let summary = "The QASM quantum circuit dialect.";
let description = "This dialect provides operations for representing quantum circuits based on the OpenQASM 2.0 specification.";
let cppNamespace = "::qasm";
let dependentDialects = [
"mlir::arith::ArithDialect",
"mlir::func::FuncDialect"
];
}

#endif // QASM_QASM_TD