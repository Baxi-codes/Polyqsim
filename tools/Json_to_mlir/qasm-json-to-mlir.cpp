//===- qasm-json-to-mlir.cpp --------------------------------------===//
//
// This file implements a translator from a JSON representation of an
// OpenQASM 2.0 quantum circuit to the QASM MLIR dialect.
//
//===----------------------------------------------------------------===//

#include "QASM/QASM.h"

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

  // Create a main function and add an entry block.
  auto funcOp = builder.create<mlir::func::FuncOp>(
      builder.getUnknownLoc(), "main", builder.getFunctionType({}, {}));

  // Set the insertion point to the start of the function's entry block.
  // This is the key fix to place all subsequent ops inside the function.
  builder.setInsertionPointToStart(funcOp.addEntryBlock());

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
      // registers[name] = qRegOp.getResult();
      std::cout << "Created quantum register: " << name << " of size "
                << sizeJson.get<uint64_t>() << "\n";
      qRegOp->dump();
      // here extract all the qubits from the qreg and store them in the
      // registers map
      // for (uint64_t i = 0; i < sizeJson.get<uint64_t>(); ++i) {
      //   auto qubit = builder.create<QRegExtractOp>(
      //       builder.getUnknownLoc(), QubitType::get(context), qRegOp, i);
      //   std::string qubitName = name + "[" + std::to_string(i) + "]";
      //   registers[qubitName] = qubit.getResult();
      //   std::cout << "Created qubit: " << qubitName << "\n";
      //   qubit->dump();
      // }
    }
  }

  // Process classical registers.
  if (registersData.count("clbits") && registersData["clbits"].is_object()) {
    for (auto &[name, sizeJson] : registersData["clbits"].items()) {
      auto cRegOp = builder.create<CRegOp>(
          builder.getUnknownLoc(),
          CRegType::get(context, sizeJson.get<uint64_t>()),
          sizeJson.get<uint64_t>());
      // registers[name] = cRegOp.getResult();
      // std::cout << "Created classical register: " << name << " of size "
      //           << sizeJson.get<uint64_t>() << "\n";
      // cRegOp->dump();

      // here extract all the cregs from the creg and store them in the
      // registers map

      // for (uint64_t i = 0; i < sizeJson.get<uint64_t>(); ++i) {
      //   auto creg = builder.create<CRegExtractOp>(
      //       builder.getUnknownLoc(), CBitType::get(context), cRegOp, i);
      //   std::string cregName = name + "[" + std::to_string(i) + "]";
      //   registers[cregName] = creg.getResult();
      //   std::cout << "Created creg: " << cregName << "\n";
      //   creg->dump();
      // }
    }
  }
  std::cout << "Total registers created: " << registers.size() << "\n";
  // Process operations.
  module->dump();
  json &operationsData = jsonData["operations"];
  // check if operationsData is an array
  if (!operationsData.is_array()) {
    llvm::errs() << "Expected 'operations' to be an array.\n";
    return nullptr;
  }
  operationsData.dump();
  std::cout << "Total operations to process: " << operationsData.size() << "\n";
  for (const auto &op : operationsData) {
    std::string temp;
    std::cin >> temp;
    std::string opname = op["opname"].get<std::string>();
    std::cout << "Processing operation: " << op.dump(4) << "\n";
    std::cout << "Operation name: " << opname << "\n";
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
        // rzop->dump();
      }
    } else if (opname == "measure") {
      std::string qubitName = op["qargs"][0].get<std::string>();
      std::string cregName = op["cargs"][0].get<std::string>();
      if (registers.count(qubitName) && registers.count(cregName)) {
        auto measure = builder.create<MeasureOp>(
            builder.getUnknownLoc(), registers[qubitName], registers[cregName]);
        std::cout << "Created Measure operation on qubit: " << qubitName
                  << " and classical register: " << cregName << "\n";
        // measure->dump();
      }
    }
  }

  // Add the required return operation to the end of the function.
  builder.create<mlir::func::ReturnOp>(builder.getUnknownLoc());

  // Verify the module.
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
  try {
    inputFile >> jsonData;
  } catch (json::parse_error &e) {
    llvm::errs() << "JSON parse error: " << e.what() << "\n";
    return 1;
  }

  // Create an MLIR context and load the dialects.
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
  module->dump();
  // module->print(outputFile->os());
  // outputFile->keep();

  return 0;
}
