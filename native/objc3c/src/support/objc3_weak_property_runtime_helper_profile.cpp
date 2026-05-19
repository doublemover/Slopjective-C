#include "support/objc3_weak_property_runtime_helper_profile.h"

#include "support/objc3_property_ownership_profile_tokens.h"

namespace objc3c::support {

bool UsesWeakCurrentPropertyRuntimeHelper(
    std::string_view ownership_runtime_hook_profile) {
  return ownership_runtime_hook_profile == kObjc3PropertyWeakRuntimeHookProfile;
}

}  // namespace objc3c::support
