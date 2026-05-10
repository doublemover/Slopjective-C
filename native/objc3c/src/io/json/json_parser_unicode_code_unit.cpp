#include "io/json/json_parser_unicode_code_unit.h"

#include <string>
#include <utility>

namespace objc3::io::json {
namespace {

bool FailUnicodeCodeUnit(std::optional<JsonError> &error,
                         std::size_t cursor,
                         std::string message) {
  error = JsonError{std::move(message), cursor};
  return false;
}

}  // namespace

bool ParseJsonUnicodeCodeUnit(std::string_view text,
                              std::size_t &cursor,
                              std::optional<JsonError> &error,
                              unsigned &codepoint) {
  if (cursor + 4 > text.size()) {
    return FailUnicodeCodeUnit(error, cursor, "short JSON unicode escape");
  }
  codepoint = 0;
  for (int i = 0; i < 4; ++i) {
    const char ch = text[cursor++];
    codepoint <<= 4u;
    if (ch >= '0' && ch <= '9') {
      codepoint += static_cast<unsigned>(ch - '0');
    } else if (ch >= 'a' && ch <= 'f') {
      codepoint += static_cast<unsigned>(10 + ch - 'a');
    } else if (ch >= 'A' && ch <= 'F') {
      codepoint += static_cast<unsigned>(10 + ch - 'A');
    } else {
      return FailUnicodeCodeUnit(error, cursor, "invalid JSON unicode escape");
    }
  }
  return true;
}

}  // namespace objc3::io::json
