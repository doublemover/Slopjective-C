#pragma once

#include <cstddef>
#include <string_view>

namespace objc3c::support::selectors {

bool IsSelectorSpellingCodeUnitAllowed(char c);
bool ContainsSelectorControlCodeUnit(std::string_view selector);
std::size_t CountSelectorArgumentSlots(std::string_view selector);
bool IsValidSelectorSpelling(std::string_view selector);

}  // namespace objc3c::support::selectors
