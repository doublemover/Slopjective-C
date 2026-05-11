#pragma once

#include <iostream>

#include "fixture_types.h"

#include "support/dispatch_expectations.h"

namespace objc3c {
namespace tooling {
namespace runtime_fast_path_contract_probe {

inline MethodCacheStateObservation CaptureMethodCacheState() {
  MethodCacheStateObservation observation{};
  observation.status =
      objc3_runtime_copy_method_cache_state_for_testing(&observation.state);
  observation.last_selector = CopyRuntimeString(observation.state.last_selector);
  return observation;
}

inline MethodCacheEntryObservation CaptureMethodCacheEntry(const int class_id,
                                                           const char *selector) {
  MethodCacheEntryObservation observation{};
  observation.status = objc3_runtime_copy_method_cache_entry_for_testing(
      class_id, selector, &observation.entry);
  return observation;
}

inline int ExpectedStrictDispatchValue() {
  return ::objc3c::runtime::probe::ExpectedStrictDispatchErrorValue(
      kProbeClassId, kStrictErrorSelector, 4, 5, 6, 7);
}

inline int InvokeStrictDispatchErrorPath() {
  const objc3_runtime_dispatch_i32_result result =
      objc3_runtime_dispatch_i32_checked(kProbeClassId, kStrictErrorSelector, 4,
                                         5, 6, 7);
  return result.value;
}

inline ProbeRun CaptureProbeRun() {
  ProbeRun run{};

  run.baseline = CaptureMethodCacheState();
  std::cout << "stage=baseline" << std::endl;

  run.implicit_value = callImplicit();
  run.explicit_value = callExplicit();
  std::cout << "stage=direct" << std::endl;
  run.direct = CaptureMethodCacheState();

  run.mixed_first_value = callMixed();
  std::cout << "stage=mixed_first_call" << std::endl;
  run.mixed_first = CaptureMethodCacheState();

  run.mixed_second_value = callMixed();
  std::cout << "stage=mixed_second_call" << std::endl;
  run.mixed_second = CaptureMethodCacheState();

  run.strict_error_expected = ExpectedStrictDispatchValue();
  run.strict_error_first_value = InvokeStrictDispatchErrorPath();
  std::cout << "stage=strict_error_first_call" << std::endl;
  run.strict_error_first = CaptureMethodCacheState();

  run.strict_error_second_value = InvokeStrictDispatchErrorPath();
  std::cout << "stage=strict_error_second_call" << std::endl;
  run.strict_error_second = CaptureMethodCacheState();

  run.dynamic_entry = CaptureMethodCacheEntry(kProbeClassId, kDynamicSelector);
  run.strict_error_entry =
      CaptureMethodCacheEntry(kProbeClassId, kStrictErrorSelector);

  return run;
}

} // namespace runtime_fast_path_contract_probe
} // namespace tooling
} // namespace objc3c
