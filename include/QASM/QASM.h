#ifndef QASM_QASMDIALECT_H
#define QASM_QASMDIALECT_H

#include "mlir/Bytecode/BytecodeOpInterface.h"
#include "mlir/Bytecode/BytecodeReader.h"
#include "mlir/IR/Dialect.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/Types.h"
#include "mlir/Interfaces/CallInterfaces.h"
#include "mlir/Interfaces/CastInterfaces.h"
#include "mlir/Interfaces/ControlFlowInterfaces.h"
#include "mlir/Interfaces/FunctionInterfaces.h"
#include "mlir/Interfaces/InferTypeOpInterface.h"
#include "mlir/Interfaces/LoopLikeInterface.h"
#include "mlir/Interfaces/SideEffectInterfaces.h"
#include "mlir/Transforms/InliningUtils.h"

#include "QASM/QASMDialect.h.inc"

#define GET_TYPEDEF_CLASSES
#include "QASM/QASMTypes.h.inc"

#define GET_OP_CLASSES
#include "QASM/QASMOps.h.inc"

#endif // QASM_QASMDIALECT_H
