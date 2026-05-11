#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BOOTSTRAP_API_PROBE_BOOTSTRAP_API_FIXTURE_SETUP_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BOOTSTRAP_API_PROBE_BOOTSTRAP_API_FIXTURE_SETUP_H_

#include "probe_state.h"
#include "runtime_assertion_helpers.h"
#include "runtime/public/objc3_runtime_api.h"

namespace objc3c::runtime::probe::runtime_bootstrap_api {

inline constexpr const char *kBootstrapApiProbeModuleName =
    "runtimeBootstrapApiProbe";
inline constexpr const char *kBootstrapApiProbeTranslationUnitIdentityKey =
    "runtime-bootstrap-api-probe::translation-unit";
inline constexpr const char *kBootstrapApiProbeSelector = "bootstrap:ready:";

inline constexpr int kBootstrapDispatchReceiver = 5;
inline constexpr int kBootstrapDispatchArg0 = 1;
inline constexpr int kBootstrapDispatchArg1 = 2;
inline constexpr int kBootstrapDispatchArg2 = 3;
inline constexpr int kBootstrapDispatchArg3 = 4;

inline void ResetRuntimeForBootstrapApiProbe() {
  objc3_runtime_reset_for_testing();
}

inline objc3_runtime_image_descriptor BootstrapApiImageDescriptor() {
  return {
      kBootstrapApiProbeModuleName,
      kBootstrapApiProbeTranslationUnitIdentityKey,
      1,
      1,
      1,
      1,
      1,
      1,
  };
}

inline RegistrationStateObservation CaptureRegistrationState() {
  RegistrationStateObservation state;
  state.copy_status =
      objc3_runtime_copy_registration_state_for_testing(&state.snapshot);
  state.last_registered_module_name =
      CopyRuntimeString(state.snapshot.last_registered_module_name);
  state.last_registered_translation_unit_identity_key = CopyRuntimeString(
      state.snapshot.last_registered_translation_unit_identity_key);
  return state;
}

}  // namespace objc3c::runtime::probe::runtime_bootstrap_api

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BOOTSTRAP_API_PROBE_BOOTSTRAP_API_FIXTURE_SETUP_H_
