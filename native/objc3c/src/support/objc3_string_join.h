#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace objc3c::support {

std::string JoinStringVector(const std::vector<std::string> &items,
                             std::string_view separator);

}  // namespace objc3c::support
