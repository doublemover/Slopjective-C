#include "io/json/json_parser_cursor_whitespace.h"

namespace objc3::io::json {
namespace {

bool IsJsonWhitespace(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

}  // namespace

void SkipJsonParserWhitespace(std::string_view text, std::size_t &cursor) {
  while (cursor < text.size() && IsJsonWhitespace(text[cursor])) {
    ++cursor;
  }
}

}  // namespace objc3::io::json
