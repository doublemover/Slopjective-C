#include "io/json/json_schema_subschema.h"

#include "io/json/json_schema_validation.h"

namespace objc3::io::json {

bool JsonSubschemaPasses(const JsonValue &schema_root,
                         const JsonValue &schema,
                         const JsonValue &payload,
                         const std::string &instance_path,
                         const std::string &schema_path) {
  JsonSchemaResult probe;
  ValidateJsonSchemaNode(schema_root, schema, payload, instance_path,
                         schema_path, probe);
  return probe.ok;
}

}  // namespace objc3::io::json
