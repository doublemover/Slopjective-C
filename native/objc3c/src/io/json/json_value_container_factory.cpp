#include "io/json/json_value.h"

#include <utility>

namespace objc3::io::json {

JsonValue JsonValue::ArrayValue(Array value) {
  JsonValue out;
  out.kind_ = Kind::kArray;
  out.array_value_ = std::move(value);
  return out;
}

JsonValue JsonValue::ObjectValue(Object value) {
  JsonValue out;
  out.kind_ = Kind::kObject;
  out.object_value_ = std::move(value);
  return out;
}

}  // namespace objc3::io::json
