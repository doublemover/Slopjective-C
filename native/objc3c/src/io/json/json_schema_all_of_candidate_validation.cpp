#include "io/json/json_schema_all_of_candidate_validation.h"

#include <cstddef>

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaAllOfCandidates(
    const JsonValue &schema_root, const JsonValue::Array &candidates,
    const JsonValue &payload, const std::string &instance_path,
    const std::string &schema_path, JsonSchemaResult &result) {
  for (std::size_t i = 0; i < candidates.size(); ++i) {
    ValidateJsonSchemaNode(schema_root, candidates[i], payload, instance_path,
                           JsonSchemaArrayElementPath(schema_path, "allOf", i),
                           result);
  }
}

}  // namespace objc3::io::json
