#include "support/objc3_identifier_safe_suffix.h"

#include "support/objc3_ascii_predicates.h"

namespace objc3c::support {

bool IsIdentifierSafeSuffixChar(char c) {
  return IsAsciiAlphaNumeric(c) || c == '_';
}

std::string MakeIdentifierSafeSuffix(std::string_view text,
                                     std::string_view empty_replacement) {
  std::string suffix;
  suffix.reserve(text.size());
  for (const char c : text) {
    suffix.push_back(IsIdentifierSafeSuffixChar(c) ? c : '_');
  }
  if (suffix.empty()) {
    suffix.assign(empty_replacement);
  }
  return suffix;
}

}  // namespace objc3c::support
