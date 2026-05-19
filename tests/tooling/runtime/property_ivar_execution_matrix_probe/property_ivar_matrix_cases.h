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

inline void ExecutePropertyIvarMatrixCases(
    int widget_instance, PropertyIvarExecutionCases &cases) {
  cases = PropertyIvarExecutionCases{};
  cases.set_count_result = objc3_runtime_dispatch_i32(
      widget_instance, kCountSetterSelector, 37, 0, 0, 0);
  CaptureLatestDispatchObservation(cases.set_count_dispatch);

  cases.count_value = objc3_runtime_dispatch_i32(
      widget_instance, kCountGetterSelector, 0, 0, 0, 0);
  CaptureLatestDispatchObservation(cases.count_dispatch);

  cases.set_enabled_result = objc3_runtime_dispatch_i32(
      widget_instance, kEnabledSetterSelector, 1, 0, 0, 0);
  CaptureLatestDispatchObservation(cases.set_enabled_dispatch);

  cases.enabled_value = objc3_runtime_dispatch_i32(
      widget_instance, kEnabledGetterSelector, 0, 0, 0, 0);
  CaptureLatestDispatchObservation(cases.enabled_dispatch);

  cases.set_value_result = objc3_runtime_dispatch_i32(
      widget_instance, kValueSetterSelector, 55, 0, 0, 0);
  CaptureLatestDispatchObservation(cases.set_value_dispatch);

  cases.value_result = objc3_runtime_dispatch_i32(
      widget_instance, kValueGetterSelector, 0, 0, 0, 0);
  CaptureLatestDispatchObservation(cases.value_dispatch);

  cases.token_value = objc3_runtime_dispatch_i32(
      widget_instance, kTokenGetterSelector, 0, 0, 0, 0);
  CaptureLatestDispatchObservation(cases.token_dispatch);
}

}  // namespace objc3c::runtime::probe::property_ivar_execution_matrix

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_PROPERTY_IVAR_MATRIX_CASES_H_
