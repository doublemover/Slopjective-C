#pragma once

#include "class_metadata_definitions.h"
#include "invariant_assertion_helpers.h"
#include "probe_state.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::class_realization_runtime {

inline int DispatchRuntimeMethod(const RuntimeDispatch &dispatch) {
  return objc3_runtime_dispatch_i32(dispatch.receiver, dispatch.selector, 0, 0,
                                    0, 0);
}

inline int DispatchRuntimeMethodChecked(const RuntimeDispatch &dispatch) {
  const objc3_runtime_dispatch_i32_result result =
      objc3_runtime_dispatch_i32_checked(dispatch.receiver, dispatch.selector,
                                         0, 0, 0, 0);
  return result.value;
}

inline void StabilizeMethodCacheStateObservation(
    MethodCacheStateObservation &observation) {
  ::objc3c::runtime::probe::StabilizeMethodCacheState(
      observation.state, observation.selector, observation.class_name,
      observation.owner);
}

inline void StabilizeMethodCacheEntryObservation(
    MethodCacheEntryObservation &observation) {
  ::objc3c::runtime::probe::StabilizeMethodCacheEntry(
      observation.entry, observation.selector, observation.class_name,
      observation.owner);
}

inline void CaptureMethodCacheState(MethodCacheStateObservation &observation) {
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &observation.state);
  StabilizeMethodCacheStateObservation(observation);
}

inline int DispatchAndCaptureMethodCacheState(
    const RuntimeDispatch &dispatch, MethodCacheStateObservation &observation) {
  const int value = DispatchRuntimeMethod(dispatch);
  CaptureMethodCacheState(observation);
  return value;
}

inline int DispatchCheckedAndCaptureMethodCacheState(
    const RuntimeDispatch &dispatch, MethodCacheStateObservation &observation) {
  const int value = DispatchRuntimeMethodChecked(dispatch);
  CaptureMethodCacheState(observation);
  return value;
}

inline void CaptureMethodCacheEntry(
    const RuntimeDispatch &dispatch, MethodCacheEntryObservation &observation) {
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      dispatch.receiver, dispatch.selector, &observation.entry);
  StabilizeMethodCacheEntryObservation(observation);
}

inline void CaptureClassRealizationActions(ClassRealizationProbeRun &run) {
  run.values.inherited_value = DispatchAndCaptureMethodCacheState(
      kInheritedMethodDispatch, run.inherited_state);
  run.values.category_value = DispatchAndCaptureMethodCacheState(
      kCategoryMethodDispatch, run.category_state);
  run.values.class_value = DispatchAndCaptureMethodCacheState(
      kClassMethodDispatch, run.class_state);
  run.values.known_class_value = DispatchAndCaptureMethodCacheState(
      kKnownClassMethodDispatch, run.known_class_state);
  run.values.protocol_strict_error =
      DispatchCheckedAndCaptureMethodCacheState(
          kProtocolStrictErrorDispatch, run.protocol_strict_error_state);
  run.values.protocol_strict_error_cached =
      DispatchCheckedAndCaptureMethodCacheState(
          kProtocolStrictErrorDispatch,
          run.protocol_strict_error_cached_state);
  run.values.protocol_strict_error_expected =
      ExpectedProtocolStrictDispatchErrorValue();

  CaptureMethodCacheEntry(kInheritedMethodDispatch, run.inherited_entry);
  CaptureMethodCacheEntry(kCategoryMethodDispatch, run.category_entry);
  CaptureMethodCacheEntry(kKnownClassMethodDispatch, run.known_class_entry);
  CaptureMethodCacheEntry(kProtocolStrictErrorDispatch,
                          run.protocol_strict_error_entry);
}

}  // namespace objc3c::runtime::probe::class_realization_runtime
