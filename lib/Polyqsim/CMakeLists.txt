add_subdirectory(Passes)
add_mlir_dialect_library(MLIRPolyqsim
        PolyqsimDialect.cpp
        PolyqsimOps.cpp

        ADDITIONAL_HEADER_DIRS
        ${PROJECT_SOURCE_DIR}/include/Polyqsim

        DEPENDS
        MLIRPolyqsimOpsIncGen

	LINK_LIBS PUBLIC
	MLIRIR
        MLIRInferTypeOpInterface
	)
