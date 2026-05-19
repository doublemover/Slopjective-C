#include "runtime/storage/property_accessor_profiles.h"

#include "runtime/metadata/runtime_realized_records.h"

#include <cstring>

namespace objc3c::runtime {

namespace {

const char *PropertyOwnershipLifetimeProfile(
    const RealizedPropertyAccessor &accessor) {
  return accessor.property_descriptor != nullptr
             ? accessor.property_descriptor->ownership_lifetime_profile
             : nullptr;
}

const char *PropertyOwnershipRuntimeHookProfile(
    const RealizedPropertyAccessor &accessor) {
  return accessor.property_descriptor != nullptr
             ? accessor.property_descriptor->ownership_runtime_hook_profile
             : nullptr;
}

const char *PropertyAccessorOwnershipProfile(
    const RealizedPropertyAccessor &accessor) {
  return accessor.property_descriptor != nullptr
             ? accessor.property_descriptor->accessor_ownership_profile
             : nullptr;
}

bool AccessorProfileContains(const char *profile, const char *needle) {
  return profile != nullptr && needle != nullptr &&
         std::strstr(profile, needle) != nullptr;
}

}  // namespace

bool UsesStrongOwnedRuntimeHooks(const RealizedPropertyAccessor &accessor) {
  const char *lifetime_profile = PropertyOwnershipLifetimeProfile(accessor);
  if (lifetime_profile != nullptr &&
      std::strcmp(lifetime_profile, "strong-owned") == 0) {
    return true;
  }
  return AccessorProfileContains(PropertyAccessorOwnershipProfile(accessor),
                                 "ownership_lifetime=strong-owned");
}

bool UsesWeakRuntimeHooks(const RealizedPropertyAccessor &accessor) {
  const char *hook_profile = PropertyOwnershipRuntimeHookProfile(accessor);
  return hook_profile != nullptr &&
         std::strcmp(hook_profile, "objc-weak-side-table") == 0;
}

bool UsesSafeUnownedRuntimeHooks(const RealizedPropertyAccessor &accessor) {
  const char *hook_profile = PropertyOwnershipRuntimeHookProfile(accessor);
  return hook_profile != nullptr &&
         std::strcmp(hook_profile, "objc-unowned-safe-guard") == 0;
}

}  // namespace objc3c::runtime
