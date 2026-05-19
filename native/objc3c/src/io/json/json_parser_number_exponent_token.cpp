#include "io/json/json_parser_number_exponent_token.h"

#include <cstdlib>
#include <string>

#include "io/json/json_parser_number_exponent_digits_token.h"

namespace objc3::io::json {

bool ParseJsonNumberExponentAndConvert(std::string_view text,
                                       std::size_t token_start,
                                       std::size_t &cursor,
                                       std::optional<JsonError> &error,
                                       JsonValue &out) {
  if (cursor < text.size() && (text[cursor] == 'e' || text[cursor] == 'E')) {
    ++cursor;
    if (cursor < text.size() && (text[cursor] == '+' || text[cursor] == '-')) {
      ++cursor;
    }
    if (!ParseJsonNumberExponentDigitsToken(text, cursor, error)) {
      return false;
    }
  }
  const std::string raw(text.substr(token_start, cursor - token_start));
  char *end = nullptr;
  const double parsed = std::strtod(raw.c_str(), &end);
  if (end == nullptr || *end != '\0') {
    error = JsonError{"invalid JSON number", cursor};
    return false;
  }
  out = JsonValue::Number(parsed);
  return true;
}

}  // namespace objc3::io::json
