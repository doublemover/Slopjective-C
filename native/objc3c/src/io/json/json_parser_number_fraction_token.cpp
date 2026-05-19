#include "io/json/json_parser_number_fraction_token.h"

#include "io/json/json_parser_number_fraction_digits_token.h"

#include <string>
#include <utility>

namespace objc3::io::json {
namespace {

bool Consume(char expected, std::string_view text, std::size_t &cursor) {
  if (cursor < text.size() && text[cursor] == expected) {
    ++cursor;
    return true;
  }
  return false;
}

bool FailNumberFraction(std::optional<JsonError> &error,
                        std::size_t cursor,
                        std::string message) {
  error = JsonError{std::move(message), cursor};
  return false;
}

}  // namespace

bool ParseJsonNumberFractionToken(std::string_view text,
                                  std::size_t &cursor,
                                  std::optional<JsonError> &error) {
  if (!Consume('.', text, cursor)) {
    return true;
  }
  if (!ConsumeJsonNumberFractionDigits(text, cursor)) {
    return FailNumberFraction(error, cursor,
                              "expected JSON number fraction digits");
  }
  return true;
}

}  // namespace objc3::io::json
