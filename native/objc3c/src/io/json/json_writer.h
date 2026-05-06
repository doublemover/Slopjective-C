#pragma once

#include <ostream>
#include <string>

#include "io/json/json_value.h"

namespace objc3::io::json {

void WriteJson(std::ostream &out, const JsonValue &value);
[[nodiscard]] std::string RenderJson(const JsonValue &value);

}  // namespace objc3::io::json
