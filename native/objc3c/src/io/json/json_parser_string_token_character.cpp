#include "io/json/json_parser_string_token_character.h"

#include <utility>

#include "io/json/json_parser_string_escape_token.h"

namespace objc3::io::json {
namespace {

bool FailStringTokenCharacter(std::optional<JsonError> &error,
                              std::size_t cursor,
                              std::string message) {
  error = JsonError{std::move(message), cursor};
  return false;
}

}  // namespace

bool AppendJsonStringTokenCharacter(unsigned char ch,
                                    std::string_view text,
                                    std::size_t &cursor,
                                    std::optional<JsonError> &error,
                                    std::string &out) {
  if (ch < 0x20u) {
    return FailStringTokenCharacter(
        error, cursor, "unescaped control character in JSON string");
  }
  if (ch != '\\') {
    out.push_back(static_cast<char>(ch));
    return true;
  }
  return ParseJsonStringEscapeToken(text, cursor, error, out);
}

}  // namespace objc3::io::json
