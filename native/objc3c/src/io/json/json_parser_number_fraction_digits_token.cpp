#include "io/json/json_parser_number_fraction_digits_token.h"

namespace objc3::io::json {
namespace {

bool IsDigit(char ch) {
  return ch >= '0' && ch <= '9';
}

}  // namespace

bool ConsumeJsonNumberFractionDigits(std::string_view text,
                                     std::size_t &cursor) {
  const std::size_t start = cursor;
  while (cursor < text.size() && IsDigit(text[cursor])) {
    ++cursor;
  }
  return cursor > start;
}

}  // namespace objc3::io::json
