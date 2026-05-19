#pragma once

#include <ostream>

#include "io/json/json_value.h"

namespace objc3::io::json {

void WriteJsonArrayValue(std::ostream &out, const JsonValue::Array &array);
void WriteJsonObjectValue(std::ostream &out, const JsonValue::Object &object);

}  // namespace objc3::io::json
