//===- QASMOps.cpp - QASM Operations Implementation -----------------------===//
//
// Part of the QASM project.
//
//===----------------------------------------------------------------------===//

#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/DialectImplementation.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/TypeSupport.h"
#include "mlir/IR/Types.h"
#include "mlir/Support/LLVM.h"
#include "llvm/ADT/TypeSwitch.h"
using namespace mlir;

#include "QASM/QASM.h"
using namespace qasm;

#include "mlir/IR/OpImplementation.h"

// Define a helper function to create a zero-initialised value.
static mlir::Attribute ZeroInitValue(mlir::Type type) {
  if (auto integerType = mlir::dyn_cast<mlir::IntegerType>(type))
    return mlir::IntegerAttr::get(integerType, 0);

  return {};
}

// The generated code for the operations themselves.
#define GET_OP_CLASSES
#include "QASM/QASMOps.cpp.inc"

//===----------------------------------------------------------------------===//