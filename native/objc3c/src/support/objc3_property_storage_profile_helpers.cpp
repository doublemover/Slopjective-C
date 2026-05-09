#include "support/objc3_property_storage_profile_helpers.h"

namespace objc3c::support {

const char kObjc3PropertyWeakLifetimeProfile[] = "weak";
const char kObjc3PropertyStrongOwnedLifetimeProfile[] = "strong-owned";
const char kObjc3PropertyWeakRuntimeHookProfile[] = "objc-weak-side-table";

bool PropertyAttributeProfileContains(std::string_view profile,
                                      std::string_view needle) {
  return profile.find(needle) != std::string_view::npos;
}

}  // namespace objc3c::support
