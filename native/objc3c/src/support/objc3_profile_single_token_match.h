#pragma once

#include <string_view>

namespace objc3c::support {

bool ProfileContainsToken(std::string_view profile,
                          std::string_view token);

}  // namespace objc3c::support
