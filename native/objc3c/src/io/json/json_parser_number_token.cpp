#include "io/json/json_parser_number_token.h"

#include "io/json/json_parser_number_exponent_token.h"
#include "io/json/json_parser_number_mantissa_token.h"

namespace objc3::io::json {

bool ParseJsonNumberToken(std::string_view text,
                          std::size_t &cursor,
                          std::optional<JsonError> &error,
                          JsonValue &out) {
  const std::size_t start = cursor;
  if (!ParseJsonNumberMantissaToken(text, cursor, error)) {
    return false;
  }
  return ParseJsonNumberExponentAndConvert(text, start, cursor, error, out);
}

}  // namespace objc3::io::json
