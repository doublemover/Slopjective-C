#include "io/json/json_schema.h"

#include "io/json/json_schema_validation.h"

namespace objc3::io::json {

JsonSchemaResult ValidateJsonSchema(const JsonValue &schema, const JsonValue &payload) {
  JsonSchemaResult result;
  ValidateJsonSchemaNode(schema, schema, payload, "$", result);
  return result;
}

}  // namespace objc3::io::json
