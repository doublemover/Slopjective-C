#include "io/json/json_parser_unicode_escape.h"

#include "io/json/json_parser_unicode_code_unit.h"

namespace objc3::io::json {

bool ParseJsonUnicodeEscape(std::string_view text,
                            std::size_t &cursor,
                            std::optional<JsonError> &error,
                            std::string &out) {
  unsigned codepoint = 0;
  if (!ParseJsonUnicodeCodeUnit(text, cursor, error, codepoint)) {
    return false;
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
