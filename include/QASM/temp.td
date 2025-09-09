#ifndef QASM_QASM_TD
#define QASM_QASM_TD

include "mlir/IR/OpBase.td"
include "mlir/IR/Utils.td"
include "mlir/IR/AttrTypeBase.td"
include "mlir/IR/BuiltinTypeInterfaces.td"
include "mlir/IR/BuiltinTypes.td"
include "mlir/IR/CommonTypeConstraints.td"
include "mlir/Interfaces/FunctionInterfaces.td"

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

// The base class for all types in the QASM dialect.
class QASM_Type<string name, string typeMnemonic>
: TypeDef<QASMDialect, name> {
let mnemonic = typeMnemonic;
}

// Represents a single quantum bit.
def QubitType : QASM_Type<"Qubit", "qubit">;

// Represents a quantum register of a specific size.
def QRegType : QASM_Type<"QReg", "qreg"> {
let summary = "Quantum register type.";
let description = [{
A quantum register of a specific size.
}];
let parameters = (ins
I64Attr:$size // The size of the register.
);
let assemblyFormat = " < $size > ";
}

// Represents a classical register of a specific size.
def CRegType : QASM_Type<"CReg", "creg"> {
let summary = "Classical register type.";
let description = [{
A classical register of a specific size.
}];
let parameters = (ins
I64Attr:$size // The size of the register.
);
let assemblyFormat = " < $size > ";
}

// The base class for QASM operations.
class QASMOp<string mnemonic, list<Trait> traits = []>
: Op<QASMDialect, mnemonic, traits>;

// Defines a quantum register.
def QRegOp : QASMOp<"qreg", [NoSideEffect, NoTerminator]> {
let summary = "Defines a quantum register.";
let description = "Declares a quantum register of a given size.";
let arguments = (ins I64Attr:$size);
let results = (ins QRegType);
// Verifier to ensure the register size is positive.
let verifier = "$size > 0";
let assemblyFormat = " $size attr-dict : type($results) ";
}

// Defines a classical register.
def CRegOp : QASMOp<"creg", [NoSideEffect, NoTerminator]> {
let summary = "Defines a classical register.";
let description = "Declares a classical register of a given size.";
let arguments = (ins I64Attr:$size);
let results = (ins CRegType);
// Verifier to ensure the register size is positive.
let verifier = "$size > 0";
let assemblyFormat = " $size attr-dict : type($results) ";
}

// Applies a single-qubit square-root-of-X gate.
def SXOp : QASMOp<"sx", [Pure, NoSideEffect, NoTerminator]> {
let summary = "Square-root-of-X gate.";
let description = "Applies the single-qubit square-root-of-X gate.";
let arguments = (ins QubitType:$qubit);
let results = (ins QubitType);
let assemblyFormat = " $qubit attr-dict : type($qubit) -> type($results) ";
}

// Applies a single-qubit rotation around the Z-axis.
def RZOp : QASMOp<"rz", [Pure, NoSideEffect, NoTerminator]> {
let summary = "Z-axis rotation gate.";
let description = "Applies a rotation around the Z-axis to a single qubit.";
let arguments = (ins QubitType:$qubit, F64Attr:$angle);
let results = (ins QubitType);
let assemblyFormat = " ( $angle ) $qubit attr-dict : type($qubit) -> type($results) ";
}

#endif // QASM_QASM_TD