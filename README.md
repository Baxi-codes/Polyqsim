# An out-of-tree dialect template for MLIR

This repository contains a template for an out-of-tree [MLIR](https://mlir.llvm.org/) dialect as well as a
polyqsim `opt`-like tool to operate on that dialect.

## Conda env setup
This project uses the qiskit python library for qasm parsing.
To set up the conda env please do the following.
```sh
conda create --name Polyqsim_env python=3.12
conda activate Polyqsim_env
conda install pip
pip install qiskit
```


## How to build

This setup assumes that you have built LLVM and MLIR in `$BUILD_DIR` and installed them to `$PREFIX`. To build and launch the tests, run
```sh
mkdir build && cd build
cmake -G Ninja .. -DMLIR_DIR=$PREFIX/lib/cmake/mlir -DLLVM_EXTERNAL_LIT=$BUILD_DIR/bin/llvm-lit
cmake --build . --target check-polyqsim-opt
```
To build the documentation from the TableGen description of the dialect
operations, run
```sh
cmake --build . --target mlir-doc
```
**Note**: Make sure to pass `-DLLVM_INSTALL_UTILS=ON` when building LLVM with
CMake so that it installs `FileCheck` to the chosen installation prefix.

## Dialect Idea
```mlir
module {
  func.func @main() {
    %0 = qasm.qreg qreg<4> : <4>
    %1 = qasm.qreg qreg<1> : <1>
  }
}
```
## License

This dialect template is made available under the Apache License 2.0 with LLVM Exceptions. See the `LICENSE.txt` file for more details.
