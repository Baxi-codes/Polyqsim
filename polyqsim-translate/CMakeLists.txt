get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
get_property(conversion_libs GLOBAL PROPERTY MLIR_CONVERSION_LIBS)
set(LIBS
        ${dialect_libs}
        ${conversion_libs}
        MLIROptLib
        MLIRPolyqsim
        )
add_llvm_executable(polyqsim-translate polyqsim-translate.cpp)

llvm_update_compile_flags(polyqsim-translate)
target_link_libraries(polyqsim-translate PRIVATE ${LIBS} pybridge)
target_include_directories(polyqsim-translate PRIVATE ${CMAKE_CURRENT_BINARY_DIR})