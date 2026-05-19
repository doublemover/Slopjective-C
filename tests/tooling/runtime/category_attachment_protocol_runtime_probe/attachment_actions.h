#pragma once

#include "category_protocol_fixture_definitions.h"
#include "probe_state.h"
#include "support/runtime_snapshot_stabilizers.h"

#include <cstdint>

namespace objc3c::runtime::probe::category_attachment_protocol_runtime {

inline int ReceiverIdentityAsInt(std::uint64_t receiver_identity) {
  return static_cast<int>(receiver_identity);
}

inline RuntimeDispatch BuildWidgetInstanceDispatch(
    const CategoryAttachmentProtocolProbeRun &run, const char *selector) {
  return RuntimeDispatch{
      ReceiverIdentityAsInt(run.widget_entry.entry.instance_receiver_identity),
      selector};
}

inline RuntimeDispatch BuildWidgetClassDispatch(
    const CategoryAttachmentProtocolProbeRun &run, const char *selector) {
  return RuntimeDispatch{
      ReceiverIdentityAsInt(run.widget_entry.entry.class_receiver_identity),
      selector};
}

inline int DispatchRuntimeValue(const RuntimeDispatch &dispatch) {
  return objc3_runtime_dispatch_i32(dispatch.receiver, dispatch.selector, 0, 0,
                                    0, 0);
}

inline int DispatchRuntimeValueChecked(const RuntimeDispatch &dispatch) {
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

inline void CaptureMethodCacheState(MethodCacheStateObservation &observation) {
  observation = MethodCacheStateObservation{};
  (void)objc3_runtime_copy_method_cache_state_for_testing(&observation.state);
  StabilizeMethodCacheStateObservation(observation);
}

inline void StabilizeMethodCacheEntryObservation(
    MethodCacheEntryObservation &observation) {
  ::objc3c::runtime::probe::StabilizeMethodCacheEntry(
      observation.entry, observation.selector, observation.class_name,
      observation.owner);
}

inline void CaptureMethodCacheEntry(const RuntimeDispatch &dispatch,
                                    MethodCacheEntryObservation &observation) {
  observation = MethodCacheEntryObservation{};
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      dispatch.receiver, dispatch.selector, &observation.entry);
  StabilizeMethodCacheEntryObservation(observation);
}

inline void CaptureCategoryAttachmentActions(
    CategoryAttachmentProtocolProbeRun &run) {
  const RuntimeDispatch category_dispatch =
      BuildWidgetInstanceDispatch(run, kCategorySelector);
  const RuntimeDispatch class_dispatch =
      BuildWidgetClassDispatch(run, kClassSelector);
  const RuntimeDispatch strict_error_dispatch =
      BuildWidgetInstanceDispatch(run, kProtocolStrictErrorSelector);
  run.values.category_value = DispatchRuntimeValue(category_dispatch);
  CaptureMethodCacheState(run.category_first_state);
  run.values.category_cached_value = DispatchRuntimeValue(category_dispatch);
  CaptureMethodCacheState(run.category_second_state);
  run.values.class_value = DispatchRuntimeValue(class_dispatch);
  run.values.protocol_strict_error =
      DispatchRuntimeValueChecked(strict_error_dispatch);
  CaptureMethodCacheState(run.method_state);
  CaptureMethodCacheEntry(category_dispatch, run.category_entry);
  CaptureMethodCacheEntry(strict_error_dispatch, run.strict_error_entry);
}

}  // namespace objc3c::runtime::probe::category_attachment_protocol_runtime
