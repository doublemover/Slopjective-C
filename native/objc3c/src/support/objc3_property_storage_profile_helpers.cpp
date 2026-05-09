#include "support/objc3_property_storage_profile_helpers.h"

namespace objc3c::support {

const char kObjc3PropertyWeakLifetimeProfile[] = "weak";
const char kObjc3PropertyStrongOwnedLifetimeProfile[] = "strong-owned";
const char kObjc3PropertyWeakRuntimeHookProfile[] = "objc-weak-side-table";

bool PropertyAttributeProfileContains(std::string_view profile,
                                      std::string_view needle) {
  return profile.find(needle) != std::string_view::npos;
}

std::size_t CountRuntimeBackedPropertyOwnershipModifierSlots(
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

bool SupportsRuntimeManagedPropertyOwnership(
    bool is_vector,
    bool id_spelling,
    bool class_spelling,
    bool instancetype_spelling,
    bool object_pointer_type_spelling) {
  return !is_vector &&
         (id_spelling || class_spelling || instancetype_spelling ||
          object_pointer_type_spelling);
}

bool HasRuntimeManagedPropertyOwnershipModifier(
    bool is_copy,
    bool is_strong,
    bool is_retain,
    bool is_weak,
    bool is_unowned,
    bool is_unsafe_unretained) {
  return is_copy || is_strong || is_retain || is_weak || is_unowned ||
         is_unsafe_unretained;
}

bool HasExplicitRuntimeBackedPropertyStorageModifier(
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

std::string BuildRuntimeBackedPropertyStorageModifier(
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

bool HasArcRuntimeOwnedPropertyAttribute(
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

Objc3PropertyOwnershipProfileKind ClassifyPropertyOwnershipProfile(
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

std::string BuildPropertyAccessorOwnershipProfile(
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

bool ShouldRebuildPropertyAccessorOwnershipProfile(
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

bool UsesWeakCurrentPropertyRuntimeHelper(
    std::string_view ownership_runtime_hook_profile) {
  return ownership_runtime_hook_profile == kObjc3PropertyWeakRuntimeHookProfile;
}

bool UsesStrongOwnedCurrentPropertyExchange(
    std::string_view ownership_lifetime_profile,
    std::string_view accessor_ownership_profile) {
  return ownership_lifetime_profile ==
             kObjc3PropertyStrongOwnedLifetimeProfile ||
         PropertyAttributeProfileContains(accessor_ownership_profile,
                                          "ownership_lifetime=strong-owned");
}

}  // namespace objc3c::support
