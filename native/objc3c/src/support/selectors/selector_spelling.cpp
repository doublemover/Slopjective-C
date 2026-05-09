#include "support/selectors/selector_spelling.h"

namespace objc3c::support::selectors {

bool IsSelectorSpellingCodeUnitAllowed(char c) {
  const unsigned char code_unit = static_cast<unsigned char>(c);
  return code_unit >= 0x20u && code_unit != 0x7fu;
}

bool ContainsSelectorControlCodeUnit(std::string_view selector) {
  for (char c : selector) {
    if (!IsSelectorSpellingCodeUnitAllowed(c)) {
      return true;
    }
  }
  return false;
}

std::size_t CountSelectorArgumentSlots(std::string_view selector) {
  std::size_t argument_slots = 0u;
  for (char c : selector) {
    if (c == ':') {
      ++argument_slots;
    }
  }
  return argument_slots;
}

bool IsValidSelectorSpelling(std::string_view selector) {
  return !selector.empty() && !ContainsSelectorControlCodeUnit(selector);
}

}  // namespace objc3c::support::selectors
