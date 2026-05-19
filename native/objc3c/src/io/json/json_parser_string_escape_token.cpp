#include "io/json/json_parser_string_escape_token.h"

#include "io/json/json_parser_string_escape_diagnostics.h"
#include "io/json/json_parser_string_unicode_escape_token.h"

namespace objc3::io::json {

bool ParseJsonStringEscapeToken(std::string_view text,
                                std::size_t &cursor,
                                std::optional<JsonError> &error,
                                std::string &out) {
  if (cursor >= text.size()) {
    return FailUnterminatedJsonStringEscape(error, cursor);
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
      return ParseJsonStringUnicodeEscapeToken(text, cursor, error, out);
    default:
      return FailInvalidJsonStringEscape(error, cursor);
  }
}

}  // namespace objc3::io::json
