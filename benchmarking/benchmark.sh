#! /usr/bin/env bash
set -e
set -o pipefail

# Check if the correct number of arguments is provided
if [[ $# -ne 3 ]]; then
echo "Usage: $0 <mlir directory> <output directory for results> <input mlir file>"
exit 1
fi

#get current script directory
SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
echo "Script directory: $SCRIPT_DIR"


# Assign command-line arguments to variables
MLIR_DIR=$1
OUTPUT_DIR=$2

# Create the output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

# MLIR tool paths and options
MLIR_ROOT="$MLIR_DIR/bin"
MLIR_OPT="$MLIR_ROOT/mlir-opt"

MLIR_OPT_OPTIONS="--lower-affine --expand-strided-metadata --convert-scf-to-cf --convert-cf-to-llvm --convert-math-to-llvm --llvm-request-c-wrappers --convert-func-to-llvm --normalize-memrefs --memref-expand --finalize-memref-to-llvm --convert-complex-to-standard --lower-sparse-foreach-to-scf --convert-complex-to-llvm --convert-arith-to-llvm --reconcile-unrealized-casts --llvm-legalize-for-export"

MLIR_ADV_OPT_OPTIONS=" $MLIR_OPT_OPTIONS --affine-loop-tile --affine-super-vectorize"
# MLIR_ADV_OPT_OPTIONS="--polyqsim-lowering --canonicalize --cse"

MLIR_TRANSLATE="$MLIR_ROOT/mlir-translate"
MLIR_TRANSLATE_OPTIONS="--mlir-to-llvmir"

CLANG="$MLIR_ROOT/clang"
CLANG_OPTIONS="-O3"

# Define input and output filenames
INPUT_FILE=$3
BENCHMARK_DEFALUT_MLIR="$OUTPUT_DIR/benchmark.default.mlir"
BENCHMARK_DEFAULT_LL="$OUTPUT_DIR/benchmark.default.ll"
BENCHMARK_DEFAULT_EXEC="$OUTPUT_DIR/benchmark.default.run"

# Define optimized output filenames
BENCHMARK_OPTIMIZED_MLIR="$OUTPUT_DIR/benchmark.optimized.mlir"
BENCHMARK_OPTIMIZED_LL="$OUTPUT_DIR/benchmark.optimized.ll"
BENCHMARK_OPTIMIZED_EXEC="$OUTPUT_DIR/benchmark.optimized.run"

# --- Execution Steps ---

# 1. Run mlir-opt
echo "Running mlir-opt..."
"$MLIR_OPT" $MLIR_OPT_OPTIONS "$INPUT_FILE" -o "$BENCHMARK_DEFALUT_MLIR"
"$MLIR_OPT" $MLIR_ADV_OPT_OPTIONS "$INPUT_FILE" -o "$BENCHMARK_OPTIMIZED_MLIR"

echo "-----------------------------------"

# 2. Run mlir-translate
echo "Running mlir-translate..."
"$MLIR_TRANSLATE" $MLIR_TRANSLATE_OPTIONS "$BENCHMARK_DEFALUT_MLIR" -o "$BENCHMARK_DEFAULT_LL"
"$MLIR_TRANSLATE" $MLIR_TRANSLATE_OPTIONS "$BENCHMARK_OPTIMIZED_MLIR" -o "$BENCHMARK_OPTIMIZED_LL"

echo "-----------------------------------"

# 3. Run clang
echo "Running clang..."
"$CLANG" $CLANG_OPTIONS "$BENCHMARK_DEFAULT_LL" -o "$BENCHMARK_DEFAULT_EXEC"
"$CLANG" $CLANG_OPTIONS "$BENCHMARK_OPTIMIZED_LL" -o "$BENCHMARK_OPTIMIZED_EXEC"

# echo "-----------------------------------"

# # 4. Run the executable and measure time
# echo "Running the executable and measuring time..."
# /usr/bin/time -v "$BENCHMARK_EXEC" &> "$OUTPUT_DIR/benchmark.default.execution.log"
# /usr/bin/time -v "$BENCHMARK_OPTIMIZED_EXEC" &> "$OUTPUT_DIR/benchmark.optimized.execution.log"

# echo "-----------------------------------"

# # 5.compare the outputs
# echo "Comparing outputs..."
# diff "$BENCHMARK_EXEC" "$BENCHMARK_OPTIMIZED_EXEC" > "$OUTPUT_DIR/diff.log" 
# echo "-----------------------------------"

echo "Outputs differ. See diff.log for details."

echo "Execution complete. Logs are saved in the output directory."
echo "Script execution complete. Output files are in: $OUTPUT_DIR"