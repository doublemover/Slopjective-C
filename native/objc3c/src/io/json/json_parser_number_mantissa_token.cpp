#include "io/json/json_parser_number_mantissa_token.h"

#include <string>
#include <utility>

#include "io/json/json_parser_number_digit_run_token.h"
#include "io/json/json_parser_number_fraction_token.h"
#include "io/json/json_parser_number_sign_token.h"

namespace objc3::io::json {
namespace {

bool IsDigit(char ch) {
  return ch >= '0' && ch <= '9';
}

bool Consume(char expected, std::string_view text, std::size_t &cursor) {
  if (cursor < text.size() && text[cursor] == expected) {
    ++cursor;
    return true;
  }
  return false;
}

bool FailNumberMantissa(std::optional<JsonError> &error,
                        std::size_t cursor,
                        std::string message) {
  error = JsonError{std::move(message), cursor};
  return false;
}

}  // namespace

bool ParseJsonNumberMantissaToken(std::string_view text,
                                  std::size_t &cursor,
                                  std::optional<JsonError> &error) {
  if (!ParseJsonNumberSignToken(text, cursor, error)) {
    return false;
  }
  if (Consume('0', text, cursor)) {
    if (cursor < text.size() && IsDigit(text[cursor])) {
      return FailNumberMantissa(error, cursor, "JSON number has leading zero");
    }
  } else if (!ParseJsonNumberDigitRunToken(text, cursor, error)) {
    return false;
  }
  return ParseJsonNumberFractionToken(text, cursor, error);
}

}  // namespace objc3::io::json
