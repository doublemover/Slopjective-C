#include "support/selectors/selector_argument_slots.h"

namespace objc3c::support::selectors {

std::size_t CountSelectorArgumentSlots(std::string_view selector) {
  std::size_t argument_slots = 0u;
  for (char c : selector) {
    if (c == ':') {
      ++argument_slots;
    }
  }
  return argument_slots;
}

}  // namespace objc3c::support::selectors
