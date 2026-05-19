#include "artifacts/objc3_frontend_metaprogramming_semantic_artifacts.h"

#include <sstream>
#include <vector>

#include "artifacts/objc3_frontend_artifact_semantic_contract_records.h"
#include "io/json/json_writer.h"
#include "io/objc3_json.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  return objc3::io::json::RenderJsonStringArray(values);
}

}  // namespace

#include "artifacts/objc3_frontend_metaprogramming_semantic_json_expansion_behavior.inc"
#include "artifacts/objc3_frontend_metaprogramming_semantic_json_derive_inventory.inc"
#include "artifacts/objc3_frontend_metaprogramming_semantic_json_macro_safety.inc"
#include "artifacts/objc3_frontend_metaprogramming_semantic_json_property_behavior.inc"
#include "artifacts/objc3_frontend_metaprogramming_semantic_json_expansion_contracts.inc"

}  // namespace objc3::artifacts::frontend
