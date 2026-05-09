#pragma once

#include <string>
#include <string_view>

namespace objc3c::support {

std::string ClassifyMethodFamilyFromSelector(std::string_view selector);

}  // namespace objc3c::support
