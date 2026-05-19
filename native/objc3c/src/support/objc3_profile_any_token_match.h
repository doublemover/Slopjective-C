#pragma once

#include <initializer_list>
#include <string_view>

namespace objc3c::support {

bool ProfileContainsAnyToken(
    std::string_view profile,
    std::initializer_list<std::string_view> tokens);

}  // namespace objc3c::support
