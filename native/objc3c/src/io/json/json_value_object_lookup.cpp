#include "io/json/json_value.h"

namespace objc3::io::json {

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
