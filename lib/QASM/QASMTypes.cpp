//===- QASMTypes.cpp - QASM Type Implementation ---------------------------===//
//
// Part of the QASM project.
//
//===----------------------------------------------------------------------===//

#include "mlir/IR/Builders.h"
#include "mlir/IR/DialectImplementation.h"
#include "llvm/ADT/TypeSwitch.h"

using namespace mlir;
#include "QASM/QASM.h"

using namespace qasm;

// The generated code for the types.
#define GET_TYPEDEF_CLASSES
#include "QASM/QASMTypes.cpp.inc"
