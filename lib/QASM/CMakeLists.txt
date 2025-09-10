# # This file is for the QASM dialect.


# Add the dialect library.
add_mlir_dialect_library(MLIRQASM
  QASMDialect.cpp
  # QASMOps.cpp
  # QASMTypes.cpp

  DEPENDS MLIRQASMIncGen
  LINK_COMPONENTS
    Core
  LINK_LIBS
    MLIRFuncDialect
    MLIRSCFDialect
    MLIRArithDialect
    MLIRPass
    MLIRTransforms
    MLIRDialectUtils
    MLIRIR
)

# You may need to add more .cpp files here as you implement them.
