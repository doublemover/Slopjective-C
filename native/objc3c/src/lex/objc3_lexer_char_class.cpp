#include "lex/objc3_lexer_char_class.h"

#include <cctype>

bool IsObjc3IdentifierStart(char c) {
  return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_';
}

bool IsObjc3IdentifierBody(char c) {
  return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
}

bool IsObjc3HorizontalWhitespace(char c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\v' || c == '\f';
}

bool TryCountObjc3Utf8Scalars(const std::string &value, int &unit_count) {
  unit_count = 0;
  for (std::size_t index = 0; index < value.size();) {
    const unsigned char lead = static_cast<unsigned char>(value[index]);
    if (lead <= 0x7Fu) {
      ++unit_count;
      ++index;
      continue;
    }

    std::size_t width = 0u;
    unsigned codepoint = 0u;
    unsigned min_codepoint = 0u;
    if ((lead & 0xE0u) == 0xC0u) {
      width = 2u;
      codepoint = lead & 0x1Fu;
      min_codepoint = 0x80u;
    } else if ((lead & 0xF0u) == 0xE0u) {
      width = 3u;
      codepoint = lead & 0x0Fu;
      min_codepoint = 0x800u;
    } else if ((lead & 0xF8u) == 0xF0u) {
      width = 4u;
      codepoint = lead & 0x07u;
      min_codepoint = 0x10000u;
    } else {
      return false;
    }

    if (index + width > value.size()) {
      return false;
    }
    for (std::size_t offset = 1u; offset < width; ++offset) {
      const unsigned char continuation =
          static_cast<unsigned char>(value[index + offset]);
      if ((continuation & 0xC0u) != 0x80u) {
        return false;
      }
      codepoint = (codepoint << 6u) | (continuation & 0x3Fu);
    }
    if (codepoint < min_codepoint ||
        (codepoint >= 0xD800u && codepoint <= 0xDFFFu) ||
        codepoint > 0x10FFFFu) {
      return false;
    }
    ++unit_count;
    index += width;
  }
  return true;
}

std::string EscapeObjc3StringTokenText(const std::string &value) {
  std::string escaped;
  escaped.reserve(value.size() + 2u);
  escaped.push_back('"');
  for (char c : value) {
    if (c == '\n') {
      escaped.append("\\n");
      continue;
    }
    if (c == '\t') {
      escaped.append("\\t");
      continue;
    }
    if (c == '\r') {
      escaped.append("\\r");
      continue;
    }
    if (c == '"' || c == '\\') {
      escaped.push_back('\\');
    }
    escaped.push_back(c);
  }
  escaped.push_back('"');
  return escaped;
}

bool TryDecodeObjc3StringTokenText(const std::string &token_text,
                                   std::string &value) {
  value.clear();
  if (token_text.size() < 2u || token_text.front() != '"' ||
      token_text.back() != '"') {
    return false;
  }
  for (std::size_t index = 1u; index + 1u < token_text.size();) {
    const char current = token_text[index];
    if (current != '\\') {
      value.push_back(current);
      ++index;
      continue;
    }
    if (index + 2u >= token_text.size()) {
      return false;
    }
    const char escaped = token_text[index + 1u];
    switch (escaped) {
      case 'n':
        value.push_back('\n');
        break;
      case 'r':
        value.push_back('\r');
        break;
      case 't':
        value.push_back('\t');
        break;
      case '"':
        value.push_back('"');
        break;
      case '\\':
        value.push_back('\\');
        break;
      default:
        return false;
    }
    index += 2u;
  }
  return true;
}
