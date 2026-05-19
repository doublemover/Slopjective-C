#include "io/json/json_parser_string_escape_diagnostics.h"

#include <string>
#include <utility>

namespace objc3::io::json {
namespace {

bool FailStringEscape(std::optional<JsonError> &error,
                      std::size_t cursor,
                      std::string message) {
  error = JsonError{std::move(message), cursor};
  return false;
}

}  // namespace

bool FailUnterminatedJsonStringEscape(std::optional<JsonError> &error,
                                      std::size_t cursor) {
  return FailStringEscape(error, cursor, "unterminated JSON string escape");
}

bool FailInvalidJsonStringEscape(std::optional<JsonError> &error,
                                 std::size_t cursor) {
  return FailStringEscape(error, cursor, "invalid JSON string escape");
}

}  // namespace objc3::io::json
