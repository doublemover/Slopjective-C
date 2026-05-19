#include "support/selectors/selector_code_units.h"

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

}  // namespace objc3c::support::selectors
