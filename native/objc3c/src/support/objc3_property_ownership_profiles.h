#pragma once

#include <string_view>

#include "support/objc3_property_profile_tokens.h"

namespace objc3c::support {

enum class Objc3PropertyOwnershipProfileKind {
  kNone,
  kWeak,
  kStrongOwned,
};

bool HasArcRuntimeOwnedPropertyAttribute(
    bool has_strong_attribute,
    bool has_retain_attribute,
    bool has_copy_attribute,
    bool has_weak_attribute,
    std::string_view property_attribute_profile);
Objc3PropertyOwnershipProfileKind ClassifyPropertyOwnershipProfile(
    bool has_weak_ownership,
    bool has_strong_ownership,
    bool has_retain_ownership,
    bool has_copy_ownership,
    std::string_view property_attribute_profile,
    bool include_retain_profile);

template <typename PropertyProfileLike>
inline Objc3PropertyOwnershipProfileKind ApplyPropertyOwnershipProfile(
    PropertyProfileLike &profile,
    bool has_weak_ownership,
    bool has_strong_ownership,
    bool has_retain_ownership,
    bool has_copy_ownership,
    bool include_retain_profile) {
  if (!profile.ownership_lifetime_profile.empty()) {
    return Objc3PropertyOwnershipProfileKind::kNone;
  }
  const Objc3PropertyOwnershipProfileKind ownership_kind =
      ClassifyPropertyOwnershipProfile(
          has_weak_ownership, has_strong_ownership, has_retain_ownership,
          has_copy_ownership, profile.property_attribute_profile,
          include_retain_profile);
  if (ownership_kind == Objc3PropertyOwnershipProfileKind::kWeak) {
    profile.ownership_lifetime_profile = kObjc3PropertyWeakLifetimeProfile;
    profile.ownership_runtime_hook_profile =
        kObjc3PropertyWeakRuntimeHookProfile;
  } else if (ownership_kind ==
             Objc3PropertyOwnershipProfileKind::kStrongOwned) {
    profile.ownership_lifetime_profile =
        kObjc3PropertyStrongOwnedLifetimeProfile;
    profile.ownership_runtime_hook_profile.clear();
  }
  return ownership_kind;
}

template <typename PropertyProfileLike>
inline Objc3PropertyOwnershipProfileKind ApplyPropertyOwnershipProfile(
    PropertyProfileLike &profile,
    bool include_retain_profile) {
  return ApplyPropertyOwnershipProfile(
      profile, false, false, false, false, include_retain_profile);
}

template <typename PropertyProfileLike>
inline Objc3PropertyOwnershipProfileKind ApplyPropertyOwnershipProfileFallback(
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
inline Objc3PropertyOwnershipProfileKind ApplyPropertyOwnershipProfileFallback(
    PropertyProfileLike &profile,
    bool include_retain_profile) {
  return ApplyPropertyOwnershipProfile(profile, include_retain_profile);
}

}  // namespace objc3c::support
