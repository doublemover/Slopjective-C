#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_SAMPLE_EXECUTION_ASSERTIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_SAMPLE_EXECUTION_ASSERTIONS_H_

#include "sample_fixture_definitions.h"
#include "support/typed_dispatch_helpers.h"

namespace objc3c::runtime::probe::canonical_runnable_sample_set {

inline int DispatchI32(int receiver, const char *selector, int arg0 = 0) {
  return objc3_runtime_dispatch_i32(receiver, selector, arg0, 0, 0, 0);
}

inline int DispatchObjectReference(int receiver, const char *selector,
                                   int arg0 = 0) {
  return ::objc3c::runtime::probe::DispatchTypedObjectReference(receiver,
                                                                selector,
                                                                arg0);
}

inline int DispatchBool(int receiver, const char *selector) {
  return ::objc3c::runtime::probe::DispatchTypedBoolValue(receiver, selector);
}

inline int DispatchVoidStatus(int receiver, const char *selector,
                              int arg0 = 0) {
  return ::objc3c::runtime::probe::DispatchTypedStatus(receiver, selector,
                                                       arg0);
}

inline RunnableSampleExecution ExecuteRunnableSampleSet(
    const RunnableSampleFixture &fixture) {
  RunnableSampleExecution execution;
  execution.widget_instance =
      DispatchObjectReference(fixture.widget_class_receiver, kAllocSelector);
  execution.init_value =
      DispatchObjectReference(execution.widget_instance, kInitSelector);
  execution.traced_value =
      DispatchI32(execution.init_value, kTracedValueSelector);
  execution.inherited_value =
      DispatchI32(execution.init_value, kInheritedValueSelector);
  execution.class_value =
      DispatchI32(fixture.widget_class_receiver, kClassValueSelector);
  execution.shared_value =
      DispatchI32(fixture.widget_class_receiver, kSharedSelector);
  (void)DispatchVoidStatus(execution.init_value, kSetCountSelector, 37);
  execution.count_value = DispatchI32(execution.init_value, kCountSelector);
  (void)DispatchVoidStatus(execution.init_value, kSetEnabledSelector, 1);
  execution.enabled_value = DispatchBool(execution.init_value, kEnabledSelector);
  (void)DispatchVoidStatus(execution.init_value, kSetCurrentValueSelector, 55);
  execution.current_value =
      DispatchObjectReference(execution.init_value, kCurrentValueSelector);
  execution.token_value =
      DispatchObjectReference(execution.init_value, kTokenValueSelector);
  return execution;
}

}  // namespace objc3c::runtime::probe::canonical_runnable_sample_set

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_SAMPLE_EXECUTION_ASSERTIONS_H_
