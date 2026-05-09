#pragma once

#include <string_view>

namespace objc3c::support::selectors {

bool IsSelectorSpellingCodeUnitAllowed(char c);
bool ContainsSelectorControlCodeUnit(std::string_view selector);

}  // namespace objc3c::support::selectors
