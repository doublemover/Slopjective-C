#include "io/json/json_value.h"

#include <utility>

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

const JsonValue *JsonValue::Find(std::string_view key) const {
  if (!IsObject()) {
    return nullptr;
  }
  const auto found = object_value_.find(std::string(key));
  return found == object_value_.end() ? nullptr : &found->second;
}

std::optional<std::string> JsonValue::GetString(std::string_view key) const {
  const JsonValue *value = Find(key);
  if (value == nullptr || !value->IsString()) {
    return std::nullopt;
  }
  return value->AsString();
}

std::optional<bool> JsonValue::GetBool(std::string_view key) const {
  const JsonValue *value = Find(key);
  if (value == nullptr || !value->IsBool()) {
    return std::nullopt;
  }
  return value->AsBool();
}

}  // namespace objc3::io::json
