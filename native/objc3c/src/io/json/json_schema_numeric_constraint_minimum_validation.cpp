#include "io/json/json_schema_numeric_constraint_minimum_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaMinimumConstraint(const JsonValue &schema,
                                         const JsonValue &payload,
                                         const std::string &instance_path,
                                         const std::string &schema_path,
                                         JsonSchemaResult &result) {
  const JsonValue *minimum = schema.Find("minimum");
  if (minimum != nullptr && payload.IsNumber()) {
    if (!minimum->IsNumber()) {
      AddJsonSchemaContractError(
          result, "invalid_minimum",
          JsonSchemaKeywordPath(schema_path, "minimum"),
          "minimum must be a number");
    } else if (payload.AsNumber() < minimum->AsNumber()) {
      AddJsonSchemaPayloadError(
          result, "minimum", instance_path,
          JsonSchemaKeywordPath(schema_path, "minimum"),
          "number is below minimum");
    }
  }
}

}  // namespace objc3::io::json
