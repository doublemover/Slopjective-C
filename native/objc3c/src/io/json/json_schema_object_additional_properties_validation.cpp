#include "io/json/json_schema_object_additional_properties_validation.h"

#include "io/json/json_schema_object_additional_properties_keyword_gate_validation.h"
#include "io/json/json_schema_object_additional_properties_undeclared_traversal_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaAdditionalProperties(const JsonValue &schema_root,
                                            const JsonValue &schema,
                                            const JsonValue &payload,
                                            const JsonValue *properties,
                                            const std::string &instance_path,
                                            const std::string &schema_path,
                                            JsonSchemaResult &result) {
  const JsonValue *additional_properties =
      ValidateJsonSchemaAdditionalPropertiesKeywordGate(schema, payload,
                                                        schema_path, result);
  if (additional_properties == nullptr) {
    return;
  }
  ValidateJsonSchemaAdditionalPropertiesUndeclaredTraversal(
      schema_root, *additional_properties, payload, properties, instance_path,
      schema_path, result);
}

}  // namespace objc3::io::json
