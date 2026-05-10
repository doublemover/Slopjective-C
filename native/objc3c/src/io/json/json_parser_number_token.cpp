#include "io/json/json_parser_number_token.h"

#include <cstdlib>
#include <string>
#include <utility>

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

bool ConsumeDigits(std::string_view text, std::size_t &cursor) {
  const std::size_t start = cursor;
  while (cursor < text.size() && IsDigit(text[cursor])) {
    ++cursor;
  }
  return cursor > start;
}

bool FailNumberToken(std::optional<JsonError> &error,
                     std::size_t cursor,
                     std::string message) {
  error = JsonError{std::move(message), cursor};
  return false;
}

}  // namespace

bool ParseJsonNumberToken(std::string_view text,
                          std::size_t &cursor,
                          std::optional<JsonError> &error,
                          JsonValue &out) {
  const std::size_t start = cursor;
  if (Consume('-', text, cursor) && cursor >= text.size()) {
    return FailNumberToken(error, cursor, "incomplete JSON number");
  }
  if (Consume('0', text, cursor)) {
    if (cursor < text.size() && IsDigit(text[cursor])) {
      return FailNumberToken(error, cursor, "JSON number has leading zero");
    }
  } else if (!ConsumeDigits(text, cursor)) {
    return FailNumberToken(error, cursor, "expected JSON number digits");
  }
  if (Consume('.', text, cursor)) {
    if (!ConsumeDigits(text, cursor)) {
      return FailNumberToken(error, cursor,
                             "expected JSON number fraction digits");
    }
  }
  if (cursor < text.size() && (text[cursor] == 'e' || text[cursor] == 'E')) {
    ++cursor;
    if (cursor < text.size() && (text[cursor] == '+' || text[cursor] == '-')) {
      ++cursor;
    }
    if (!ConsumeDigits(text, cursor)) {
      return FailNumberToken(error, cursor,
                             "expected JSON number exponent digits");
    }
  }
  const std::string raw(text.substr(start, cursor - start));
  char *end = nullptr;
  const double parsed = std::strtod(raw.c_str(), &end);
  if (end == nullptr || *end != '\0') {
    return FailNumberToken(error, cursor, "invalid JSON number");
  }
  out = JsonValue::Number(parsed);
  return true;
}

}  // namespace objc3::io::json
