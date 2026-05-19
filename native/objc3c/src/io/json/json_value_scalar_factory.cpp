#include "io/json/json_value.h"

#include <utility>

namespace objc3::io::json {

JsonValue::JsonValue() = default;

JsonValue JsonValue::Null() {
  return JsonValue();
}

JsonValue JsonValue::Bool(bool value) {
  JsonValue out;
  out.kind_ = Kind::kBool;
  out.bool_value_ = value;
  return out;
}

JsonValue JsonValue::Number(double value) {
  JsonValue out;
  out.kind_ = Kind::kNumber;
  out.number_value_ = value;
  return out;
}

JsonValue JsonValue::String(std::string value) {
  JsonValue out;
  out.kind_ = Kind::kString;
  out.string_value_ = std::move(value);
  return out;
}

}  // namespace objc3::io::json
