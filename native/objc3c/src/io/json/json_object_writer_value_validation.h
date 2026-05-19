#pragma once

#include <string_view>

#include "io/json/json_value.h"

namespace objc3::io::json {

void ValidateJsonObjectNumberMemberValue(double value);
[[nodiscard]] JsonValue ParseJsonObjectRawMemberValue(std::string_view value);

}  // namespace objc3::io::json
