#include "ir/objc3_ir_runtime_helper_calls.h"

#include <cstddef>
#include <sstream>

namespace {

void AppendObjc3IRI32Arguments(std::ostringstream &out,
                               const std::vector<std::string> &args) {
  for (std::size_t index = 0; index < args.size(); ++index) {
    if (index != 0) {
      out << ", ";
    }
    out << "i32 " << args[index];
  }
}

}  // namespace

std::string BuildObjc3IRRuntimeI32CallLine(const std::string &result_value,
                                           const std::string &symbol,
                                           const std::vector<std::string> &args) {
  std::ostringstream out;
  out << "  " << result_value << " = call i32 @" << symbol << "(";
  AppendObjc3IRI32Arguments(out, args);
  out << ")";
  return out.str();
}

std::string BuildObjc3IRRuntimeVoidCallLine(
    const std::string &symbol, const std::vector<std::string> &args) {
  std::ostringstream out;
  out << "  call void @" << symbol << "(";
  AppendObjc3IRI32Arguments(out, args);
  out << ")";
  return out.str();
}
