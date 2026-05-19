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

std::string EscapeObjc3StringTokenText(const std::string &value) {
  std::string escaped;
  escaped.reserve(value.size() + 2u);
  escaped.push_back('"');
  for (char c : value) {
    if (c == '"' || c == '\\') {
      escaped.push_back('\\');
    }
    escaped.push_back(c);
  }
  escaped.push_back('"');
  return escaped;
}
