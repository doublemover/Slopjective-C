#pragma once

#include "support/objc3_property_ownership_profile_application.h"

namespace objc3c::support {

template <typename PropertyProfileLike>
inline Objc3PropertyOwnershipProfileKind ApplyPropertyOwnershipProfileRetiredRoute(
    PropertyProfileLike &profile,
    bool has_weak_ownership,
    bool has_strong_ownership,
    bool has_retain_ownership,
    bool has_copy_ownership,
    bool include_retain_profile) {
  return ApplyPropertyOwnershipProfile(
      profile, has_weak_ownership, has_strong_ownership, has_retain_ownership,
      has_copy_ownership, include_retain_profile);
}

template <typename PropertyProfileLike>
inline Objc3PropertyOwnershipProfileKind ApplyPropertyOwnershipProfileRetiredRoute(
    PropertyProfileLike &profile,
    bool include_retain_profile) {
  return ApplyPropertyOwnershipProfile(profile, include_retain_profile);
}

}  // namespace objc3c::support
