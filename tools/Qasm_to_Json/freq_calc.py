import json
import csv
import os
import argparse
from qiskit.qasm2 import loads as load_qasm2
import qiskit.qasm2 as qasm2


def circuit_to_dict(qasm_str: str):
    """Convert an OpenQASM 2.0 string into a dictionary representation of the quantum circuit."""
    qc = load_qasm2(qasm_str,custom_instructions=qasm2.LEGACY_CUSTOM_INSTRUCTIONS)

    circuit_dict = {
        "qasm_version": "2.0",
        "header": {
            "name": qc.name,
            "global_phase": float(qc.global_phase),
            "metadata": qc.metadata if qc.metadata else {}
        },
        "registers": {
            "qubits": {reg.name: reg.size for reg in qc.qregs},
            "clbits": {reg.name: reg.size for reg in qc.cregs}
        },
        "operations": []
    }

    # Walk through instructions
    for inst in qc.data:  # Each inst is a CircuitInstruction
        op = inst.operation
        qargs = inst.qubits
        cargs = inst.clbits

        # Map Qubit -> register[index]
        qarg_labels = []
        for q in qargs:
            regs = qc.find_bit(q).registers  # list of (register, index)
            if regs:
                reg, idx = regs[0]
                qarg_labels.append(f"{reg.name}[{idx}]")
            else:
                qarg_labels.append(f"qubit_{q.index}")

        # Map Clbit -> register[index]
        carg_labels = []
        for c in cargs:
            regs = qc.find_bit(c).registers
            if regs:
                reg, idx = regs[0]
                carg_labels.append(f"{reg.name}[{idx}]")
            else:
                carg_labels.append(f"clbit_{c.index}")

        op_dict = {
            "name": op.name,
            "params": [
                float(p) if hasattr(p, "real") else str(p) for p in op.params
            ],
            "qargs": qarg_labels,
            "cargs": carg_labels,
        }
        circuit_dict["operations"].append(op_dict)

    return circuit_dict


def process_files(filenames, csv_out):
    """Process multiple QASM files, output JSONs, and create an operation frequency CSV."""
    op_freq = {}

    for filename in filenames:
        with open(filename, "r") as f:
            qasm_code = f.read()

        circuit_dict = circuit_to_dict(qasm_code)

        # Count operations
        for op in circuit_dict["operations"]:
            op_name = op["name"]
            op_freq[op_name] = op_freq.get(op_name, 0) + 1

        # Write JSON file
        json_filename = os.path.splitext(filename)[0] + ".json"
        with open(json_filename, "w") as jf:
            json.dump(circuit_dict, jf, indent=2)
        print(f"✅ Wrote {json_filename}")

    # Write CSV summary
    with open(csv_out, "w", newline="") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["operation", "frequency"])
        for op, freq in sorted(op_freq.items()):
            writer.writerow([op, freq])
    print(f"✅ Wrote {csv_out}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Convert QASM2 files into JSON and generate operation frequency CSV."
    )
    parser.add_argument(
        "files",
        nargs="+",
        help="Input QASM files"
    )
    parser.add_argument(
        "--csv",
        default="operation_frequencies.csv",
        help="Output CSV filename (default: operation_frequencies.csv)"
    )

    args = parser.parse_args()
    process_files(args.files, args.csv)
