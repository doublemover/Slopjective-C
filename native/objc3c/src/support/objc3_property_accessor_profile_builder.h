#pragma once

#include <string>
#include <string_view>

namespace objc3c::support {

std::string BuildPropertyAccessorOwnershipProfile(
    std::string_view effective_getter_selector,
    bool effective_setter_available,
    std::string_view effective_setter_selector,
    std::string_view ownership_lifetime_profile,
    std::string_view ownership_runtime_hook_profile);
bool ShouldRebuildPropertyAccessorOwnershipProfile(
    std::string_view ownership_lifetime_profile,
    std::string_view ownership_runtime_hook_profile,
    std::string_view accessor_ownership_profile);

template <typename PropertyProfileLike>
inline void RebuildPropertyAccessorOwnershipProfile(
    PropertyProfileLike &profile) {
  profile.accessor_ownership_profile = BuildPropertyAccessorOwnershipProfile(
      profile.effective_getter_selector, profile.effective_setter_available,
      profile.effective_setter_selector, profile.ownership_lifetime_profile,
      profile.ownership_runtime_hook_profile);
}

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
