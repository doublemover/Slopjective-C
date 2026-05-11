#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_INSTRUMENTATION_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_INSTRUMENTATION_ORCHESTRATION_H_

#include "debug_state_capture.h"
#include "instrumentation_assertion_helpers.h"
#include "probe_state.h"

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c::runtime::probe::arc_debug_instrumentation {

inline int StoreWeakCurrentPropertyAndReportValue(int value) {
  ::objc3_runtime_store_weak_current_property_i32(value);
  return value;
}

inline void CaptureArcDebugInstrumentation(ProbeRun *run) {
  ArcInstrumentationResults &instrumentation = run->instrumentation;
  const ArcDebugFixture &fixture = run->fixture;

  instrumentation.bind_current_status =
      ::objc3_runtime_bind_current_property_context_for_testing(
          fixture.parent,
          "ArcBox",
          "currentValue");
  instrumentation.strong_set_result =
      ::objc3_runtime_exchange_current_property_i32(fixture.child);
  instrumentation.release_local_result =
      ::objc3_runtime_release_i32(fixture.child);
  instrumentation.retained = ::objc3_runtime_retain_i32(9);

  ::objc3_runtime_push_autoreleasepool_scope();
  instrumentation.autoreleased =
      ::objc3_runtime_autorelease_i32(instrumentation.retained);
  instrumentation.getter_value = ::objc3_runtime_read_current_property_i32();
  instrumentation.bind_weak_status =
      ::objc3_runtime_bind_current_property_context_for_testing(
          fixture.parent,
          "ArcBox",
          "weakValue");
  instrumentation.weak_set_result =
      StoreWeakCurrentPropertyAndReportValue(instrumentation.getter_value);
  instrumentation.weak_inside_pool =
      ::objc3_runtime_load_weak_current_property_i32();
  instrumentation.rebind_current_status =
      ::objc3_runtime_bind_current_property_context_for_testing(
          fixture.parent,
          "ArcBox",
          "currentValue");
  instrumentation.clear_strong_result =
      ::objc3_runtime_exchange_current_property_i32(0);
  instrumentation.rebind_weak_status =
      ::objc3_runtime_bind_current_property_context_for_testing(
          fixture.parent,
          "ArcBox",
          "weakValue");
  CaptureStableArcDebugSnapshot(&run->inside);

  ::objc3_runtime_pop_autoreleasepool_scope();
  instrumentation.weak_after_pool =
      ::objc3_runtime_load_weak_current_property_i32();
  ::objc3_runtime_clear_current_property_context_for_testing();
  instrumentation.released =
      ::objc3_runtime_release_i32(instrumentation.retained);
  instrumentation.parent_release_result =
      ::objc3_runtime_release_i32(fixture.parent);
  CaptureStableArcDebugSnapshot(&run->after);
  run->instrumentation_assertions_passed =
      ArcDebugInstrumentationAssertionsPassed(*run);
}

}  // namespace objc3c::runtime::probe::arc_debug_instrumentation

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_INSTRUMENTATION_ORCHESTRATION_H_
