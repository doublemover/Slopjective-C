#include "io/json/json_parser_unicode_escape.h"

#include <utility>

namespace objc3::io::json {
namespace {

bool FailUnicodeEscape(std::optional<JsonError> &error,
                       std::size_t cursor,
                       std::string message) {
  error = JsonError{std::move(message), cursor};
  return false;
}

}  // namespace

bool ParseJsonUnicodeEscape(std::string_view text,
                            std::size_t &cursor,
                            std::optional<JsonError> &error,
                            std::string &out) {
  if (cursor + 4 > text.size()) {
    return FailUnicodeEscape(error, cursor, "short JSON unicode escape");
  }
  unsigned codepoint = 0;
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
      return FailUnicodeEscape(error, cursor, "invalid JSON unicode escape");
    }
  }
  if (codepoint <= 0x7fu) {
    out.push_back(static_cast<char>(codepoint));
    return true;
  }
  if (codepoint <= 0x7ffu) {
    out.push_back(static_cast<char>(0xc0u | (codepoint >> 6u)));
    out.push_back(static_cast<char>(0x80u | (codepoint & 0x3fu)));
    return true;
  }
  out.push_back(static_cast<char>(0xe0u | (codepoint >> 12u)));
  out.push_back(static_cast<char>(0x80u | ((codepoint >> 6u) & 0x3fu)));
  out.push_back(static_cast<char>(0x80u | (codepoint & 0x3fu)));
  return true;
}

}  // namespace objc3::io::json
