#include "io/json/json_schema_validation.h"

#include "io/json/json_schema_runtime_node_phase_sequence_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaNode(const JsonValue &schema_root,
                            const JsonValue &schema,
                            const JsonValue &payload,
                            std::string instance_path,
                            std::string schema_path,
                            JsonSchemaResult &result) {
  ValidateJsonSchemaRuntimeNodePhaseSequence(
      schema_root, schema, payload, instance_path, schema_path, result);
}

}  // namespace objc3::io::json
