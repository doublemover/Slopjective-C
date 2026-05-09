#pragma once

#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>

#include "io/json/json_value.h"

namespace objc3::io::json {

class JsonObjectWriter {
 public:
  explicit JsonObjectWriter(std::ostream &out);
  JsonObjectWriter(const JsonObjectWriter &) = delete;
  JsonObjectWriter &operator=(const JsonObjectWriter &) = delete;

  void StringField(std::string_view name, std::string_view value);
  void BoolField(std::string_view name, bool value);
  void NumberField(std::string_view name, double value);
  void UnsignedField(std::string_view name, std::uint64_t value);
  void RawJsonField(std::string_view name, std::string_view value);
  void End();

 private:
  void BeginField(std::string_view name);

  std::ostream &out_;
  bool first_ = true;
  bool ended_ = false;
};

void WriteJson(std::ostream &out, const JsonValue &value);
[[nodiscard]] std::string RenderJson(const JsonValue &value);

}  // namespace objc3::io::json
