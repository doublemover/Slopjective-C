#pragma once

#include "probe_state.h"
#include "support/dispatch_expectations.h"

namespace objc3c {
namespace runtime {
namespace probe {
namespace runtime_canonical_runnable_object {

inline void CaptureRunnableInvocationAssertions(ProbeRun &run) {
  RuntimeFixture &fixture = run.fixture;
  RunnableInvocationAssertions &runnable = run.runnable;

  runnable.alloc_value = objc3_runtime_dispatch_i32(
      fixture.widget_class_receiver, "alloc", 0, 0, 0, 0);
  runnable.init_value = objc3_runtime_dispatch_i32(runnable.alloc_value, "init",
                                                   0, 0, 0, 0);
  runnable.new_value = objc3_runtime_dispatch_i32(
      fixture.widget_class_receiver, "new", 0, 0, 0, 0);
  runnable.inherited_value = objc3_runtime_dispatch_i32(
      runnable.init_value, "inheritedValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &runnable.inherited_state);
  runnable.traced_value = objc3_runtime_dispatch_i32(
      runnable.init_value, "tracedValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &runnable.traced_state);
  runnable.class_value = objc3_runtime_dispatch_i32(
      fixture.widget_class_receiver, "classValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &runnable.class_state);
  runnable.ignored_result = objc3_runtime_dispatch_i32_checked(
      runnable.init_value, "ignoredValue", 0, 0, 0, 0);
  runnable.ignored_value = runnable.ignored_result.value;
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &runnable.ignored_state);
  runnable.ignored_cached_result = objc3_runtime_dispatch_i32_checked(
      runnable.init_value, "ignoredValue", 0, 0, 0, 0);
  runnable.ignored_cached_value = runnable.ignored_cached_result.value;
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &runnable.ignored_cached_state);
  runnable.ignored_expected = ::objc3c::runtime::probe::
      ExpectedStrictDispatchErrorValue(runnable.init_value, "ignoredValue", 0,
                                       0, 0, 0);
}

inline bool RunnableInvocationAssertionsPassed(
    const RunnableInvocationAssertions &runnable) {
  return ::objc3c::runtime::probe::HasDispatchStatus(
             runnable.ignored_result,
             OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR, 0, "O3RT001",
             "runtime dispatch failed: unknown selector") &&
         ::objc3c::runtime::probe::HasDispatchStatus(
             runnable.ignored_cached_result,
             OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR, 0, "O3RT001",
             "runtime dispatch failed: unknown selector");
}

} // namespace runtime_canonical_runnable_object
} // namespace probe
} // namespace runtime
} // namespace objc3c
