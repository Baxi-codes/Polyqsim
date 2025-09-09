#! /usr/bin/env python3
# -*- coding: utf-8 -*-
import json
from qiskit import QuantumCircuit
import qiskit.qasm2 as qasm2
from qiskit.qasm2 import loads as load_qasm2
import argparse


def circuit_to_json(qasm_str: str) -> str:
    """
    Convert an OpenQASM 2.0 string into a full JSON representation
    of the quantum circuit using Qiskit.
    """
    # Parse QASM string into a QuantumCircuit
    qc = load_qasm2(qasm_str,custom_instructions=qasm2.LEGACY_CUSTOM_INSTRUCTIONS)
    # print(qc.draw())
    
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

    # Walk through circuit instructions
    for inst in qc.data:  # Each inst is a CircuitInstruction
        op = inst.operation
        qargs = inst.qubits
        cargs = inst.clbits

        # Map Qubit -> register[index] string
        qarg_labels = []
        for q in qargs:
            regs = qc.find_bit(q).registers  # list of (register, index)
            if regs:
                reg, idx = regs[0]
                qarg_labels.append(f"{reg.name}[{idx}]")
            else:
                qarg_labels.append(f"qubit_{q.index}")  # fallback

        # Map Clbit -> register[index] string
        carg_labels = []
        for c in cargs:
            regs = qc.find_bit(c).registers
            if regs:
                reg, idx = regs[0]
                carg_labels.append(f"{reg.name}[{idx}]")
            else:
                carg_labels.append(f"clbit_{c.index}")

        op_dict = {
            "opname": op.name,
            "params": [
                float(p) if hasattr(p, "real") else str(p) for p in op.params
            ],
            "qargs": qarg_labels,
            "cargs": carg_labels,
        }
        #print
        
        circuit_dict["operations"].append(op_dict)

    return json.dumps(circuit_dict, indent=2)


def process_files(filenames):
    """Process multiple QASM files, output JSONs, and create an operation frequency CSV."""
    op_freq = {}

    for filename in filenames:
        with open(filename, "r") as f:
            qasm_code = f.read()

        circuit_dict = circuit_to_json(qasm_code)

        # Count operations
        for op in circuit_dict["operations"]:
            op_name = op["name"]
            op_freq[op_name] = op_freq.get(op_name, 0) + 1

        # Write JSON file
        json_filename = os.path.splitext(filename)[0] + ".json"
        with open(json_filename, "w") as jf:
            json.dump(circuit_dict, jf, indent=2)
        print(f"✅ Wrote {json_filename}")


    
if __name__ == "__main__":
    # Argument parser for command line usage to take file name and read the QASM code
    parser = argparse.ArgumentParser(description="Convert QASM 2.0 file to JSON representation.")
    parser.add_argument("qasm_file", type=str, help="Path to the QASM 2.0 file.")
    args = parser.parse_args()
    file_list = [args.qasm_file]
    process_files(file_list)
    # with open(args.qasm_file, 'r') as f:
    #     qasm_code = f.read()
    # json_output = circuit_to_json(qasm_code)
    # print(json_output)
