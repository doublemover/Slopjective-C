#pragma once

#include <string>
#include <string_view>

#include "support/objc3_ascii_predicates.h"

namespace objc3c::support {

inline bool IsIdentifierSafeSuffixChar(char c) {
  return IsAsciiAlphaNumeric(c) || c == '_';
}

inline std::string MakeIdentifierSafeSuffix(std::string_view text, std::string_view empty_fallback) {
  std::string suffix;
  suffix.reserve(text.size());
  for (const char c : text) {
    suffix.push_back(IsIdentifierSafeSuffixChar(c) ? c : '_');
  }
  if (suffix.empty()) {
    suffix.assign(empty_fallback);
  }
  return suffix;
}

}  // namespace objc3c::support
