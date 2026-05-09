#include "io/json/json_schema_numeric_constraint_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaNumericConstraints(const JsonValue &schema,
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

  const JsonValue *maximum = schema.Find("maximum");
  if (maximum != nullptr && payload.IsNumber()) {
    if (!maximum->IsNumber()) {
      AddJsonSchemaContractError(
          result, "invalid_maximum",
          JsonSchemaKeywordPath(schema_path, "maximum"),
          "maximum must be a number");
    } else if (payload.AsNumber() > maximum->AsNumber()) {
      AddJsonSchemaPayloadError(
          result, "maximum", instance_path,
          JsonSchemaKeywordPath(schema_path, "maximum"),
          "number is above maximum");
    }
  }
}

}  // namespace objc3::io::json
