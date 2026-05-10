#include "io/json/json_value.h"

namespace objc3::io::json {
namespace {

const JsonValue::Array &EmptyArray() {
  static const JsonValue::Array value;
  return value;
}

const JsonValue::Object &EmptyObject() {
  static const JsonValue::Object value;
  return value;
}

}  // namespace

bool JsonValue::IsArray() const {
  return kind_ == Kind::kArray;
}

bool JsonValue::IsObject() const {
  return kind_ == Kind::kObject;
}

const JsonValue::Array &JsonValue::AsArray() const {
  return IsArray() ? array_value_ : EmptyArray();
}

const JsonValue::Object &JsonValue::AsObject() const {
  return IsObject() ? object_value_ : EmptyObject();
}

}  // namespace objc3::io::json
