#include "artifacts/json/program_manifest_json.h"

#include <cstdint>
#include <ostream>
#include <sstream>
#include <string>

#include "io/json/json_writer.h"
#include "support/objc3_value_type_names.h"

namespace objc3::artifacts::json {
namespace {

using objc3::io::json::JsonObjectWriter;
using objc3::io::json::JsonArrayWriter;

std::vector<std::string> BuildParamTypeNames(const FunctionDecl &function) {
  std::vector<std::string> names;
  names.reserve(function.params.size());
  for (const FuncParam &param : function.params) {
    names.emplace_back(objc3c::support::ValueTypeName(param.type));
  }
  return names;
}

void WriteGlobalRecord(std::ostream &out, const GlobalDecl &global,
                       int resolved_value) {
  JsonObjectWriter object(out);
  object.StringField("name", global.name);
  object.IntField("value", resolved_value);
  object.UnsignedField("line", global.line);
  object.UnsignedField("column", global.column);
  object.End();
}

void WriteFunctionRecord(std::ostream &out, const FunctionDecl &function) {
  JsonObjectWriter object(out);
  object.StringField("name", function.name);
  object.SizeField("params", function.params.size());
  object.StringArrayField("param_types", BuildParamTypeNames(function));
  object.StringField("return",
                     objc3c::support::ValueTypeName(function.return_type));
  object.UnsignedField("line", function.line);
  object.UnsignedField("column", function.column);
  object.End();
}

}  // namespace

void WriteProgramGlobalsManifestArray(
    std::ostream &out, const std::vector<GlobalDecl> &globals,
    const std::vector<int> &resolved_global_values) {
  JsonArrayWriter array(out);
  for (std::size_t index = 0; index < globals.size(); ++index) {
    array.BeginElement();
    WriteGlobalRecord(out, globals[index], resolved_global_values[index]);
  }
  array.End();
}

void WriteFunctionDeclarationsManifestArray(
    std::ostream &out, const std::vector<const FunctionDecl *> &functions) {
  JsonArrayWriter array(out);
  for (const FunctionDecl *function : functions) {
    array.BeginElement();
    WriteFunctionRecord(out, *function);
  }
  array.End();
}

}  // namespace objc3::artifacts::json
