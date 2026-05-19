#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_SYNTHESIZED_ACCESSOR_ACTIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_SYNTHESIZED_ACCESSOR_ACTIONS_H_

#include "accessor_fixture_definitions.h"
#include "probe_result.h"

namespace objc3c::runtime::probe::synthesized_accessor {

inline int DispatchVoidStatus(int receiver, const char *selector, int a0) {
  const objc3_runtime_dispatch_typed_result result =
      objc3_runtime_dispatch_typed_checked(receiver, selector, a0, 0, 0, 0);
  return result.status_code;
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

inline AccessorActions ExecuteSynthesizedAccessorActions() {
  AccessorActions actions;
  actions.widget_instance =
      DispatchObjectReference(kWidgetClassReceiver, kAllocSelector);
  actions.set_count_result = DispatchVoidStatus(
      actions.widget_instance, kCountSetterSelector, kCountSetterValue);
  actions.count_value = objc3_runtime_dispatch_i32(
      actions.widget_instance, kCountGetterSelector, 0, 0, 0, 0);
  actions.set_enabled_result = DispatchVoidStatus(
      actions.widget_instance, kEnabledSetterSelector, kEnabledSetterValue);
  actions.enabled_value = DispatchBoolValue(
      actions.widget_instance, kEnabledGetterSelector);
  actions.set_value_result = DispatchVoidStatus(
      actions.widget_instance, kValueSetterSelector, kValueSetterValue);
  actions.value_result = DispatchObjectReference(
      actions.widget_instance, kValueGetterSelector);
  return actions;
}

}  // namespace objc3c::runtime::probe::synthesized_accessor

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_SYNTHESIZED_ACCESSOR_ACTIONS_H_
