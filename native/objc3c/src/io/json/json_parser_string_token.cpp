#include "io/json/json_parser_string_token.h"

#include <utility>

#include "io/json/json_parser_unicode_escape.h"

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
    if (cursor >= text.size()) {
      return FailStringToken(error, cursor, "unterminated JSON string escape");
    }
    const char escaped = text[cursor++];
    switch (escaped) {
      case '"':
      case '\\':
      case '/':
        out.push_back(escaped);
        break;
      case 'b':
        out.push_back('\b');
        break;
      case 'f':
        out.push_back('\f');
        break;
      case 'n':
        out.push_back('\n');
        break;
      case 'r':
        out.push_back('\r');
        break;
      case 't':
        out.push_back('\t');
        break;
      case 'u':
        if (!ParseJsonUnicodeEscape(text, cursor, error, out)) {
          return false;
        }
        break;
      default:
        return FailStringToken(error, cursor, "invalid JSON string escape");
    }
  }
  return FailStringToken(error, cursor, "unterminated JSON string");
}

}  // namespace objc3::io::json
