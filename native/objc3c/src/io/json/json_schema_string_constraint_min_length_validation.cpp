#include "io/json/json_schema_string_constraint_min_length_validation.h"

#include <cstddef>

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaStringMinLengthConstraint(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *min_length = schema.Find("minLength");
  if (min_length != nullptr && payload.IsString()) {
    if (!min_length->IsNumber()) {
      AddJsonSchemaContractError(
          result, "invalid_min_length",
          JsonSchemaKeywordPath(schema_path, "minLength"),
          "minLength must be a number");
    } else if (payload.AsString().size() <
               static_cast<std::size_t>(min_length->AsNumber())) {
      AddJsonSchemaPayloadError(
          result, "min_length", instance_path,
          JsonSchemaKeywordPath(schema_path, "minLength"),
          "string is shorter than minLength");
    }
  }
}

}  // namespace objc3::io::json
