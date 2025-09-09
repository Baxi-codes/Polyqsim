
set(LLVM_TARGET_DEFINITIONS QASMOps.td)
mlir_tablegen(QASMOps.h.inc -gen-op-decls)
mlir_tablegen(QASMOps.cpp.inc -gen-op-defs)
mlir_tablegen(QASMOpsDialect.h.inc -gen-dialect-decls -dialect=qasm)
mlir_tablegen(QASMOpsDialect.cpp.inc -gen-dialect-defs -dialect=qasm)

set(LLVM_TARGET_DEFINITIONS QASMTypes.td)
mlir_tablegen(QASMTypes.h.inc -gen-typedef-decls -typedefs-dialect=qasm)
mlir_tablegen(QASMTypes.cpp.inc -gen-typedef-defs -typedefs-dialect=qasm)

set(LLVM_TARGET_DEFINITIONS QASMBase.td)
mlir_tablegen(QASMDialect.h.inc -gen-dialect-decls -dialect=qasm)
mlir_tablegen(QASMDialect.cpp.inc -gen-dialect-defs -dialect=qasm)
add_public_tablegen_target(QASMTypesIncGen)
add_public_tablegen_target(MLIRQASMIncGen)
add_dependencies(mlir-headers MLIRQASMIncGen)
