#include "io/json/json_value.h"

#include <utility>

namespace objc3::io::json {
namespace {

const std::string &EmptyString() {
  static const std::string value;
  return value;
}

}  // namespace

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

JsonValue::Kind JsonValue::kind() const {
  return kind_;
}

bool JsonValue::IsNull() const {
  return kind_ == Kind::kNull;
}

bool JsonValue::IsBool() const {
  return kind_ == Kind::kBool;
}

bool JsonValue::IsNumber() const {
  return kind_ == Kind::kNumber;
}

bool JsonValue::IsString() const {
  return kind_ == Kind::kString;
}

bool JsonValue::AsBool(bool default_value) const {
  return IsBool() ? bool_value_ : default_value;
}

double JsonValue::AsNumber(double default_value) const {
  return IsNumber() ? number_value_ : default_value;
}

const std::string &JsonValue::AsString() const {
  return IsString() ? string_value_ : EmptyString();
}

}  // namespace objc3::io::json
