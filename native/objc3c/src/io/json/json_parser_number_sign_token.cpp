#include "io/json/json_parser_number_sign_token.h"

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

bool FailNumberSign(std::optional<JsonError> &error,
                    std::size_t cursor,
                    std::string message) {
  error = JsonError{std::move(message), cursor};
  return false;
}

}  // namespace

bool ParseJsonNumberSignToken(std::string_view text,
                              std::size_t &cursor,
                              std::optional<JsonError> &error) {
  if (Consume('-', text, cursor) && cursor >= text.size()) {
    return FailNumberSign(error, cursor, "incomplete JSON number");
  }
  return true;
}

}  // namespace objc3::io::json
