#ifndef POLYQSIM_PASSES
#define POLYQSIM_PASSES

include "mlir/Pass/PassBase.td"
include "mlir/Rewrite/PassUtil.td"

def PolyqsimLowering : Pass<"polyqsim-lowering"> {
  let summary = "Lower Polyqsim operations to standard operations";
  let description = [{
    This pass lowers Polyqsim dialect operations to standard MLIR operations.
  }];
  let constructor = "polyqsim::createPolyqsimLoweringPass()";
  let dependentDialects = [
    "scf::SCFDialect",
    "arith::ArithDialect",
    "affine::AffineDialect",
    "memref::MemRefDialect",
    "complex::ComplexDialect",
    "polyqsim::PolyqsimDialect",
  ];
}

#endif // POLYQSIM_PASSES