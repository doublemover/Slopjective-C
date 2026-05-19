#include "artifacts/json/program_manifest_json.h"

#include <cstddef>
#include <ostream>

#include "artifacts/json/program_manifest_records.h"
#include "io/json/json_writer.h"

namespace objc3::artifacts::json {
namespace {

using objc3::io::json::JsonArrayWriter;

}  // namespace

void WriteProgramGlobalsManifestArray(
    std::ostream &out, const std::vector<GlobalDecl> &globals,
    const std::vector<int> &resolved_global_values) {
  JsonArrayWriter array(out);
  for (std::size_t index = 0; index < globals.size(); ++index) {
    array.BeginElement();
    WriteProgramGlobalManifestRecord(out, globals[index],
                                     resolved_global_values[index]);
  }
  array.End();
}

void WriteFunctionDeclarationsManifestArray(
    std::ostream &out, const std::vector<const FunctionDecl *> &functions) {
  JsonArrayWriter array(out);
  for (const FunctionDecl *function : functions) {
    array.BeginElement();
    WriteProgramFunctionManifestRecord(out, *function);
  }
  array.End();
}

}  // namespace objc3::artifacts::json
