#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BOOTSTRAP_API_PROBE_API_CALL_SCENARIOS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BOOTSTRAP_API_PROBE_API_CALL_SCENARIOS_H_

#include "bootstrap_api_fixture_setup.h"
#include "probe_state.h"
#include "runtime_assertion_helpers.h"
#include "runtime/public/objc3_runtime_api.h"

namespace objc3c::runtime::probe::runtime_bootstrap_api {

inline BootstrapApiScenario RunBootstrapApiScenario() {
  ResetRuntimeForBootstrapApiProbe();

  objc3_runtime_registration_state_snapshot initial_snapshot{};
  BootstrapApiScenario scenario;
  scenario.initial_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&initial_snapshot);

  const objc3_runtime_image_descriptor image = BootstrapApiImageDescriptor();
  scenario.register_status = objc3_runtime_register_image(&image);

  const objc3_runtime_selector_handle *selector_handle =
      objc3_runtime_lookup_selector(kBootstrapApiProbeSelector);
  scenario.selector_stable_id = SelectorStableId(selector_handle);

  const objc3_runtime_dispatch_i32_result dispatch =
      objc3_runtime_dispatch_i32_checked(
          kBootstrapDispatchReceiver, kBootstrapApiProbeSelector,
          kBootstrapDispatchArg0, kBootstrapDispatchArg1,
          kBootstrapDispatchArg2, kBootstrapDispatchArg3);
  scenario.dispatch_result = dispatch.value;
  scenario.expected_dispatch_result = ExpectedBootstrapDispatchResult(
      kBootstrapDispatchReceiver, kBootstrapApiProbeSelector,
      kBootstrapDispatchArg0, kBootstrapDispatchArg1, kBootstrapDispatchArg2,
      kBootstrapDispatchArg3);

  scenario.post_register_registration = CaptureRegistrationState();
  return scenario;
}

inline ResetScenario RunResetScenario() {
  ResetRuntimeForBootstrapApiProbe();

  ResetScenario scenario;
  scenario.post_reset_registration = CaptureRegistrationState();
  const objc3_runtime_selector_handle *selector_after_reset =
      objc3_runtime_lookup_selector(kBootstrapApiProbeSelector);
  scenario.selector_after_reset_stable_id =
      SelectorStableId(selector_after_reset);
  return scenario;
}

}  // namespace objc3c::runtime::probe::runtime_bootstrap_api

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BOOTSTRAP_API_PROBE_API_CALL_SCENARIOS_H_
