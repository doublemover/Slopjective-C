#include "io/json/json_schema_composition_all_of_dispatch_validation.h"

#include "io/json/json_schema_all_of_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaAllOfCompositionDispatch(
    const JsonValue &schema_root,
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaAllOf(schema_root, schema, payload, instance_path,
                          schema_path, result);
}

}  // namespace objc3::io::json
