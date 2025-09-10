#ifndef QASM_QASMTYPES_TD
#define QASM_QASMTYPES_TD

include "QASM/QASMBase.td"

// The base class for all types in the QASM dialect.
class QASM_Type<string name, string typeMnemonic>
: TypeDef<QASMDialect, name> {
let mnemonic = typeMnemonic;
}

// Represents a single quantum bit.
def QubitType : QASM_Type<"Qubit", "qubit">{
let summary = "Quantum bit type.";
let description = [{
A single quantum bit (qubit).
}];
let assemblyFormat = " `qubit` ";
}

// Represents a quantum register of a specific size.
def QRegType : QASM_Type<"QReg", "qreg"> {
let summary = "Quantum register type.";
let description = [{
A quantum register of a specific size.
}];
let parameters = (ins TypeParameter<"int64_t", "size of reg">:$size);
let assemblyFormat = " `qreg``<` $size `>` ";
}

// Represents a classical register of a specific size.
def CRegType : QASM_Type<"CReg", "creg"> {
let summary = "Classical register type.";
let description = [{
A classical register of a specific size.
}];
let parameters = (ins TypeParameter<"int64_t", "size of reg">:$size);
let assemblyFormat = " `creg``<` $size `>` ";
}

// Represents a measurement result (0 or 1) we will call it a cbit.
def CBitType : QASM_Type<"CBit", "cbit"> {
let summary = "Classical bit type.";
let description = [{
A classical bit (cbit) representing a measurement result (0 or 1).
}];
let assemblyFormat = " `cbit` ";
}

#endif // QASM_QASMTYPES_TD
