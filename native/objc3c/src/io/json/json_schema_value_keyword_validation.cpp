#include "io/json/json_schema_value_keyword_validation.h"

#include "io/json/json_schema_value_keyword_literal_dispatch_validation.h"
#include "io/json/json_schema_value_keyword_type_dispatch_validation.h"

namespace objc3::io::json {

bool ValidateJsonSchemaValueKeywords(const JsonValue &schema,
                                     const JsonValue &payload,
                                     const std::string &instance_path,
                                     const std::string &schema_path,
                                     JsonSchemaResult &result) {
  if (!ValidateJsonSchemaValueTypeKeywordDispatch(
          schema, payload, instance_path, schema_path, result)) {
    return false;
  }

  ValidateJsonSchemaValueLiteralKeywordDispatch(
      schema, payload, instance_path, schema_path, result);

  return true;
}

}  // namespace objc3::io::json
