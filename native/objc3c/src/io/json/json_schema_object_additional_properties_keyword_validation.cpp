#include "io/json/json_schema_object_additional_properties_keyword_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

bool ValidateJsonSchemaAdditionalPropertiesKeyword(
    const JsonValue &additional_properties, const std::string &schema_path,
    JsonSchemaResult &result) {
  if (!additional_properties.IsBool() && !additional_properties.IsObject()) {
    AddJsonSchemaContractError(
        result, "invalid_additional_properties",
        JsonSchemaKeywordPath(schema_path, "additionalProperties"),
        "additionalProperties must be false, true, or a schema object");
    return false;
  }
  return true;
}

}  // namespace objc3::io::json
