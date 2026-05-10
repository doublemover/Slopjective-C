#include "io/json/json_parser_cursor.h"

#include <cctype>
#include <cstdlib>
#include <string>
#include <utility>

#include "io/json/json_parser_string_token.h"

namespace objc3::io::json {

JsonParserCursor::JsonParserCursor(std::string_view text) : text_(text) {}

bool JsonParserCursor::IsDigit(char ch) {
  return std::isdigit(static_cast<unsigned char>(ch)) != 0;
}

bool JsonParserCursor::AtEnd() const {
  return cursor_ >= text_.size();
}

char JsonParserCursor::Peek() const {
  return text_[cursor_];
}

const std::optional<JsonError> &JsonParserCursor::error() const {
  return error_;
}

void JsonParserCursor::SkipWhitespace() {
  while (cursor_ < text_.size()) {
    const char ch = text_[cursor_];
    if (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n') {
      return;
    }
    ++cursor_;
  }
}

bool JsonParserCursor::Consume(char expected) {
  if (cursor_ < text_.size() && text_[cursor_] == expected) {
    ++cursor_;
    return true;
  }
  return false;
}

bool JsonParserCursor::ConsumeLiteral(std::string_view literal) {
  if (text_.substr(cursor_, literal.size()) != literal) {
    return false;
  }
  cursor_ += literal.size();
  return true;
}

bool JsonParserCursor::ParseString(std::string &out) {
  return ParseJsonStringToken(text_, cursor_, error_, out);
}

bool JsonParserCursor::ParseNumber(JsonValue &out) {
  const std::size_t start = cursor_;
  if (Consume('-') && cursor_ >= text_.size()) {
    return Fail("incomplete JSON number");
  }
  if (Consume('0')) {
    if (cursor_ < text_.size() && IsDigit(text_[cursor_])) {
      return Fail("JSON number has leading zero");
    }
  } else if (!ConsumeDigits()) {
    return Fail("expected JSON number digits");
  }
  if (Consume('.')) {
    if (!ConsumeDigits()) {
      return Fail("expected JSON number fraction digits");
    }
  }
  if (cursor_ < text_.size() &&
      (text_[cursor_] == 'e' || text_[cursor_] == 'E')) {
    ++cursor_;
    if (cursor_ < text_.size() &&
        (text_[cursor_] == '+' || text_[cursor_] == '-')) {
      ++cursor_;
    }
    if (!ConsumeDigits()) {
      return Fail("expected JSON number exponent digits");
    }
  }
  const std::string raw(text_.substr(start, cursor_ - start));
  char *end = nullptr;
  const double parsed = std::strtod(raw.c_str(), &end);
  if (end == nullptr || *end != '\0') {
    return Fail("invalid JSON number");
  }
  out = JsonValue::Number(parsed);
  return true;
}

bool JsonParserCursor::Fail(std::string message) {
  error_ = JsonError{std::move(message), cursor_};
  return false;
}

bool JsonParserCursor::ConsumeDigits() {
  const std::size_t start = cursor_;
  while (cursor_ < text_.size() && IsDigit(text_[cursor_])) {
    ++cursor_;
  }
  return cursor_ > start;
}

}  // namespace objc3::io::json
