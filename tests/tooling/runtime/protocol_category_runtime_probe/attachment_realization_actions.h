#pragma once

#include "probe_state.h"
#include "protocol_category_fixtures.h"
#include "runtime_assertions.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::protocol_category_runtime {

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

inline void CaptureProtocolCategoryAttachmentActions(
    ProtocolCategoryProbeRun &run) {
  run.values.instance_first = DispatchAndCaptureMethodCacheState(
      kInstanceDispatch, run.instance_first_state);
  run.values.instance_second = DispatchAndCaptureMethodCacheState(
      kInstanceDispatch, run.instance_second_state);
  run.values.class_self = DispatchAndCaptureMethodCacheState(
      kClassSelfDispatch, run.class_self_state);
  run.values.known_class = DispatchAndCaptureMethodCacheState(
      kKnownClassDispatch, run.known_class_state);
  run.values.strict_error_first = DispatchRuntimeMethodChecked(
      kStrictErrorDispatch);
  run.values.strict_error_expected = ExpectedStrictErrorDispatchValue();
  CaptureMethodCacheState(run.strict_error_first_state);
  run.values.strict_error_second = DispatchCheckedAndCaptureMethodCacheState(
      kStrictErrorDispatch, run.strict_error_second_state);

  CaptureMethodCacheEntry(kInstanceDispatch, run.instance_entry);
  CaptureMethodCacheEntry(kKnownClassDispatch, run.class_entry);
  CaptureMethodCacheEntry(kStrictErrorDispatch, run.strict_error_entry);
}

}  // namespace objc3c::runtime::probe::protocol_category_runtime
