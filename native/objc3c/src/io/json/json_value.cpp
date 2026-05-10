#include "io/json/json_value.h"

namespace objc3::io::json {
namespace {

const std::string &EmptyString() {
  static const std::string value;
  return value;
}

}  // namespace

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
