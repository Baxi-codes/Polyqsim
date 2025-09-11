import qiskit.circuit as circuit
from qiskit import qasm2, transpile
import sys

filtered_ops = ["rz", "cx", "u", "sx", "x", "reset"]

def simplify_qasm(qasm_str):
  simplified = "comment BEGIN_PREAMBLE\n"
  qc = qasm2.loads(qasm_str, custom_instructions=qasm2.LEGACY_CUSTOM_INSTRUCTIONS)
  # qc = transpile(qc)
  # print(qasm2.dumps(qc))
  # float(a)
  simplified += f"num_qubits {qc.num_qubits}\n"
  simplified += "comment END_PREAMBLE\n"
  # if True:
  #   qc.
  skipped = set()
  for inst in qc.data:
    op : circuit.Instruction = inst.operation
    qargs = inst.qubits
    cargs = inst.clbits
    if op.name not in filtered_ops:
      if op.name not in skipped:
        # simplified += f"comment skipping {op.name}\n"
        skipped.add(op.name)
      continue
    qarg_idxs = []
    params = []
    for q in qargs:
      regs = qc.find_bit(q).registers  # list of (register, index)
      if regs:
        _, idx = regs[0]
        qarg_idxs.append(idx)
      else:
        simplified += "error could not find qubit register\n"
    for p in op.params:
      params.append(float(p))
    param_str = " ".join([str(p) for p in params])
    qarg_str = " ".join([str(q) for q in qarg_idxs])
    simplified += f"{op.name} PARAMS {param_str} QARGS {qarg_str} END\n"
  print(f"Skipped ops: {skipped}")
  return simplified


if __name__ == "__main__":
  print(simplify_qasm(sys.argv[1]))