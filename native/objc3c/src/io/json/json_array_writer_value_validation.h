#pragma once

#include <string_view>

#include "io/json/json_value.h"

namespace objc3::io::json {

void ValidateJsonArrayNumberValue(double value);
[[nodiscard]] JsonValue ParseJsonArrayRawValue(std::string_view value);

}  // namespace objc3::io::json
