#pragma once

#include "fixture_types.h"

#include "support/dispatch_expectations.h"

namespace objc3c {
namespace tooling {
namespace live_dispatch_fast_path_probe {

inline MethodCacheStateObservation CaptureMethodCacheState() {
  MethodCacheStateObservation observation{};
  observation.status =
      objc3_runtime_copy_method_cache_state_for_testing(&observation.state);
  observation.last_selector = CopyRuntimeString(observation.state.last_selector);
  observation.last_fast_path_reason =
      CopyRuntimeString(observation.state.last_fast_path_reason);
  return observation;
}

inline DispatchStateObservation CaptureDispatchState() {
  DispatchStateObservation observation{};
  observation.status =
      objc3_runtime_copy_dispatch_state_for_testing(&observation.state);
  observation.last_selector = CopyRuntimeString(observation.state.last_selector);
  observation.last_fast_path_reason =
      CopyRuntimeString(observation.state.last_fast_path_reason);
  observation.last_path =
      CopyRuntimeString(observation.state.last_dispatch_path);
  observation.last_implementation_kind =
      CopyRuntimeString(observation.state.last_implementation_kind);
  observation.last_resolved_class_name =
      CopyRuntimeString(observation.state.last_resolved_class_name);
  return observation;
}

inline MethodCacheEntryObservation CaptureMethodCacheEntry(
    const int class_id,
    const char *selector) {
  MethodCacheEntryObservation observation{};
  observation.status = objc3_runtime_copy_method_cache_entry_for_testing(
      class_id, selector, &observation.entry);
  observation.selector = CopyRuntimeString(observation.entry.selector);
  observation.fast_path_reason =
      CopyRuntimeString(observation.entry.fast_path_reason);
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
  run.dynamic_entry = CaptureMethodCacheEntry(kProbeClassId, kDynamicSelector);
  run.explicit_entry = CaptureMethodCacheEntry(kProbeClassId, kExplicitSelector);

  run.implicit_value = callImplicit();
  run.explicit_value = callExplicit();
  run.direct = CaptureMethodCacheState();

  run.mixed_first_value = callMixed();
  run.mixed_first = CaptureMethodCacheState();
  run.mixed_first_dispatch = CaptureDispatchState();

  run.mixed_second_value = callMixed();
  run.mixed_second = CaptureMethodCacheState();
  run.mixed_second_dispatch = CaptureDispatchState();

  run.strict_error_expected = ExpectedStrictDispatchValue();
  run.strict_error_first_value = InvokeStrictDispatchErrorPath();
  run.strict_error_first = CaptureMethodCacheState();
  run.strict_error_first_dispatch = CaptureDispatchState();

  run.strict_error_second_value = InvokeStrictDispatchErrorPath();
  run.strict_error_second = CaptureMethodCacheState();
  run.strict_error_second_dispatch = CaptureDispatchState();

  run.strict_error_entry =
      CaptureMethodCacheEntry(kProbeClassId, kStrictErrorSelector);

  return run;
}

} // namespace live_dispatch_fast_path_probe
} // namespace tooling
} // namespace objc3c
