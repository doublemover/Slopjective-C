#pragma once

#include <ostream>

#include "io/json/json_value.h"

namespace objc3::io::json {

void WriteJsonValue(std::ostream &out, const JsonValue &value);

}  // namespace objc3::io::json
