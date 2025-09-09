//===- qasm-json-to-mlir.cpp --------------------------------------===//
//
// This file implements a translator from a JSON representation of an
// OpenQASM 2.0 quantum circuit to the QASM MLIR dialect.
//
//===----------------------------------------------------------------===//

#include "QASM/QASMDialect.h"
#include "QASM/QASMOps.h"
#include "QASM/QASMTypes.h"

#include "mlir/IR/Dialect.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/Verifier.h"
#include "mlir/Support/FileUtilities.h"
#include "mlir/Support/MlirOptMain.h"
#include "mlir/Support/ToolUtilities.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/ToolOutputFile.h"

#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <string>

using namespace mlir;
using namespace qasm;
using json = nlohmann::json;

// Define the static TypeID for the QASMDialect. This is a common
// practice for MLIR-based projects. The linker was unable to find
// the definition of this static variable.
void mlir::detail::TypeIDResolver<qasm::QASMDialect, void>::id;

// Function to register the custom QASM dialect.
void registerQASMDialect(DialectRegistry &registry) {
  registry.insert<qasm::QASMDialect>();
}

// The main function for the translator.
std::unique_ptr<ModuleOp> translateQASMJsonToMLIR(json &jsonData,
                                                  MLIRContext *context) {
  // Create an empty MLIR module.
  auto module = ModuleOp::create(UnknownLoc::get(context));
  OpBuilder builder(context);

  // Set the insertion point to the start of the module.
  builder.setInsertionPointToStart(module.getBody());

  // Store register values to use later in operations.
  std::map<std::string, Value> registers;

  // Process registers section.
  json &registersData = jsonData["registers"];

  // Process quantum registers.
  if (registersData.count("qubits") && registersData["qubits"].is_object()) {
    for (auto &[name, sizeJson] : registersData["qubits"].items()) {
      uint64_t size = sizeJson.get<uint64_t>();
      auto qRegOp = builder.create<QRegOp>(builder.getUnknownLoc(),
                                           QRegType::get(context), size);
      registers[name] = qRegOp.getResult();
    }
  }

  // Process classical registers.
  if (registersData.count("clbits") && registersData["clbits"].is_object()) {
    for (auto &[name, sizeJson] : registersData["clbits"].items()) {
      uint64_t size = sizeJson.get<uint64_t>();
      auto cRegOp = builder.create<CRegOp>(builder.getUnknownLoc(),
                                           CRegType::get(context), size);
      registers[name] = cRegOp.getResult();
    }
  }

  // Process operations.
  json &operationsData = jsonData["operations"];
  for (const auto &op : operationsData) {
    std::string opname = op["opname"].get<std::string>();

    if (opname == "sx") {
      std::string qubitName = op["qargs"][0].get<std::string>();
      if (registers.count(qubitName)) {
        builder.create<SXOp>(builder.getUnknownLoc(), QubitType::get(context),
                             registers[qubitName]);
      }
    } else if (opname == "rz") {
      std::string qubitName = op["qargs"][0].get<std::string>();
      double angle = op["params"][0].get<double>();
      if (registers.count(qubitName)) {
        builder.create<RZOp>(builder.getUnknownLoc(), QubitType::get(context),
                             builder.getF64FloatAttr(angle),
                             registers[qubitName]);
      }
    } else if (opname == "barrier") {
      std::vector<Value> qubits;
      for (const auto &qarg : op["qargs"]) {
        std::string qubitName = qarg.get<std::string>();
        if (registers.count(qubitName)) {
          qubits.push_back(registers[qubitName]);
        }
      }
      builder.create<BarrierOp>(builder.getUnknownLoc(), qubits);
    } else if (opname == "measure") {
      std::string qubitName = op["qargs"][0].get<std::string>();
      std::string cregName = op["cargs"][0].get<std::string>();
      if (registers.count(qubitName) && registers.count(cregName)) {
        builder.create<MeasureOp>(builder.getUnknownLoc(), registers[qubitName],
                                  registers[cregName]);
      }
    }
  }

  // Verify the module
  if (failed(verify(module.get()))) {
    module->emitError("Module verification failed.");
    return nullptr;
  }

  return module;
}

int main(int argc, char **argv) {
  // The main entry point for the tool.
  llvm::InitLLVM y(argc, argv);

  // Register the QASM dialect with MLIR.
  DialectRegistry registry;
  registerQASMDialect(registry);

  // Handle command line options.
  llvm::cl::opt<std::string> inputFilename(llvm::cl::Positional,
                                           llvm::cl::desc("<input json>"),
                                           llvm::cl::init("-"));

  llvm::cl::opt<std::string> outputFilename(
      "o", llvm::cl::desc("Output filename"), llvm::cl::value_desc("filename"),
      llvm::cl::init("-"));

  llvm::cl::ParseCommandLineOptions(argc, argv);

  // Read the input JSON file.
  std::ifstream inputFile(inputFilename);
  if (!inputFile.is_open()) {
    llvm::errs() << "Could not open file: " << inputFilename << "\n";
    return 1;
  }
  json jsonData;
  inputFile >> jsonData;

  // Create an MLIR context and load the dialects.
  MLIRContext context(registry);

  // Create the MLIR module by translating the JSON.
  auto module = translateQASMJsonToMLIR(jsonData, &context);
  if (!module) {
    return 1;
  }

  // Print the MLIR module to the output file.
  std::error_code EC;
  auto outputFile = std::make_unique<llvm::ToolOutputFile>(
      outputFilename.getValue(), EC, llvm::sys::fs::OF_None);
  if (EC) {
    llvm::errs() << EC.message() << "\n";
    return 1;
  }

  module->print(outputFile->os());

  outputFile->keep();

  return 0;
}
