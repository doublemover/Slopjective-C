#include "io/json/json_parser_string_token.h"

#include <utility>

namespace objc3::io::json {
namespace {

bool FailStringToken(std::optional<JsonError> &error,
                     std::size_t cursor,
                     std::string message) {
  error = JsonError{std::move(message), cursor};
  return false;
}

bool ParseUnicodeEscape(std::string_view text,
                        std::size_t &cursor,
                        std::optional<JsonError> &error,
                        std::string &out) {
  if (cursor + 4 > text.size()) {
    return FailStringToken(error, cursor, "short JSON unicode escape");
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
      return FailStringToken(error, cursor, "invalid JSON unicode escape");
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

}  // namespace

bool ParseJsonStringToken(std::string_view text,
                          std::size_t &cursor,
                          std::optional<JsonError> &error,
                          std::string &out) {
  if (cursor >= text.size() || text[cursor] != '"') {
    return FailStringToken(error, cursor, "expected JSON string");
  }
  ++cursor;
  out.clear();
  while (cursor < text.size()) {
    const unsigned char ch = static_cast<unsigned char>(text[cursor++]);
    if (ch == '"') {
      return true;
    }
    if (ch < 0x20u) {
      return FailStringToken(error, cursor,
                             "unescaped control character in JSON string");
    }
    if (ch != '\\') {
      out.push_back(static_cast<char>(ch));
      continue;
    }
    if (cursor >= text.size()) {
      return FailStringToken(error, cursor, "unterminated JSON string escape");
    }
    const char escaped = text[cursor++];
    switch (escaped) {
      case '"':
      case '\\':
      case '/':
        out.push_back(escaped);
        break;
      case 'b':
        out.push_back('\b');
        break;
      case 'f':
        out.push_back('\f');
        break;
      case 'n':
        out.push_back('\n');
        break;
      case 'r':
        out.push_back('\r');
        break;
      case 't':
        out.push_back('\t');
        break;
      case 'u':
        if (!ParseUnicodeEscape(text, cursor, error, out)) {
          return false;
        }
        break;
      default:
        return FailStringToken(error, cursor, "invalid JSON string escape");
    }
  }
  return FailStringToken(error, cursor, "unterminated JSON string");
}

}  // namespace objc3::io::json
