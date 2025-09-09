#ifndef QASM_QASMOPS_TD
#define QASM_QASMOPS_TD

include "QASM/QASMTypes.td"

// The base class for QASM operations.
class QASMOp<string mnemonic, list<Trait> traits = []>
: Op<QASMDialect, mnemonic, traits>;

// Defines a quantum register.
def QRegOp : QASMOp<"qreg", [MemoryEffects<[MemAlloc]>, NoTerminator]> {
let summary = "Defines a quantum register.";
let description = "Declares a quantum register of a given size.";
let arguments = (ins I64Attr:$size);
let results = (outs QRegType:$res);
// Verifier to ensure the register size is positive.
// let verifier = "return $_self.getValue().getZExtValue() > 0;";
let assemblyFormat = "`qreg` `<`$size`>` attr-dict `:` type($res)";
}

// Defines a classical register.
def CRegOp : QASMOp<"creg", [MemoryEffects<[MemRead]>, NoTerminator]> {
let summary = "Defines a classical register.";
let description = "Declares a classical register of a given size.";
let arguments = (ins I64Attr:$size);
let results = (outs CRegType:$res);
// Verifier to ensure the register size is positive.
// let verifier = "return $_self.getValue().getZExtValue() > 0;";
let assemblyFormat = "`creg` `<` $size `>` attr-dict `:` type($res)";
}

// Applies a single-qubit square-root-of-X gate.
def SXOp : QASMOp<"sx", [Pure, MemoryEffects<[MemRead]>, MemoryEffects<[MemWrite]>, NoTerminator]> {
let summary = "Square-root-of-X gate.";
let description = "Applies the single-qubit square-root-of-X gate.";
let arguments = (ins QubitType:$qubit);
let results = (outs QubitType:$res);
let assemblyFormat = "$qubit attr-dict `:` type($qubit) `->` type($res)";
}

// Applies a single-qubit rotation around the Z-axis.
def RZOp : QASMOp<"rz", [Pure, MemoryEffects<[MemRead]>, MemoryEffects<[MemWrite]>, NoTerminator]> {
  let summary = "Z-axis rotation gate.";
  let description = "Applies a rotation around the Z-axis to a single qubit.";
  let arguments = (ins QubitType:$qubit, F64Attr:$angle);
  let results = (outs QubitType:$res);

  let assemblyFormat = "`(` $angle `)` $qubit attr-dict `:` type($qubit) `->` type($res)";
}

// A barrier on a variadic list of qubits.
def BarrierOp : QASMOp<"barrier", [NoMemoryEffect, NoTerminator]> {
let summary = "A barrier on a list of qubits.";
let description = "Ensures all preceding operations on the specified qubits are complete before any following operations.";
let arguments = (ins Variadic<QubitType>:$qubits);
let assemblyFormat = " $qubits attr-dict ";
}

// Measures a qubit and stores the result in a classical register.
def MeasureOp : QASMOp<"measure", [NoMemoryEffect, NoTerminator]> {
let summary = "Measures a qubit.";
let description = "Measures a single qubit and stores the result in a classical register.";
let arguments = (ins QubitType:$qubit, CRegType:$creg);
let assemblyFormat = " $qubit `->` $creg attr-dict `:` type($qubit) `->` type($creg)";
}

#endif // QASM_QASMOPS_TD
