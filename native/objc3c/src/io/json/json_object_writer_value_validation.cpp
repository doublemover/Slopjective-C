#include "io/json/json_object_writer_value_validation.h"

#include <cmath>
#include <stdexcept>

#include "io/json/json_parser.h"

namespace objc3::io::json {

void ValidateJsonObjectNumberMemberValue(double value) {
  if (!std::isfinite(value)) {
    throw std::invalid_argument("NumberField received a non-finite number");
  }
}

JsonValue ParseJsonObjectRawMemberValue(std::string_view value) {
  JsonParseResult parsed = ParseJson(value);
  if (!parsed.ok()) {
    throw std::invalid_argument("RawJsonField received invalid JSON: " +
                                parsed.error->Format());
  }
  return parsed.value;
}

}  // namespace objc3::io::json
