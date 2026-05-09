#include "artifacts/json/program_manifest_records.h"

#include <string>
#include <vector>

#include "io/json/json_writer.h"
#include "support/objc3_value_type_names.h"

namespace objc3::artifacts::json {
namespace {

std::vector<std::string> BuildParamTypeNames(const FunctionDecl &function) {
  std::vector<std::string> names;
  names.reserve(function.params.size());
  for (const FuncParam &param : function.params) {
    names.emplace_back(objc3c::support::ValueTypeName(param.type));
  }
  return names;
}

}  // namespace

void WriteProgramGlobalManifestRecord(std::ostream &out,
                                      const GlobalDecl &global,
                                      int resolved_value) {
  objc3::io::json::JsonObjectWriter object(out);
  object.StringField("name", global.name);
  object.IntField("value", resolved_value);
  object.UnsignedField("line", global.line);
  object.UnsignedField("column", global.column);
  object.End();
}

void WriteProgramFunctionManifestRecord(std::ostream &out,
                                        const FunctionDecl &function) {
  objc3::io::json::JsonObjectWriter object(out);
  object.StringField("name", function.name);
  object.SizeField("params", function.params.size());
  object.StringArrayField("param_types", BuildParamTypeNames(function));
  object.StringField("return",
                     objc3c::support::ValueTypeName(function.return_type));
  object.UnsignedField("line", function.line);
  object.UnsignedField("column", function.column);
  object.End();
}

}  // namespace objc3::artifacts::json
