add_mlir_dialect(PolyqsimOps polyqsim)
add_mlir_doc(PolyqsimDialect PolyqsimDialect Polyqsim/ -gen-dialect-doc)
add_mlir_doc(PolyqsimOps PolyqsimOps Polyqsim/ -gen-op-doc)

set(LLVM_TARGET_DEFINITIONS Passes.td)
mlir_tablegen(Passes.h.inc -gen-pass-decls -name Polyqsim)
add_mlir_dialect_tablegen_target(PolyqsimPassesIncGen)
