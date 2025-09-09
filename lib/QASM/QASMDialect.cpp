
#include "QASM/QASM.h"
using namespace mlir;
#include "mlir/IR/Operation.h"
using namespace qasm;
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/DialectImplementation.h"
#include "mlir/IR/TypeSupport.h"
#include "mlir/IR/Types.h"
#include "mlir/IR/Verifier.h"
#include "mlir/Support/LLVM.h"
#include "llvm/ADT/TypeSwitch.h"
#include "llvm/Support/Debug.h"

#define GET_TYPEDEF_CLASSES
#include "QASM/QASMTypes.cpp.inc"

//===----------------------------------------------------------------------===//
// QASMDialect
//===----------------------------------------------------------------------===//

// our types, operations, and attributes.
void QASMDialect::initialize() {
  // Register the QASM dialect's operations.
  addOperations<
#define GET_OP_LIST
#include "QASM/QASMOps.cpp.inc"
      >();
  // Register the QASM dialect's types.
  addTypes<
#define GET_TYPEDEF_LIST
#include "QASM/QASMTypes.cpp.inc"
      >();
}

#define GET_DIALECT_CLASSES
#include "QASM/QASMDialect.cpp.inc"