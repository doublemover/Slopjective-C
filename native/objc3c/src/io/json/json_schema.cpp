#include "io/json/json_schema.h"

#include "io/json/json_schema_contract_validation.h"
#include "io/json/json_schema_validation.h"

namespace objc3::io::json {

JsonSchemaResult ValidateJsonSchema(const JsonValue &schema,
                                    const JsonValue &payload) {
  JsonSchemaResult result;
  ValidateJsonSchemaNodeContract(schema, schema, "$", result);
  if (!result.ok) {
    return result;
  }
  ValidateJsonSchemaNode(schema, schema, payload, "$", "$", result);
  return result;
}

}  // namespace objc3::io::json
