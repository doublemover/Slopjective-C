#pragma once

#include <string_view>

namespace objc3c::support {

bool UsesStrongOwnedCurrentPropertyExchange(
    std::string_view ownership_lifetime_profile,
    std::string_view accessor_ownership_profile);

}  // namespace objc3c::support
