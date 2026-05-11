#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_DEBUG_STATE_CAPTURE_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_DEBUG_STATE_CAPTURE_H_

#include "probe_state.h"

#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::arc_debug_instrumentation {

inline void CaptureStableArcDebugSnapshot(StableArcDebugSnapshot *state) {
  (void)::objc3_runtime_copy_arc_debug_state_for_testing(&state->snapshot);
  ::objc3c::runtime::probe::StabilizeArcDebugSnapshot(
      state->snapshot,
      state->property_name_storage,
      state->owner_identity_storage);
}

}  // namespace objc3c::runtime::probe::arc_debug_instrumentation

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_DEBUG_STATE_CAPTURE_H_
