#pragma once

namespace objc3c::support {

inline bool IsAsciiAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

inline bool IsAsciiDigit(char c) {
  return c >= '0' && c <= '9';
}

inline bool IsAsciiAlphaNumeric(char c) {
  return IsAsciiAlpha(c) || IsAsciiDigit(c);
}

inline bool IsHexDigit(char c) {
  return IsAsciiDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

inline bool IsBinaryDigit(char c) {
  return c == '0' || c == '1';
}

inline bool IsOctalDigit(char c) {
  return c >= '0' && c <= '7';
}

inline bool IsDigitSeparator(char c) {
  return c == '_';
}

}  // namespace objc3c::support
