#pragma once

#include <cstddef>
#include <string_view>

namespace objc3c::support::selectors {

std::size_t CountSelectorArgumentSlots(std::string_view selector);

}  // namespace objc3c::support::selectors
