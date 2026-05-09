#include "support/objc3_property_accessor_profiles.h"

#include "support/objc3_property_profile_tokens.h"

namespace objc3c::support {

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
