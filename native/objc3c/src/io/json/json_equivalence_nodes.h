#pragma once

#include "io/json/json_value.h"

namespace objc3::io::json {

bool JsonArraysEqual(const JsonValue::Array &left,
                     const JsonValue::Array &right);
bool JsonObjectsEqual(const JsonValue::Object &left,
                      const JsonValue::Object &right);

}  // namespace objc3::io::json
