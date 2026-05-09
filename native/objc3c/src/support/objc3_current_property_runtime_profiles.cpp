#include "support/objc3_current_property_runtime_profiles.h"

#include "support/objc3_property_profile_tokens.h"

namespace objc3c::support {

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
