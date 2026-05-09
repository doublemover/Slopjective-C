#include "runtime/memory/arc_debug_events.h"

#include "runtime/memory/arc_debug_state.h"

namespace objc3c::runtime {

void RecordRuntimeArcRetainCall(int value) {
  RuntimeArcDebugState &state = RuntimeArcDebugStateForCurrentThread();
  ++state.retain_call_count;
  state.last_retain_value = value;
}

void RecordRuntimeArcReleaseCall(int value) {
  RuntimeArcDebugState &state = RuntimeArcDebugStateForCurrentThread();
  ++state.release_call_count;
  state.last_release_value = value;
}

void RecordRuntimeArcAutoreleaseCall(int value) {
  RuntimeArcDebugState &state = RuntimeArcDebugStateForCurrentThread();
  ++state.autorelease_call_count;
  state.last_autorelease_value = value;
}

void RecordRuntimeArcAutoreleasePoolPushCall() {
  ++RuntimeArcDebugStateForCurrentThread().autoreleasepool_push_count;
}

void RecordRuntimeArcAutoreleasePoolPopCall() {
  ++RuntimeArcDebugStateForCurrentThread().autoreleasepool_pop_count;
}

}  // namespace objc3c::runtime
