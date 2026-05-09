#include "support/objc3_identifier_safe_suffix_builder.h"

#include "support/objc3_identifier_safe_suffix_characters.h"

namespace objc3c::support {

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
