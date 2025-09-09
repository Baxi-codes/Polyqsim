//===- qasm-json-to-mlir.cpp --------------------------------------===//
//
// This file implements a translator from a JSON representation of an
// OpenQASM 2.0 quantum circuit to the QASM MLIR dialect.
//
//===----------------------------------------------------------------===//

#include "QASM/QASM.h"
// #include "QASM/QASM.h"
// #include "QASM/QASMDialect.h"
// #include "QASM/QASMOps.h"
// #include "QASM/QASMTypes.h"

#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/Dialect.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/Verifier.h"
#include "mlir/Parser/Parser.h"
#include "mlir/Support/FileUtilities.h"
#include "mlir/Support/ToolUtilities.h"
#include "mlir/Tools/mlir-opt/MlirOptMain.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_ostream.h"

#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <string>

using namespace mlir;
using namespace qasm;
using json = nlohmann::json;
#include <fstream>
#include <iostream>
#include <map>
#include <string>

using namespace mlir;
using namespace qasm;
using json = nlohmann::json;

// Function to register the custom QASM dialect.
void registerQASMDialect(DialectRegistry &registry) {
  registry.insert<qasm::QASMDialect>();
}

// The main function for the translator.
ModuleOp translateQASMJsonToMLIR(json &jsonData, MLIRContext *context) {
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
      auto qRegOp = builder.create<QRegOp>(
          builder.getUnknownLoc(),
          QRegType::get(context, sizeJson.get<uint64_t>()),
          sizeJson.get<uint64_t>());
      registers[name] = qRegOp.getResult();
      std::cout << "Created quantum register: " << name << " of size "
                << sizeJson.get<uint64_t>() << "\n";
      qRegOp->dump();
    }
  }
  // return module;

  // Process classical registers.
  if (registersData.count("clbits") && registersData["clbits"].is_object()) {
    for (auto &[name, sizeJson] : registersData["clbits"].items()) {
      auto cRegOp = builder.create<CRegOp>(
          builder.getUnknownLoc(),
          CRegType::get(context, sizeJson.get<uint64_t>()),
          sizeJson.get<uint64_t>());
      registers[name] = cRegOp.getResult();
      std::cout << "Created classical register: " << name << " of size "
                << sizeJson.get<uint64_t>() << "\n";
      cRegOp->dump();
    }
  }

  // Process operations.
  json &operationsData = jsonData["operations"];
  for (const auto &op : operationsData) {
    std::string opname = op["opname"].get<std::string>();

    if (opname == "sx") {
      std::string qubitName = op["qargs"][0].get<std::string>();
      if (registers.count(qubitName)) {
        auto sxOp =
            builder.create<SXOp>(builder.getUnknownLoc(),
                                 QubitType::get(context), registers[qubitName]);
        std::cout << "Created SX operation on qubit: " << qubitName << "\n";
        sxOp->dump();
      }
    } else if (opname == "rz") {
      std::string qubitName = op["qargs"][0].get<std::string>();
      double angle = op["params"][0].get<double>();
      if (registers.count(qubitName)) {
        auto rzop = builder.create<RZOp>(
            builder.getUnknownLoc(), QubitType::get(context),
            registers[qubitName], builder.getF64FloatAttr(angle));
        std::cout << "Created RZ operation on qubit: " << qubitName
                  << " with angle: " << angle << "\n";
        rzop->dump();
      }
    }
    // } else if (opname == "barrier") {
    //   std::vector<Value> qubits;
    //   for (const auto &qarg : op["qargs"]) {
    //     std::string qubitName = qarg.get<std::string>();
    //     if (registers.count(qubitName)) {
    //       qubits.push_back(registers[qubitName]);
    //     }
    //   }
    //   auto barrier = builder.create<BarrierOp>(builder.getUnknownLoc(),
    //   qubits); barrier->dump();
    // } else if (opname == "measure") {
    //   std::string qubitName = op["qargs"][0].get<std::string>();
    //   std::string cregName = op["cargs"][0].get<std::string>();
    //   if (registers.count(qubitName) && registers.count(cregName)) {
    //     builder.create<MeasureOp>(builder.getUnknownLoc(),
    //     registers[qubitName],
    //                               registers[cregName]);
    //   }
    //   auto measure = builder.create<MeasureOp>(
    //       builder.getUnknownLoc(), registers[qubitName],
    //       registers[cregName]);
    //   measure->dump();
    // }
  }

  // Verify the module
  if (failed(verify(module))) {
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
  // The corrected line: pass the registry to the context.
  MLIRContext context(registry);
  context.loadDialect<qasm::QASMDialect>();
  context.loadDialect<mlir::func::FuncDialect>();
  context.loadDialect<mlir::scf::SCFDialect>();
  context.loadDialect<mlir::arith::ArithDialect>();

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

  // module->dump();
  // module->print(outputFile->os());

  // outputFile->keep();

  return 0;
}
