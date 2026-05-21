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

inline CacheAwareDispatchObservation CaptureCacheAwareDispatchRecord() {
  CacheAwareDispatchObservation observation{};
  observation.status =
      objc3_runtime_copy_cache_aware_dispatch_record_for_testing(
          &observation.record);
  observation.selector = CopyRuntimeString(observation.record.selector);
  observation.source_path = CopyRuntimeString(observation.record.source_path);
  observation.dispatch_path =
      CopyRuntimeString(observation.record.dispatch_path);
  observation.implementation_kind =
      CopyRuntimeString(observation.record.implementation_kind);
  observation.diagnostic_code =
      CopyRuntimeString(observation.record.diagnostic_code);
  return observation;
}

inline objc3_runtime_cache_aware_dispatch_descriptor
MakeCacheAwareDescriptor(const MethodCacheStateObservation &state,
                         const MethodCacheEntryObservation &entry) {
  objc3_runtime_cache_aware_dispatch_descriptor descriptor{};
  descriptor.abi_version = OBJC3_RUNTIME_CACHE_AWARE_DISPATCH_ABI_VERSION;
  descriptor.flags =
      OBJC3_RUNTIME_CACHE_AWARE_DISPATCH_REQUIRE_SELECTOR_STABLE_ID |
      OBJC3_RUNTIME_CACHE_AWARE_DISPATCH_REQUIRE_GENERATIONS |
      OBJC3_RUNTIME_CACHE_AWARE_DISPATCH_DEBUG_VISIBLE;
  descriptor.selector = kDynamicSelector;
  descriptor.selector_stable_id = entry.entry.selector_stable_id;
  descriptor.class_graph_generation = state.state.class_graph_generation;
  descriptor.category_attachment_generation =
      state.state.category_attachment_generation;
  descriptor.protocol_declaration_generation =
      state.state.protocol_declaration_generation;
  descriptor.storage_surface_generation = state.state.storage_surface_generation;
  descriptor.method_surface_generation = state.state.method_surface_generation;
  descriptor.source_path =
      "tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3";
  descriptor.source_line = 1;
  descriptor.source_column = 1;
  return descriptor;
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

  objc3_runtime_cache_aware_dispatch_descriptor cache_aware_descriptor =
      MakeCacheAwareDescriptor(run.mixed_second, run.dynamic_entry);
  const objc3_runtime_dispatch_i32_result cache_aware_result =
      objc3_runtime_cache_aware_dispatch_i32_checked(
          kProbeClassId, &cache_aware_descriptor, 0, 0, 0, 0);
  run.cache_aware_value = cache_aware_result.value;
  run.cache_aware_dispatch = CaptureCacheAwareDispatchRecord();

  objc3_runtime_cache_aware_dispatch_descriptor stale_descriptor =
      cache_aware_descriptor;
  stale_descriptor.class_graph_generation = 0;
  const objc3_runtime_dispatch_i32_result stale_result =
      objc3_runtime_cache_aware_dispatch_i32_checked(
          kProbeClassId, &stale_descriptor, 0, 0, 0, 0);
  run.cache_aware_stale_value = stale_result.value;
  run.cache_aware_stale_dispatch = CaptureCacheAwareDispatchRecord();

  objc3_runtime_cache_aware_dispatch_descriptor malformed_descriptor =
      cache_aware_descriptor;
  malformed_descriptor.abi_version = 0;
  const objc3_runtime_dispatch_i32_result malformed_result =
      objc3_runtime_cache_aware_dispatch_i32_checked(
          kProbeClassId, &malformed_descriptor, 0, 0, 0, 0);
  run.cache_aware_malformed_status = malformed_result.status_code;
  run.cache_aware_malformed_dispatch = CaptureCacheAwareDispatchRecord();

  return run;
}

} // namespace live_dispatch_fast_path_probe
} // namespace tooling
} // namespace objc3c
