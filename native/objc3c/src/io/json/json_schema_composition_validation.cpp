#include "io/json/json_schema_composition_validation.h"

#include "io/json/json_schema_all_of_validation.h"
#include "io/json/json_schema_any_of_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaCompositionKeywords(const JsonValue &schema_root,
                                           const JsonValue &schema,
                                           const JsonValue &payload,
                                           const std::string &instance_path,
                                           const std::string &schema_path,
                                           JsonSchemaResult &result) {
  ValidateJsonSchemaAllOf(schema_root, schema, payload, instance_path,
                          schema_path, result);
  if (!ValidateJsonSchemaAnyOf(schema_root, schema, payload, instance_path,
                               schema_path, result)) {
    return;
  }
}

}  // namespace objc3::io::json
