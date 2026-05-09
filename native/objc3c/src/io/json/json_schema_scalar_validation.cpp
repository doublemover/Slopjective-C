#include "io/json/json_schema_scalar_validation.h"

#include <cstddef>
#include <regex>

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaScalarFields(const JsonValue &schema,
                                    const JsonValue &payload,
                                    const std::string &path,
                                    JsonSchemaResult &result) {
  const JsonValue *minimum = schema.Find("minimum");
  if (minimum != nullptr && minimum->IsNumber() && payload.IsNumber() &&
      payload.AsNumber() < minimum->AsNumber()) {
    AddJsonSchemaError(result, path + " below minimum");
  }
  const JsonValue *maximum = schema.Find("maximum");
  if (maximum != nullptr && maximum->IsNumber() && payload.IsNumber() &&
      payload.AsNumber() > maximum->AsNumber()) {
    AddJsonSchemaError(result, path + " above maximum");
  }
  const JsonValue *min_length = schema.Find("minLength");
  if (min_length != nullptr && min_length->IsNumber() && payload.IsString() &&
      payload.AsString().size() <
          static_cast<std::size_t>(min_length->AsNumber())) {
    AddJsonSchemaError(result, path + " is shorter than minLength");
  }
  const JsonValue *max_length = schema.Find("maxLength");
  if (max_length != nullptr && max_length->IsNumber() && payload.IsString() &&
      payload.AsString().size() >
          static_cast<std::size_t>(max_length->AsNumber())) {
    AddJsonSchemaError(result, path + " is longer than maxLength");
  }
  if (const auto pattern = schema.GetString("pattern");
      pattern.has_value() && payload.IsString()) {
    try {
      if (!std::regex_search(payload.AsString(), std::regex(*pattern))) {
        AddJsonSchemaError(result, path + " did not match pattern");
      }
    } catch (const std::regex_error &) {
      AddJsonSchemaError(result, path + " contains an invalid schema pattern");
    }
  }
}

}  // namespace objc3::io::json

