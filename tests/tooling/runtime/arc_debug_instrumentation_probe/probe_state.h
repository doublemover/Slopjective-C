#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_PROBE_STATE_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_PROBE_STATE_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe::arc_debug_instrumentation {

struct StableArcDebugSnapshot {
  ::objc3_runtime_arc_debug_state_snapshot snapshot{};
  std::string property_name_storage;
  std::string owner_identity_storage;
};

struct ArcDebugFixture {
  int parent = 0;
  int child = 0;
};

struct ArcInstrumentationResults {
  int bind_current_status = 0;
  int strong_set_result = 0;
  int release_local_result = 0;
  int retained = 0;
  int autoreleased = 0;
  int getter_value = 0;
  int bind_weak_status = 0;
  int weak_set_result = 0;
  int rebind_current_status = 0;
  int clear_strong_result = 0;
  int rebind_weak_status = 0;
  int weak_inside_pool = 0;
  int weak_after_pool = 0;
  int released = 0;
  int parent_release_result = 0;
};

struct ProbeRun {
  ArcDebugFixture fixture;
  ArcInstrumentationResults instrumentation;
  StableArcDebugSnapshot inside;
  StableArcDebugSnapshot after;
  bool instrumentation_assertions_passed = false;
};

}  // namespace objc3c::runtime::probe::arc_debug_instrumentation

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_PROBE_STATE_H_
