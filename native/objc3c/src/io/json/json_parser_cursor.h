#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "io/json/json_error.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

class JsonParserCursor {
 public:
  explicit JsonParserCursor(std::string_view text);

  [[nodiscard]] static bool IsDigit(char ch);

  [[nodiscard]] bool AtEnd() const;
  [[nodiscard]] char Peek() const;
  [[nodiscard]] const std::optional<JsonError> &error() const;

  void SkipWhitespace();
  bool Consume(char expected);
  bool ConsumeLiteral(std::string_view literal);
  bool ParseString(std::string &out);
  bool ParseNumber(JsonValue &out);
  bool Fail(std::string message);

 private:
  std::string_view text_;
  std::size_t cursor_ = 0;
  std::optional<JsonError> error_;
};

}  // namespace objc3::io::json
