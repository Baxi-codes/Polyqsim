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
let assemblyFormat = " `<` $size `>` ";
}

// Represents a classical register of a specific size.
def CRegType : QASM_Type<"CReg", "creg"> {
let summary = "Classical register type.";
let description = [{
A classical register of a specific size.
}];
let parameters = (ins TypeParameter<"int64_t", "size of reg">:$size);

let assemblyFormat = " `<` $size `>` ";
}

#endif // QASM_QASMTYPES_TD
