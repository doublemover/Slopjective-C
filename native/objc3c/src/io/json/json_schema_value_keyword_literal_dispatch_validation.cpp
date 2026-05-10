#include "io/json/json_schema_value_keyword_literal_dispatch_validation.h"

#include "io/json/json_schema_value_literal_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaValueLiteralKeywordDispatch(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaValueLiteralKeywords(schema, payload, instance_path,
                                         schema_path, result);
}

}  // namespace objc3::io::json
