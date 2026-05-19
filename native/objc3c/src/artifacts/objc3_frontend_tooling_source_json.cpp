#include "artifacts/objc3_frontend_tooling_source_artifacts.h"

#include <sstream>
#include <string>
#include <vector>

#include "io/json/json_writer.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  return objc3::io::json::RenderJsonStringArray(values);
}

}  // namespace

#include "artifacts/objc3_frontend_tooling_source_json_inventory.inc"
#include "artifacts/objc3_frontend_tooling_source_json_canonicalization_completion.inc"
#include "artifacts/objc3_frontend_tooling_source_json_diagnostic_taxonomy.inc"
#include "artifacts/objc3_frontend_tooling_source_json_fixit_synthesis.inc"
#include "artifacts/objc3_frontend_tooling_source_json_legacy_canonical_migration.inc"

}  // namespace objc3::artifacts::frontend
