#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_PROPERTY_IVAR_MATRIX_CASES_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_PROPERTY_IVAR_MATRIX_CASES_H_

#include "probe_result.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::property_ivar_execution_matrix {

inline void StabilizeDispatchObservation(DispatchObservation &observation) {
  ::objc3c::runtime::probe::StabilizeDispatchState(
      observation.state, observation.selector, observation.fast_path_reason,
      observation.path, observation.implementation_kind,
      observation.property_name, observation.class_name,
      observation.owner_identity);
}

inline void CaptureLatestDispatchObservation(DispatchObservation &observation) {
  observation = DispatchObservation{};
  (void)objc3_runtime_copy_dispatch_state_for_testing(&observation.state);
  StabilizeDispatchObservation(observation);
}

inline int DispatchVoidStatus(int receiver, const char *selector, int a0) {
  const objc3_runtime_dispatch_typed_result result =
      objc3_runtime_dispatch_typed_checked(receiver, selector, a0, 0, 0, 0);
  return result.status_code;
}

inline int DispatchI32Value(int receiver, const char *selector) {
  const objc3_runtime_dispatch_i32_result result =
      objc3_runtime_dispatch_i32_checked(receiver, selector, 0, 0, 0, 0);
  return result.status_code == OBJC3_RUNTIME_DISPATCH_STATUS_OK ? result.value
                                                                : 0;
}

inline int DispatchBoolValue(int receiver, const char *selector) {
  const objc3_runtime_dispatch_typed_result result =
      objc3_runtime_dispatch_typed_checked(receiver, selector, 0, 0, 0, 0);
  return result.status_code == OBJC3_RUNTIME_DISPATCH_STATUS_OK
             ? result.bool_value
             : 0;
}

inline int DispatchObjectReference(int receiver, const char *selector) {
  const objc3_runtime_dispatch_typed_result result =
      objc3_runtime_dispatch_typed_checked(receiver, selector, 0, 0, 0, 0);
  return result.status_code == OBJC3_RUNTIME_DISPATCH_STATUS_OK
             ? result.object_reference
             : 0;
}

inline void ExecutePropertyIvarMatrixCases(
    int widget_instance, PropertyIvarExecutionCases &cases) {
  cases = PropertyIvarExecutionCases{};
  cases.set_base_count_result =
      DispatchVoidStatus(widget_instance, kBaseCountSetterSelector, 21);
  CaptureLatestDispatchObservation(cases.set_base_count_dispatch);

  cases.base_count_value =
      DispatchI32Value(widget_instance, kBaseCountGetterSelector);
  CaptureLatestDispatchObservation(cases.base_count_dispatch);

  cases.set_count_result =
      DispatchVoidStatus(widget_instance, kCountSetterSelector, 37);
  CaptureLatestDispatchObservation(cases.set_count_dispatch);

  cases.count_value = DispatchI32Value(widget_instance, kCountGetterSelector);
  CaptureLatestDispatchObservation(cases.count_dispatch);

  cases.set_enabled_result =
      DispatchVoidStatus(widget_instance, kEnabledSetterSelector, 1);
  CaptureLatestDispatchObservation(cases.set_enabled_dispatch);

  cases.enabled_value =
      DispatchBoolValue(widget_instance, kEnabledGetterSelector);
  CaptureLatestDispatchObservation(cases.enabled_dispatch);

  cases.set_value_result =
      DispatchVoidStatus(widget_instance, kValueSetterSelector, 55);
  CaptureLatestDispatchObservation(cases.set_value_dispatch);

  cases.value_result =
      DispatchObjectReference(widget_instance, kValueGetterSelector);
  CaptureLatestDispatchObservation(cases.value_dispatch);

  cases.set_token_result =
      DispatchVoidStatus(widget_instance, kTokenSetterSelector, 89);
  CaptureLatestDispatchObservation(cases.set_token_dispatch);

  cases.token_value =
      DispatchObjectReference(widget_instance, kTokenGetterSelector);
  CaptureLatestDispatchObservation(cases.token_dispatch);
}

}  // namespace objc3c::runtime::probe::property_ivar_execution_matrix

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_PROPERTY_IVAR_MATRIX_CASES_H_
