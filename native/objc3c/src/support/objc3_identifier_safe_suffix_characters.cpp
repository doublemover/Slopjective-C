#include "support/objc3_identifier_safe_suffix_characters.h"

#include "support/objc3_ascii_predicates.h"

namespace objc3c::support {

bool IsIdentifierSafeSuffixChar(char c) {
  return IsAsciiAlphaNumeric(c) || c == '_';
}

}  // namespace objc3c::support
