#include <pybind11/embed.h>
#include "pybridge/pybridge.h"
#include "pybridge/python-config.h"

namespace py = pybind11;

// Simple wrapper: call Python function from absolute path
std::string simplifyQuantumCircuit(const std::string &input) {
  py::scoped_interpreter guard{}; // start interpreter

  // Absolute path to folder containing my_pycode.py
  std::string absPath = PY_SCRIPT_DIR;

  // Add abs path to sys.path
  py::module_ sys = py::module_::import("sys");
  sys.attr("path").attr("append")(absPath);

  // Import module
  py::object myModule = py::module_::import("simplify");

  // Call function
  py::object result = myModule.attr("simplify_qasm")(input);

  // Convert back to C++ string
  return result.cast<std::string>();
}
