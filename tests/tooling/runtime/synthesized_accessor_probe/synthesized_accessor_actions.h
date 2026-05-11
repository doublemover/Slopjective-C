#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_SYNTHESIZED_ACCESSOR_ACTIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_SYNTHESIZED_ACCESSOR_ACTIONS_H_

#include "accessor_fixture_definitions.h"
#include "probe_result.h"

namespace objc3c::runtime::probe::synthesized_accessor {

inline AccessorActions ExecuteSynthesizedAccessorActions() {
  AccessorActions actions;
  actions.widget_instance = objc3_runtime_dispatch_i32(
      kWidgetClassReceiver, kAllocSelector, 0, 0, 0, 0);
  actions.set_count_result = objc3_runtime_dispatch_i32(
      actions.widget_instance, kCountSetterSelector, kCountSetterValue, 0, 0,
      0);
  actions.count_value = objc3_runtime_dispatch_i32(
      actions.widget_instance, kCountGetterSelector, 0, 0, 0, 0);
  actions.set_enabled_result = objc3_runtime_dispatch_i32(
      actions.widget_instance, kEnabledSetterSelector, kEnabledSetterValue, 0,
      0, 0);
  actions.enabled_value = objc3_runtime_dispatch_i32(
      actions.widget_instance, kEnabledGetterSelector, 0, 0, 0, 0);
  actions.set_value_result = objc3_runtime_dispatch_i32(
      actions.widget_instance, kValueSetterSelector, kValueSetterValue, 0, 0,
      0);
  actions.value_result = objc3_runtime_dispatch_i32(
      actions.widget_instance, kValueGetterSelector, 0, 0, 0, 0);
  return actions;
}

}  // namespace objc3c::runtime::probe::synthesized_accessor

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_SYNTHESIZED_ACCESSOR_ACTIONS_H_
