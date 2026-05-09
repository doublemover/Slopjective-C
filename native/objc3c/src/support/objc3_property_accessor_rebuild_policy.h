#pragma once

#include <string_view>

#include "support/objc3_property_accessor_profile_builder.h"

namespace objc3c::support {

bool ShouldRebuildPropertyAccessorOwnershipProfile(
    std::string_view ownership_lifetime_profile,
    std::string_view ownership_runtime_hook_profile,
    std::string_view accessor_ownership_profile);

template <typename PropertyProfileLike>
inline bool RebuildPropertyAccessorOwnershipProfileIfNeeded(
    PropertyProfileLike &profile) {
  if (!ShouldRebuildPropertyAccessorOwnershipProfile(
          profile.ownership_lifetime_profile,
          profile.ownership_runtime_hook_profile,
          profile.accessor_ownership_profile)) {
    return false;
  }
  RebuildPropertyAccessorOwnershipProfile(profile);
  return true;
}

}  // namespace objc3c::support
