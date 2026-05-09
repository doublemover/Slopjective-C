#include "io/json/json_schema_scalar_validation.h"

#include <cstddef>
#include <regex>

#include "io/json/json_schema_numeric_constraint_validation.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaScalarFields(const JsonValue &schema,
                                    const JsonValue &payload,
                                    const std::string &instance_path,
                                    const std::string &schema_path,
                                    JsonSchemaResult &result) {
  ValidateJsonSchemaNumericConstraints(schema, payload, instance_path,
                                       schema_path, result);
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
  const JsonValue *max_length = schema.Find("maxLength");
  if (max_length != nullptr && payload.IsString()) {
    if (!max_length->IsNumber()) {
      AddJsonSchemaContractError(
          result, "invalid_max_length",
          JsonSchemaKeywordPath(schema_path, "maxLength"),
          "maxLength must be a number");
    } else if (payload.AsString().size() >
               static_cast<std::size_t>(max_length->AsNumber())) {
      AddJsonSchemaPayloadError(
          result, "max_length", instance_path,
          JsonSchemaKeywordPath(schema_path, "maxLength"),
          "string is longer than maxLength");
    }
  }
  if (const auto pattern = schema.GetString("pattern");
      pattern.has_value() && payload.IsString()) {
    try {
      if (!std::regex_search(payload.AsString(), std::regex(*pattern))) {
        AddJsonSchemaPayloadError(
            result, "pattern", instance_path,
            JsonSchemaKeywordPath(schema_path, "pattern"),
            "string did not match pattern");
      }
    } catch (const std::regex_error &) {
      AddJsonSchemaContractError(
          result, "invalid_pattern",
          JsonSchemaKeywordPath(schema_path, "pattern"),
          "pattern is not a valid regular expression");
    }
  }
}

}  // namespace objc3::io::json
