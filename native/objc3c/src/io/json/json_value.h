#pragma once

#include <cstddef>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace objc3::io::json {

class JsonValue {
 public:
  enum class Kind {
    kNull,
    kBool,
    kNumber,
    kString,
    kArray,
    kObject,
  };

  using Array = std::vector<JsonValue>;
  using Object = std::map<std::string, JsonValue>;

  JsonValue();

  static JsonValue Null();
  static JsonValue Bool(bool value);
  static JsonValue Number(double value);
  static JsonValue String(std::string value);
  static JsonValue ArrayValue(Array value);
  static JsonValue ObjectValue(Object value);

  [[nodiscard]] Kind kind() const;
  [[nodiscard]] bool IsNull() const;
  [[nodiscard]] bool IsBool() const;
  [[nodiscard]] bool IsNumber() const;
  [[nodiscard]] bool IsString() const;
  [[nodiscard]] bool IsArray() const;
  [[nodiscard]] bool IsObject() const;

  [[nodiscard]] bool AsBool(bool default_value = false) const;
  [[nodiscard]] double AsNumber(double default_value = 0.0) const;
  [[nodiscard]] const std::string &AsString() const;
  [[nodiscard]] const Array &AsArray() const;
  [[nodiscard]] const Object &AsObject() const;

  [[nodiscard]] const JsonValue *Find(std::string_view key) const;
  [[nodiscard]] std::optional<std::string> GetString(std::string_view key) const;
  [[nodiscard]] std::optional<bool> GetBool(std::string_view key) const;

 private:
  Kind kind_ = Kind::kNull;
  bool bool_value_ = false;
  double number_value_ = 0.0;
  std::string string_value_;
  Array array_value_;
  Object object_value_;
};

}  // namespace objc3::io::json
