#include "io/json/json_array_writer_value_validation.h"

#include <cmath>
#include <stdexcept>

#include "io/json/json_parser.h"

namespace objc3::io::json {

void ValidateJsonArrayNumberValue(double value) {
  if (!std::isfinite(value)) {
    throw std::invalid_argument("NumberValue received a non-finite number");
  }
}

JsonValue ParseJsonArrayRawValue(std::string_view value) {
  JsonParseResult parsed = ParseJson(value);
  if (!parsed.ok()) {
    throw std::invalid_argument("RawJsonValue received invalid JSON: " +
                                parsed.error->Format());
  }
  return parsed.value;
}

}  // namespace objc3::io::json
