#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include "io/json/json_value.h"

namespace objc3::io::json {

class JsonObjectWriter {
 public:
  explicit JsonObjectWriter(std::ostream &out);
  JsonObjectWriter(const JsonObjectWriter &) = delete;
  JsonObjectWriter &operator=(const JsonObjectWriter &) = delete;

  void StringField(std::string_view name, std::string_view value);
  void BoolField(std::string_view name, bool value);
  void IntField(std::string_view name, std::int64_t value);
  void NumberField(std::string_view name, double value);
  void SizeField(std::string_view name, std::size_t value);
  void UnsignedField(std::string_view name, std::uint64_t value);
  void StringArrayField(std::string_view name,
                        const std::vector<std::string> &values);
  void ValueField(std::string_view name, const JsonValue &value);
  void RawJsonField(std::string_view name, std::string_view value);
  void End();

 private:
  void BeginField(std::string_view name);

  std::ostream &out_;
  bool first_ = true;
  bool ended_ = false;
};

class JsonArrayWriter {
 public:
  explicit JsonArrayWriter(std::ostream &out);
  JsonArrayWriter(const JsonArrayWriter &) = delete;
  JsonArrayWriter &operator=(const JsonArrayWriter &) = delete;

  void BeginElement();
  void StringValue(std::string_view value);
  void BoolValue(bool value);
  void IntValue(std::int64_t value);
  void NumberValue(double value);
  void SizeValue(std::size_t value);
  void UnsignedValue(std::uint64_t value);
  void Value(const JsonValue &value);
  void RawJsonValue(std::string_view value);
  void End();

 private:
  std::ostream &out_;
  bool first_ = true;
  bool ended_ = false;
};

void WriteJson(std::ostream &out, const JsonValue &value);
[[nodiscard]] std::string RenderJson(const JsonValue &value);
void WriteJsonStringArray(std::ostream &out,
                          const std::vector<std::string> &values);
[[nodiscard]] std::string RenderJsonStringArray(
    const std::vector<std::string> &values);

}  // namespace objc3::io::json
