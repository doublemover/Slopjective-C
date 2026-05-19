#include "io/json/json_parser_number_mantissa_integer_zero_token.h"

namespace objc3::io::json {
namespace {

bool IsDigit(char ch) {
  return ch >= '0' && ch <= '9';
}

}  // namespace

bool ParseJsonNumberMantissaIntegerZeroToken(std::string_view text,
                                             std::size_t &cursor,
                                             std::optional<JsonError> &error,
                                             bool &matched_zero) {
  matched_zero = cursor < text.size() && text[cursor] == '0';
  if (!matched_zero) {
    return true;
  }
  ++cursor;
  if (cursor < text.size() && IsDigit(text[cursor])) {
    error = JsonError{"JSON number has leading zero", cursor};
    return false;
  }
  return true;
}

}  // namespace objc3::io::json
