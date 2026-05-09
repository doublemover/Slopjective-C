#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace objc3c::support {

inline constexpr char kObjc3PropertyWeakLifetimeProfile[] = "weak";
inline constexpr char kObjc3PropertyStrongOwnedLifetimeProfile[] =
    "strong-owned";
inline constexpr char kObjc3PropertyWeakRuntimeHookProfile[] =
    "objc-weak-side-table";

enum class Objc3PropertyOwnershipProfileKind {
  kNone,
  kWeak,
  kStrongOwned,
};

inline bool PropertyAttributeProfileContains(std::string_view profile,
                                             std::string_view needle) {
  return profile.find(needle) != std::string_view::npos;
}

inline std::size_t CountRuntimeBackedPropertyOwnershipModifierSlots(
    bool is_copy,
    bool is_strong,
    bool is_retain,
    bool is_weak,
    bool is_unowned,
    bool is_assign,
    bool is_unsafe_unretained) {
  return (is_copy ? 1u : 0u) +
         ((is_strong || is_retain) ? 1u : 0u) + (is_weak ? 1u : 0u) +
         (is_unowned ? 1u : 0u) +
         ((is_assign || is_unsafe_unretained) ? 1u : 0u);
}

inline bool SupportsRuntimeManagedPropertyOwnership(
    bool is_vector,
    bool id_spelling,
    bool class_spelling,
    bool instancetype_spelling,
    bool object_pointer_type_spelling) {
  return !is_vector &&
         (id_spelling || class_spelling || instancetype_spelling ||
          object_pointer_type_spelling);
}

inline bool HasRuntimeManagedPropertyOwnershipModifier(
    bool is_copy,
    bool is_strong,
    bool is_retain,
    bool is_weak,
    bool is_unowned,
    bool is_unsafe_unretained) {
  return is_copy || is_strong || is_retain || is_weak || is_unowned ||
         is_unsafe_unretained;
}

inline bool HasExplicitRuntimeBackedPropertyStorageModifier(
    bool is_copy,
    bool is_strong,
    bool is_retain,
    bool is_weak,
    bool is_unowned,
    bool is_assign,
    bool is_unsafe_unretained) {
  return HasRuntimeManagedPropertyOwnershipModifier(
             is_copy, is_strong, is_retain, is_weak, is_unowned,
             is_unsafe_unretained) ||
         is_assign;
}

inline std::string BuildRuntimeBackedPropertyStorageModifier(
    bool is_copy,
    bool is_strong,
    bool is_retain,
    bool is_weak,
    bool is_unowned,
    bool is_assign,
    bool is_unsafe_unretained) {
  if (is_copy) {
    return "copy";
  }
  if (is_retain) {
    return "retain";
  }
  if (is_strong) {
    return "strong";
  }
  if (is_weak) {
    return "weak";
  }
  if (is_unowned) {
    return "unowned";
  }
  if (is_unsafe_unretained) {
    return "unsafe_unretained";
  }
  if (is_assign) {
    return "assign";
  }
  return {};
}

inline bool HasArcRuntimeOwnedPropertyAttribute(
    bool has_strong_attribute,
    bool has_retain_attribute,
    bool has_copy_attribute,
    bool has_weak_attribute,
    std::string_view property_attribute_profile) {
  return has_strong_attribute || has_retain_attribute || has_copy_attribute ||
         has_weak_attribute ||
         PropertyAttributeProfileContains(property_attribute_profile,
                                          "strong=1") ||
         PropertyAttributeProfileContains(property_attribute_profile,
                                          "retain=1") ||
         PropertyAttributeProfileContains(property_attribute_profile,
                                          "copy=1") ||
         PropertyAttributeProfileContains(property_attribute_profile, "weak=1");
}

inline Objc3PropertyOwnershipProfileKind ClassifyPropertyOwnershipProfile(
    bool has_weak_ownership,
    bool has_strong_ownership,
    bool has_retain_ownership,
    bool has_copy_ownership,
    std::string_view property_attribute_profile,
    bool include_retain_profile) {
  if (has_weak_ownership ||
      PropertyAttributeProfileContains(property_attribute_profile, "weak=1")) {
    return Objc3PropertyOwnershipProfileKind::kWeak;
  }
  if (has_strong_ownership || has_retain_ownership || has_copy_ownership ||
      PropertyAttributeProfileContains(property_attribute_profile,
                                       "strong=1") ||
      (include_retain_profile &&
       PropertyAttributeProfileContains(property_attribute_profile,
                                        "retain=1")) ||
      PropertyAttributeProfileContains(property_attribute_profile, "copy=1")) {
    return Objc3PropertyOwnershipProfileKind::kStrongOwned;
  }
  return Objc3PropertyOwnershipProfileKind::kNone;
}

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

inline std::string BuildPropertyAccessorOwnershipProfile(
    std::string_view effective_getter_selector,
    bool effective_setter_available,
    std::string_view effective_setter_selector,
    std::string_view ownership_lifetime_profile,
    std::string_view ownership_runtime_hook_profile) {
  std::string profile;
  profile.reserve(effective_getter_selector.size() +
                  effective_setter_selector.size() +
                  ownership_lifetime_profile.size() +
                  ownership_runtime_hook_profile.size() + 96u);
  const auto append_view = [&profile](std::string_view value) {
    if (!value.empty()) {
      profile.append(value.data(), value.size());
    }
  };
  profile += "getter=";
  append_view(effective_getter_selector);
  profile += ";setter_available=";
  profile += effective_setter_available ? "1" : "0";
  profile += ";setter=";
  if (effective_setter_available) {
    append_view(effective_setter_selector);
  } else {
    profile += "<none>";
  }
  profile += ";ownership_lifetime=";
  append_view(ownership_lifetime_profile);
  profile += ";runtime_hook=";
  append_view(ownership_runtime_hook_profile);
  return profile;
}

inline bool ShouldRebuildPropertyAccessorOwnershipProfile(
    std::string_view ownership_lifetime_profile,
    std::string_view ownership_runtime_hook_profile,
    std::string_view accessor_ownership_profile) {
  return (!ownership_lifetime_profile.empty() ||
          !ownership_runtime_hook_profile.empty()) &&
         (accessor_ownership_profile.empty() ||
          (PropertyAttributeProfileContains(accessor_ownership_profile,
                                            "ownership_lifetime=") &&
           PropertyAttributeProfileContains(accessor_ownership_profile,
                                            "ownership_lifetime=;")));
}

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

inline bool UsesWeakCurrentPropertyRuntimeHelper(
    std::string_view ownership_runtime_hook_profile) {
  return ownership_runtime_hook_profile == kObjc3PropertyWeakRuntimeHookProfile;
}

inline bool UsesStrongOwnedCurrentPropertyExchange(
    std::string_view ownership_lifetime_profile,
    std::string_view accessor_ownership_profile) {
  return ownership_lifetime_profile ==
             kObjc3PropertyStrongOwnedLifetimeProfile ||
         PropertyAttributeProfileContains(accessor_ownership_profile,
                                          "ownership_lifetime=strong-owned");
}

}  // namespace objc3c::support
