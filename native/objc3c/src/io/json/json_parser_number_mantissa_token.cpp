#include "io/json/json_parser_number_mantissa_token.h"

#include "io/json/json_parser_number_fraction_token.h"
#include "io/json/json_parser_number_mantissa_integer_token.h"
#include "io/json/json_parser_number_sign_token.h"

namespace objc3::io::json {

bool ParseJsonNumberMantissaToken(std::string_view text,
                                  std::size_t &cursor,
                                  std::optional<JsonError> &error) {
  if (!ParseJsonNumberSignToken(text, cursor, error)) {
    return false;
  }
  if (!ParseJsonNumberMantissaIntegerToken(text, cursor, error)) {
    return false;
  }
  return ParseJsonNumberFractionToken(text, cursor, error);
}

}  // namespace objc3::io::json
