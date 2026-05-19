#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_ARC_FIXTURE_SETUP_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_ARC_FIXTURE_SETUP_H_

#include "probe_state.h"

#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/typed_dispatch_helpers.h"

namespace objc3c::runtime::probe::arc_debug_instrumentation {

inline void ResetAndReplayRuntimeForArcDebugProbe() {
  ::objc3_runtime_reset_for_testing();
  (void)::objc3_runtime_replay_registered_images_for_testing();
}

inline ArcDebugFixture SetUpArcDebugFixture() {
  ArcDebugFixture fixture;
  fixture.parent =
      ::objc3c::runtime::probe::DispatchTypedObjectReference(1024, "alloc");
  fixture.child =
      ::objc3c::runtime::probe::DispatchTypedObjectReference(1024, "alloc");
  return fixture;
}

}  // namespace objc3c::runtime::probe::arc_debug_instrumentation

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_ARC_FIXTURE_SETUP_H_
