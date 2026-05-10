#include "io/json/json_parser_number_exponent_token.h"

#include <cstdlib>
#include <string>
#include <utility>

namespace objc3::io::json {
namespace {

bool IsDigit(char ch) {
  return ch >= '0' && ch <= '9';
}

bool ConsumeDigits(std::string_view text, std::size_t &cursor) {
  const std::size_t start = cursor;
  while (cursor < text.size() && IsDigit(text[cursor])) {
    ++cursor;
  }
  return cursor > start;
}

bool FailNumberExponent(std::optional<JsonError> &error,
                        std::size_t cursor,
                        std::string message) {
  error = JsonError{std::move(message), cursor};
  return false;
}

}  // namespace

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
    if (!ConsumeDigits(text, cursor)) {
      return FailNumberExponent(error, cursor,
                                "expected JSON number exponent digits");
    }
  }
  const std::string raw(text.substr(token_start, cursor - token_start));
  char *end = nullptr;
  const double parsed = std::strtod(raw.c_str(), &end);
  if (end == nullptr || *end != '\0') {
    return FailNumberExponent(error, cursor, "invalid JSON number");
  }
  out = JsonValue::Number(parsed);
  return true;
}

}  // namespace objc3::io::json
