#include "support/selectors/selector_spelling_validation.h"

#include "support/selectors/selector_code_units.h"

namespace objc3c::support::selectors {

bool IsValidSelectorSpelling(std::string_view selector) {
  return !selector.empty() && !ContainsSelectorControlCodeUnit(selector);
}

}  // namespace objc3c::support::selectors
