#include "io/json/json_parser_number_exponent_digits_token.h"

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

bool FailNumberExponentDigits(std::optional<JsonError> &error,
                              std::size_t cursor,
                              std::string message) {
  error = JsonError{std::move(message), cursor};
  return false;
}

}  // namespace

bool ParseJsonNumberExponentDigitsToken(std::string_view text,
                                        std::size_t &cursor,
                                        std::optional<JsonError> &error) {
  if (!ConsumeDigits(text, cursor)) {
    return FailNumberExponentDigits(error, cursor,
                                    "expected JSON number exponent digits");
  }
  return true;
}

}  // namespace objc3::io::json
