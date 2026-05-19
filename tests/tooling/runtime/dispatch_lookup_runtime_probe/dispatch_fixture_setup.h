#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_DISPATCH_FIXTURE_SETUP_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_DISPATCH_FIXTURE_SETUP_H_

#include "runtime/public/objc3_runtime_api.h"

namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe {

inline constexpr const char *kDispatchLookupProbeModuleName =
    "dispatch_runtime-probe";
inline constexpr const char *kDispatchLookupProbeTranslationUnit =
    "dispatch_runtime::translation-unit";

inline objc3_runtime_image_descriptor DispatchLookupImageDescriptor() {
  return objc3_runtime_image_descriptor{
      kDispatchLookupProbeModuleName,
      kDispatchLookupProbeTranslationUnit,
      1,
      1,
      0,
      0,
      0,
      0,
  };
}

inline void ResetDispatchLookupRuntime() {
  objc3_runtime_reset_for_testing();
}

inline int RegisterDispatchLookupImage() {
  const objc3_runtime_image_descriptor image = DispatchLookupImageDescriptor();
  return objc3_runtime_register_image(&image);
}

}  // namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_DISPATCH_FIXTURE_SETUP_H_
