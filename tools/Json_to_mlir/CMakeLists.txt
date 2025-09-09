# cmake_minimum_required(VERSION 3.16.3)
# project(qasm-json-to-mlir CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the nlohmann/json library.
find_package(nlohmann_json REQUIRED)

add_executable(qasm-json-to-mlir
  qasm-json-to-mlir.cpp
)

# Set up the include directories.
target_include_directories(qasm-json-to-mlir PUBLIC
  ${CMAKE_SOURCE_DIR}/include/QASM
)
set(LLVM_COMPONENTS
  Core
  Support
  # IRReader
  # JIT
  # Add the components for your native target
  # Example for x86-64:
  # X86Disassembler
  # X86AsmParser
  # X86AsmPrinter
  # X86CodeGen
  # X86Info
  # X86Target
)
set(NO_RTTI "-fno-rtti")
add_definitions(${NO_RTTI})

llvm_map_components_to_libnames(llvm_libs ${LLVM_COMPONENTS}) 

# Link the libraries.
target_link_libraries(qasm-json-to-mlir PRIVATE
  nlohmann_json::nlohmann_json
  MLIRIR
  MLIRSupport
  MLIRParser
  MLIRFuncDialect
  MLIRFuncDialect
  MLIRFuncTransforms     
  MLIRSCFDialect
  MLIRArithDialect
  MLIRPass
  MLIRTransforms
  MLIRDialectUtils
  # MLIRTools
  MLIRQASM # Link your custom dialect library
  ${llvm_libs}
  LLVMSupport
)

# Add dependencies to ensure that the dialect library is built before the translator.
add_dependencies(qasm-json-to-mlir MLIRQASM)
