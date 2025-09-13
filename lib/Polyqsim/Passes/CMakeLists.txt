add_mlir_conversion_library(PolyqsimPasses
  PolyqsimLowering.cpp

  ADDITIONAL_HEADER_DIRS
        ${PROJECT_SOURCE_DIR}/include/Polyqsim/

  DEPENDS
  PolyqsimPassesIncGen


  LINK_LIBS PUBLIC
  MLIRAffineDialect
  MLIRComplexDialect
  MLIRSCFDialect
  MLIRArithDialect
  MLIRMemRefDialect
  MLIRAnalysis
  MLIRPass
  MLIRTransforms
  MLIRTransformUtils
  MLIRIR
  MLIRFuncDialect
  MLIRSupport
  MLIRAffineToStandard
  MLIRAffineTransforms
)