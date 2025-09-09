#! /usr/bin/env python3
# -*- coding: utf-8 -*-
import json
import argparse
import os
from qiskit import QuantumCircuit
import qiskit.qasm2 as qasm2
from qiskit.qasm2 import loads as load_qasm2
from qiskit.circuit.controlflow import IfElseOp, WhileLoopOp, ForLoopOp


def serialize_instruction(inst, circuit=None):
    """Serialize a Qiskit CircuitInstruction (including control flow ops)."""
    op = inst.operation
    qargs = inst.qubits
    cargs = inst.clbits

    # Map Qubit -> register[index] string
    qarg_labels = []
    for q in qargs:
        if circuit:
            regs = circuit.find_bit(q).registers  # list of (register, index)
            if regs:
                reg, idx = regs[0]
                qarg_labels.append(f"{reg.name}[{idx}]")
            else:
                idx = circuit.find_bit(q).index
                qarg_labels.append(f"qubit_{idx}")
        else:
            qarg_labels.append("qubit")

    # Map Clbit -> register[index] string
    carg_labels = []
    for c in cargs:
        if circuit:
            regs = circuit.find_bit(c).registers
            if regs:
                reg, idx = regs[0]
                carg_labels.append(f"{reg.name}[{idx}]")
            else:
                idx = circuit.find_bit(c).index
                carg_labels.append(f"clbit_{idx}")
        else:
            carg_labels.append("clbit")

    # --- Case 1: IfElseOp ---
    if isinstance(op, IfElseOp):
        cond_reg, cond_val = op.condition
        true_block = op.blocks[0].data
        false_block = op.blocks[1].data if len(op.blocks) > 1 else []

        return {
            "opname": "if_else",
            "cond": {
                "register": getattr(cond_reg, "name", str(cond_reg)),
                "value": cond_val,
            },
            "true_body": [serialize_instruction(i, op.blocks[0]) for i in true_block],
            "false_body": [serialize_instruction(i, op.blocks[1]) for i in false_block],
            "qargs": qarg_labels,
            "cargs": carg_labels,
        }

    # --- Case 2: WhileLoopOp ---
    if isinstance(op, WhileLoopOp):
        cond_reg, cond_val = op.condition
        body_block = op.blocks[0].data

        return {
            "opname": "while_loop",
            "cond": {
                "register": getattr(cond_reg, "name", str(cond_reg)),
                "value": cond_val,
            },
            "body": [serialize_instruction(i, op.blocks[0]) for i in body_block],
            "qargs": qarg_labels,
            "cargs": carg_labels,
        }

    # --- Case 3: ForLoopOp ---
    if isinstance(op, ForLoopOp):
        iter_set = op.iter_set
        body_block = op.blocks[0].data

        return {
            "opname": "for_loop",
            "loop_parameter": str(op.loop_parameter),
            "iter_set": list(iter_set),
            "body": [serialize_instruction(i, op.blocks[0]) for i in body_block],
            "qargs": qarg_labels,
            "cargs": carg_labels,
        }

    # --- Default case: normal instruction ---
    return {
        "opname": op.name,
        "params": [
            float(p) if hasattr(p, "real") else str(p) for p in op.params
        ],
        "qargs": qarg_labels,
        "cargs": carg_labels,
    }


def circuit_to_json(qasm_str: str) -> str:
    """
    Convert an OpenQASM 2.0 string into a full JSON representation
    of the quantum circuit using Qiskit.
    """
    # Parse QASM string into a QuantumCircuit
    qc = load_qasm2(qasm_str, custom_instructions=qasm2.LEGACY_CUSTOM_INSTRUCTIONS)

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
        circuit_dict["operations"].append(serialize_instruction(inst, qc))

    return json.dumps(circuit_dict, indent=2)


if __name__ == "__main__":
    # Argument parser for command line usage
    parser = argparse.ArgumentParser(description="Convert QASM 2.0 file to JSON representation.")
    parser.add_argument("qasm_file", type=str, help="Path to the QASM 2.0 file.")
    args = parser.parse_args()

    with open(args.qasm_file, 'r') as f:
        qasm_code = f.read()

    json_output = circuit_to_json(qasm_code)

    # Print JSON to stdout
    print(json_output)

    # Save JSON to file with same name but .json extension
    base, _ = os.path.splitext(args.qasm_file)
    out_file = base + ".json"
    with open(out_file, "w") as f:
        f.write(json_output)

    print(f"\n✅ JSON written to {out_file}")
