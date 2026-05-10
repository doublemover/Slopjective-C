#include "io/json/json_schema_object_additional_properties_keyword_gate_validation.h"

#include "io/json/json_schema_object_additional_properties_keyword_validation.h"

namespace objc3::io::json {

const JsonValue *ValidateJsonSchemaAdditionalPropertiesKeywordGate(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *additional_properties = schema.Find("additionalProperties");
  if (additional_properties == nullptr || !payload.IsObject()) {
    return nullptr;
  }
  if (!ValidateJsonSchemaAdditionalPropertiesKeyword(*additional_properties,
                                                     schema_path, result)) {
    return nullptr;
  }
  return additional_properties;
}

}  // namespace objc3::io::json
