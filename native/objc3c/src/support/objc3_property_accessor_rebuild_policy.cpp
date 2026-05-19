#include "support/objc3_property_accessor_rebuild_policy.h"

#include "support/objc3_property_profile_tokens.h"

namespace objc3c::support {

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

}  // namespace objc3c::support
