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

inline RuntimeDispatch BuildNilReceiverDispatch(const char *selector) {
  return RuntimeDispatch{0, selector};
}

inline int DispatchRuntimeValue(const RuntimeDispatch &dispatch) {
  return objc3_runtime_dispatch_i32(dispatch.receiver, dispatch.selector, 0, 0,
                                    0, 0);
}

inline int DispatchRuntimeValueFromClass(const RuntimeDispatch &dispatch,
                                         const char *lookup_start_class_name) {
  return objc3_runtime_dispatch_i32_from_class(
      dispatch.receiver, lookup_start_class_name, dispatch.selector, 0, 0, 0,
      0);
}

inline objc3_runtime_dispatch_i32_result DispatchRuntimeI32FromClassChecked(
    const RuntimeDispatch &dispatch, const char *lookup_start_class_name) {
  return objc3_runtime_dispatch_i32_from_class_checked(
      dispatch.receiver, lookup_start_class_name, dispatch.selector, 0, 0, 0,
      0);
}

inline int DispatchRuntimeTypedValue(
    objc3_runtime_dispatch_return_kind_code expected_return_kind,
    const RuntimeDispatch &dispatch) {
  return objc3_runtime_dispatch_typed_value(
      expected_return_kind, dispatch.receiver, dispatch.selector, 0, 0, 0, 0);
}

inline objc3_runtime_dispatch_i32_result DispatchRuntimeI32Checked(
    const RuntimeDispatch &dispatch) {
  return objc3_runtime_dispatch_i32_checked(dispatch.receiver, dispatch.selector,
                                           0, 0, 0, 0);
}

inline objc3_runtime_dispatch_typed_result DispatchRuntimeTypedChecked(
    const RuntimeDispatch &dispatch) {
  return objc3_runtime_dispatch_typed_checked(dispatch.receiver,
                                             dispatch.selector, 0, 0, 0, 0);
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
  const RuntimeDispatch auxiliary_category_dispatch =
      BuildWidgetInstanceDispatch(run, kAuxiliaryCategorySelector);
  const RuntimeDispatch category_bool_dispatch =
      BuildWidgetInstanceDispatch(run, kCategoryBoolSelector);
  const RuntimeDispatch class_dispatch =
      BuildWidgetClassDispatch(run, kClassSelector);
  const RuntimeDispatch super_dispatch =
      BuildWidgetInstanceDispatch(run, kSuperclassSelector);
  const RuntimeDispatch nil_receiver_dispatch =
      BuildNilReceiverDispatch(kCategorySelector);
  const RuntimeDispatch strict_error_dispatch =
      BuildWidgetInstanceDispatch(run, kProtocolStrictErrorSelector);
  run.values.category_value = DispatchRuntimeValue(category_dispatch);
  CaptureMethodCacheState(run.category_first_state);
  run.values.category_cached_value = DispatchRuntimeValue(category_dispatch);
  CaptureMethodCacheState(run.category_second_state);
  run.values.auxiliary_category_value =
      DispatchRuntimeValue(auxiliary_category_dispatch);
  run.values.category_bool_value = DispatchRuntimeTypedValue(
      OBJC3_RUNTIME_DISPATCH_RETURN_KIND_BOOL, category_bool_dispatch);
  run.values.category_bool_typed_result =
      DispatchRuntimeTypedChecked(category_bool_dispatch);
  run.values.class_value = DispatchRuntimeValue(class_dispatch);
  run.values.super_inherited_i32_result =
      DispatchRuntimeI32FromClassChecked(super_dispatch, kBaseClassName);
  run.values.super_inherited_value =
      run.values.super_inherited_i32_result.value;
  CaptureMethodCacheState(run.super_first_state);
  run.values.super_cached_inherited_value =
      DispatchRuntimeValueFromClass(super_dispatch, kBaseClassName);
  CaptureMethodCacheState(run.super_second_state);
  run.values.nil_receiver_i32_result =
      DispatchRuntimeI32Checked(nil_receiver_dispatch);
  run.values.nil_receiver_value = run.values.nil_receiver_i32_result.value;
  run.values.nil_receiver_typed_result =
      DispatchRuntimeTypedChecked(nil_receiver_dispatch);
  run.values.protocol_strict_error_i32_result =
      DispatchRuntimeI32Checked(strict_error_dispatch);
  run.values.protocol_strict_error =
      run.values.protocol_strict_error_i32_result.value;
  run.values.protocol_strict_error_typed_result =
      DispatchRuntimeTypedChecked(strict_error_dispatch);
  CaptureMethodCacheState(run.method_state);
  CaptureMethodCacheEntry(category_dispatch, run.category_entry);
  CaptureMethodCacheEntry(strict_error_dispatch, run.strict_error_entry);
}

}  // namespace objc3c::runtime::probe::category_attachment_protocol_runtime
