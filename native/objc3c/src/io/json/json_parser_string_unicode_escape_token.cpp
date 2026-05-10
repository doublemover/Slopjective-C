#include "io/json/json_parser_string_unicode_escape_token.h"

#include "io/json/json_parser_unicode_escape.h"

namespace objc3::io::json {

bool ParseJsonStringUnicodeEscapeToken(std::string_view text,
                                       std::size_t &cursor,
                                       std::optional<JsonError> &error,
                                       std::string &out) {
  return ParseJsonUnicodeEscape(text, cursor, error, out);
}

}  // namespace objc3::io::json
