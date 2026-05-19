#include "support/objc3_identifier_spelling.h"

#include "support/objc3_ascii_predicates.h"

namespace objc3c::support {

bool IsObjcIdentifierSpelling(std::string_view identifier) {
  if (identifier.empty()) {
    return false;
  }
  if (!(IsAsciiAlpha(identifier.front()) || identifier.front() == '_')) {
    return false;
  }
  for (std::size_t i = 1; i < identifier.size(); ++i) {
    const char c = identifier[i];
    if (!(IsAsciiAlphaNumeric(c) || c == '_')) {
      return false;
    }
  }
  return true;
}

}  // namespace objc3c::support
