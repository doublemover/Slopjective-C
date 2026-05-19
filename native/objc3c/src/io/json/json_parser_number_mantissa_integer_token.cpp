#include "io/json/json_parser_number_mantissa_integer_token.h"

#include "io/json/json_parser_number_digit_run_token.h"
#include "io/json/json_parser_number_mantissa_integer_zero_token.h"

namespace objc3::io::json {

bool ParseJsonNumberMantissaIntegerToken(std::string_view text,
                                         std::size_t &cursor,
                                         std::optional<JsonError> &error) {
  bool matched_zero = false;
  if (!ParseJsonNumberMantissaIntegerZeroToken(text, cursor, error,
                                               matched_zero)) {
    return false;
  }
  if (matched_zero) {
    return true;
  }
  return ParseJsonNumberDigitRunToken(text, cursor, error);
}

}  // namespace objc3::io::json
