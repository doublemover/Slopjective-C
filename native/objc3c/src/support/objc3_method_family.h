#pragma once

#include <string>
#include <string_view>

namespace objc3c::support {

bool SelectorStartsWith(std::string_view selector, std::string_view prefix);
std::string ClassifyMethodFamilyFromSelector(std::string_view selector);

}  // namespace objc3c::support
