find_package(pybind11 REQUIRED)

add_library(pybridge STATIC pybridge.cpp)
target_link_libraries(pybridge PRIVATE pybind11::embed)
target_compile_options(pybridge PRIVATE -fexceptions -frtti)
target_include_directories(pybridge PRIVATE ${CMAKE_CURRENT_BINARY_DIR})