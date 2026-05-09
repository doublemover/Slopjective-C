#include "support/objc3_property_storage_profile_helpers.h"

#include "support/objc3_profile_token_match.h"

namespace objc3c::support {

const char kObjc3PropertyWeakLifetimeProfile[] = "weak";
const char kObjc3PropertyStrongOwnedLifetimeProfile[] = "strong-owned";
const char kObjc3PropertyWeakRuntimeHookProfile[] = "objc-weak-side-table";

bool PropertyAttributeProfileContains(std::string_view profile,
                                      std::string_view needle) {
  return ProfileContainsToken(profile, needle);
}

}  // namespace objc3c::support
