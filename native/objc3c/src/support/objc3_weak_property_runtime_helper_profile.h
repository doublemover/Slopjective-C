#pragma once

#include <string_view>

namespace objc3c::support {

bool UsesWeakCurrentPropertyRuntimeHelper(
    std::string_view ownership_runtime_hook_profile);

}  // namespace objc3c::support
