#include "io/json/json_parser_number_digit_run_token.h"

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

bool FailNumberDigitRun(std::optional<JsonError> &error,
                        std::size_t cursor,
                        std::string message) {
  error = JsonError{std::move(message), cursor};
  return false;
}

}  // namespace

bool ParseJsonNumberDigitRunToken(std::string_view text,
                                  std::size_t &cursor,
                                  std::optional<JsonError> &error) {
  if (!ConsumeDigits(text, cursor)) {
    return FailNumberDigitRun(error, cursor, "expected JSON number digits");
  }
  return true;
}

}  // namespace objc3::io::json
