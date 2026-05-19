#include "io/json/json_schema_value_keyword_type_dispatch_validation.h"

#include "io/json/json_schema_value_type_validation.h"

namespace objc3::io::json {

bool ValidateJsonSchemaValueTypeKeywordDispatch(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  return ValidateJsonSchemaValueTypeKeyword(schema, payload, instance_path,
                                            schema_path, result);
}

}  // namespace objc3::io::json
