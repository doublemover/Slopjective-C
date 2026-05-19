#include "io/json/json_parser_cursor.h"

namespace objc3::io::json {

char JsonParserCursor::Peek() const {
  return text_[cursor_];
}

bool JsonParserCursor::Consume(char expected) {
  if (cursor_ < text_.size() && text_[cursor_] == expected) {
    ++cursor_;
    return true;
  }
  return false;
}

}  // namespace objc3::io::json
