#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace objc3c::support {

extern const char kObjc3PropertyWeakLifetimeProfile[];
extern const char kObjc3PropertyStrongOwnedLifetimeProfile[];
extern const char kObjc3PropertyWeakRuntimeHookProfile[];

enum class Objc3PropertyOwnershipProfileKind {
  kNone,
  kWeak,
  kStrongOwned,
};

bool PropertyAttributeProfileContains(std::string_view profile,
                                      std::string_view needle);
std::size_t CountRuntimeBackedPropertyOwnershipModifierSlots(
    bool is_copy,
    bool is_strong,
    bool is_retain,
    bool is_weak,
    bool is_unowned,
    bool is_assign,
    bool is_unsafe_unretained);
bool SupportsRuntimeManagedPropertyOwnership(
    bool is_vector,
    bool id_spelling,
    bool class_spelling,
    bool instancetype_spelling,
    bool object_pointer_type_spelling);
bool HasRuntimeManagedPropertyOwnershipModifier(
    bool is_copy,
    bool is_strong,
    bool is_retain,
    bool is_weak,
    bool is_unowned,
    bool is_unsafe_unretained);
bool HasExplicitRuntimeBackedPropertyStorageModifier(
    bool is_copy,
    bool is_strong,
    bool is_retain,
    bool is_weak,
    bool is_unowned,
    bool is_assign,
    bool is_unsafe_unretained);
std::string BuildRuntimeBackedPropertyStorageModifier(
    bool is_copy,
    bool is_strong,
    bool is_retain,
    bool is_weak,
    bool is_unowned,
    bool is_assign,
    bool is_unsafe_unretained);
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
inline Objc3PropertyOwnershipProfileKind ApplyPropertyOwnershipProfileFallback(
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
inline Objc3PropertyOwnershipProfileKind ApplyPropertyOwnershipProfileFallback(
    PropertyProfileLike &profile,
    bool include_retain_profile) {
  return ApplyPropertyOwnershipProfileFallback(
      profile, false, false, false, false, include_retain_profile);
}

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

bool UsesWeakCurrentPropertyRuntimeHelper(
    std::string_view ownership_runtime_hook_profile);
bool UsesStrongOwnedCurrentPropertyExchange(
    std::string_view ownership_lifetime_profile,
    std::string_view accessor_ownership_profile);

}  // namespace objc3c::support
