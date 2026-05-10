#include "io/json/json_parser_string_token.h"

#include <utility>

#include "io/json/json_parser_string_escape_token.h"

namespace objc3::io::json {
namespace {

bool FailStringToken(std::optional<JsonError> &error,
                     std::size_t cursor,
                     std::string message) {
  error = JsonError{std::move(message), cursor};
  return false;
}

}  // namespace

bool ParseJsonStringToken(std::string_view text,
                          std::size_t &cursor,
                          std::optional<JsonError> &error,
                          std::string &out) {
  if (cursor >= text.size() || text[cursor] != '"') {
    return FailStringToken(error, cursor, "expected JSON string");
  }
  ++cursor;
  out.clear();
  while (cursor < text.size()) {
    const unsigned char ch = static_cast<unsigned char>(text[cursor++]);
    if (ch == '"') {
      return true;
    }
    if (ch < 0x20u) {
      return FailStringToken(error, cursor,
                             "unescaped control character in JSON string");
    }
    if (ch != '\\') {
      out.push_back(static_cast<char>(ch));
      continue;
    }
    if (!ParseJsonStringEscapeToken(text, cursor, error, out)) {
      return false;
    }
  }
  return FailStringToken(error, cursor, "unterminated JSON string");
}

}  // namespace objc3::io::json
