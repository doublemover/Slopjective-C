#include "io/json/json_schema_any_of_candidate_probe_validation.h"

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_validation.h"

namespace objc3::io::json {

JsonSchemaResult ValidateJsonSchemaAnyOfCandidateProbe(
    const JsonValue &schema_root,
    const JsonValue &candidate,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    std::size_t candidate_index) {
  JsonSchemaResult probe;
  ValidateJsonSchemaNode(
      schema_root, candidate, payload, instance_path,
      JsonSchemaArrayElementPath(schema_path, "anyOf", candidate_index),
      probe);
  return probe;
}

}  // namespace objc3::io::json
