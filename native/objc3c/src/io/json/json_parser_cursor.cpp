#include "io/json/json_parser_cursor.h"

#include <cctype>
#include <string>
#include <utility>

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

bool JsonParserCursor::Fail(std::string message) {
  error_ = JsonError{std::move(message), cursor_};
  return false;
}

}  // namespace objc3::io::json
