#pragma once

#include <string_view>

namespace objc3c::support {

bool MethodFamilyReturnsRetainedResult(std::string_view family_name);

}  // namespace objc3c::support
