#include "io/json/json_parser_string_escape_token.h"

#include <utility>

#include "io/json/json_parser_unicode_escape.h"

namespace objc3::io::json {
namespace {

bool FailStringEscape(std::optional<JsonError> &error,
                      std::size_t cursor,
                      std::string message) {
  error = JsonError{std::move(message), cursor};
  return false;
}

}  // namespace

bool ParseJsonStringEscapeToken(std::string_view text,
                                std::size_t &cursor,
                                std::optional<JsonError> &error,
                                std::string &out) {
  if (cursor >= text.size()) {
    return FailStringEscape(error, cursor, "unterminated JSON string escape");
  }
  const char escaped = text[cursor++];
  switch (escaped) {
    case '"':
    case '\\':
    case '/':
      out.push_back(escaped);
      return true;
    case 'b':
      out.push_back('\b');
      return true;
    case 'f':
      out.push_back('\f');
      return true;
    case 'n':
      out.push_back('\n');
      return true;
    case 'r':
      out.push_back('\r');
      return true;
    case 't':
      out.push_back('\t');
      return true;
    case 'u':
      return ParseJsonUnicodeEscape(text, cursor, error, out);
    default:
      return FailStringEscape(error, cursor, "invalid JSON string escape");
  }
}

}  // namespace objc3::io::json
