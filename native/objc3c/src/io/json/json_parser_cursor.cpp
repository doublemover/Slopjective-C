#include "io/json/json_parser_cursor.h"

#include <cctype>
#include <string>
#include <utility>

#include "io/json/json_parser_number_token.h"
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
  return ParseJsonNumberToken(text_, cursor_, error_, out);
}

bool JsonParserCursor::Fail(std::string message) {
  error_ = JsonError{std::move(message), cursor_};
  return false;
}

}  // namespace objc3::io::json
