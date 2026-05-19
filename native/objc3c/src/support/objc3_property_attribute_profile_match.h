#pragma once

#include <string_view>

namespace objc3c::support {

bool PropertyAttributeProfileContains(std::string_view profile,
                                      std::string_view needle);

}  // namespace objc3c::support
