#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_INSTRUMENTATION_ASSERTION_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_INSTRUMENTATION_ASSERTION_HELPERS_H_

#include "probe_state.h"

#include "runtime/public/objc3_runtime_api.h"

namespace objc3c::runtime::probe::arc_debug_instrumentation {

inline bool ArcDebugInstrumentationAssertionsPassed(const ProbeRun &run) {
  const ArcDebugFixture &fixture = run.fixture;
  const ArcInstrumentationResults &instrumentation = run.instrumentation;

  return fixture.parent > 0 && fixture.child > 0 &&
         instrumentation.bind_current_status ==
             OBJC3_RUNTIME_REGISTRATION_STATUS_OK &&
         instrumentation.bind_weak_status ==
             OBJC3_RUNTIME_REGISTRATION_STATUS_OK &&
         instrumentation.rebind_current_status ==
             OBJC3_RUNTIME_REGISTRATION_STATUS_OK &&
         instrumentation.rebind_weak_status ==
             OBJC3_RUNTIME_REGISTRATION_STATUS_OK &&
         instrumentation.retained == 9 &&
         instrumentation.autoreleased == instrumentation.retained &&
         instrumentation.getter_value == fixture.child &&
         instrumentation.weak_set_result == instrumentation.getter_value &&
         instrumentation.weak_inside_pool == instrumentation.getter_value &&
         instrumentation.released == instrumentation.retained &&
         instrumentation.parent_release_result == fixture.parent;
}

}  // namespace objc3c::runtime::probe::arc_debug_instrumentation

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_INSTRUMENTATION_ASSERTION_HELPERS_H_
