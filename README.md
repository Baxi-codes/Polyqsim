# Polyqsim

**Polyqsim** is a frontend tool that translates **QASM2** quantum assembly code into **MLIR Affine dialect** for efficient **statevector simulation**.  
It enables integration of quantum program representations into MLIR’s polyhedral optimisation infrastructure.

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

#### Example

**Input:** `bell.qasm`

```qasm
OPENQASM 2.0;
qreg q[2];
h q[0];
cx q[0], q[1];
```

**Command:**

```bash
polyqsim-translate --to-polyqsim bell.qasm -o bell.polyqsim.mlir
```

**Output (Polyqsim Dialect):**

```mlir
module {
  polyqsim.qreg "q" : i64 -> !polyqsim.qreg<2>
  polyqsim.h %q[0] : !polyqsim.qubit
  polyqsim.cx %q[0], %q[1] : !polyqsim.qubit, !polyqsim.qubit
}
```

---

###  2. `polyqsim-opt`

Applies Polyqsim-specific passes and lowers the dialect to **Affine** or **Standard** MLIR operations.

#### Usage

```bash
polyqsim-opt --polyqsim-lowering input.polyqsim.mlir -o lowered.mlir
```

**Output (Lowered MLIR):**

```mlir
func.func @bell() {
  %c0 = arith.constant 0 : index
  affine.for %i = 0 to 2 {
    // Representation of quantum operations in affine loops
  }
  return
}
```

#### Combined Pipeline Example

You can chain the tools together:

```bash
polyqsim-translate --to-polyqsim input.qasm | polyqsim-opt --polyqsim-lowering > final.mlir
```


##  License

This project is distributed under the **MIT License**.
See the [LICENSE](LICENSE) file for details.



