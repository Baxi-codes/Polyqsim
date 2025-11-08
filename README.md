# Polyqsim

**Polyqsim** is a frontend tool that translates **QASM2** quantum assembly code into **MLIR Affine dialect** for efficient **statevector simulation**.  
It enables integration of quantum program representations into MLIR’s polyhedral optimisation infrastructure.


Example:

```qasm
OPENQASM 2.0;
include "qelib1.inc";
qreg q[16];
creg ans[16];
rz(-0.79715247) q[0];
sx q[0];
rz(-0.55272686) q[0];
sx q[0];
rz(1.9734881) q[0];
rz(-0.79715247) q[1];
sx q[1];
rz(-0.55272686) q[1];
sx q[1];
rz(-1.1681045) q[1];
cx q[0],q[1];
rz(pi/2) q[0];
sx q[0];
rz(7*pi/5) q[0];
...
```
is lowered to affine MLIR:
```mlir
module {
  memref.global "private" @statevector : memref<65536xcomplex<f32>>
  func.func @main() -> i32 attributes {num_qubits = 16 : i32} {
    %cst = complex.constant [0.000000e+00 : f32, 0.000000e+00 : f32] : complex<f32>
    %cst_0 = complex.constant [1.000000e+00 : f32, 0.000000e+00 : f32] : complex<f32>
    %c0_i32 = arith.constant 0 : i32
    %0 = memref.get_global @statevector : memref<65536xcomplex<f32>>
    affine.for %arg0 = 0 to 65536 step 2 {
      affine.for %arg1 = #map(%arg0) to #map1(%arg0) {
        %1 = affine.load %0[%arg1] : memref<65536xcomplex<f32>>
        %2 = affine.load %0[%arg1 + 1] : memref<65536xcomplex<f32>>
        %3 = complex.mul %cst_0, %1 : complex<f32>
        %4 = complex.mul %cst, %2 : complex<f32>
        %5 = complex.add %3, %4 : complex<f32>
        %6 = complex.mul %cst, %1 : complex<f32>
        %7 = complex.mul %cst_0, %2 : complex<f32>
...
```

It is still a work in progress (Next step: multi-qubit gate lowering) 

## Build Instructions

### 1. Set up the environment

Create and activate the conda environment:
```sh
conda env create -f environment.yml
conda activate polyqsim
````

### 2. Build dependencies

This setup assumes you have built **LLVM** and **MLIR** in `$BUILD_DIR` and installed them to `$PREFIX`.

When building LLVM, make sure to include:

```sh
- DLLVM_INSTALL_UTILS=ON
```

This ensures `FileCheck` and related tools are installed.

### 3. Build Polyqsim

```sh
mkdir build && cd build
cmake -G Ninja .. -DMLIR_DIR=$PREFIX/lib/cmake/mlir
cmake --build .
```

##  Tools Overview

Polyqsim provides two primary command-line tools, modeled after `mlir-translate` and `mlir-opt`:

###  1. `polyqsim-translate`

Translates OpenQASM 2.0 programs into the **Polyqsim dialect** in MLIR.

#### Usage

```bash
polyqsim-translate --to-polyqsim input.qasm -o output.mlir
```
---

###  2. `polyqsim-opt`

Applies Polyqsim-specific passes and lowers the dialect to **Affine** or **Standard** MLIR operations.

#### Usage

```bash
polyqsim-opt --polyqsim-lowering input.polyqsim.mlir -o lowered.mlir
```

#### Combined Pipeline Example

You can chain the tools together:

```bash
polyqsim-translate --to-polyqsim input.qasm | polyqsim-opt --polyqsim-lowering > final.mlir
```


##  License

This project is distributed under the **MIT License**.
See the [LICENSE](LICENSE) file for details.



