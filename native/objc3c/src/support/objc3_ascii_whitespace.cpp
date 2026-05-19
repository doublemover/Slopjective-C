#include "support/objc3_ascii_whitespace.h"

namespace objc3c::support {
namespace {

bool IsAsciiWhitespace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' ||
         c == '\v';
}

}  // namespace

std::string TrimAsciiWhitespace(std::string_view text) {
  std::size_t start = 0;
  while (start < text.size() && IsAsciiWhitespace(text[start])) {
    ++start;
  }

  std::size_t end = text.size();
  while (end > start && IsAsciiWhitespace(text[end - 1])) {
    --end;
  }
  return std::string(text.substr(start, end - start));
}

}  // namespace objc3c::support
