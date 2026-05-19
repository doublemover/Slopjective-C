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

void RecordRuntimeArcCurrentPropertyReadCall(
    const RuntimeDispatchFrame *frame) {
  RuntimeArcDebugState &state = RuntimeArcDebugStateForCurrentThread();
  ++state.current_property_read_count;
  RecordRuntimeArcDebugPropertyContext(frame);
}

void RecordRuntimeArcLastPropertyReadValue(int value) {
  RuntimeArcDebugStateForCurrentThread().last_property_read_value = value;
}

void RecordRuntimeArcCurrentPropertyWriteCall(
    const RuntimeDispatchFrame *frame,
    int value) {
  RuntimeArcDebugState &state = RuntimeArcDebugStateForCurrentThread();
  ++state.current_property_write_count;
  state.last_property_written_value = value;
  RecordRuntimeArcDebugPropertyContext(frame);
}

void RecordRuntimeArcCurrentPropertyExchangeCall(
    const RuntimeDispatchFrame *frame,
    int value) {
  RuntimeArcDebugState &state = RuntimeArcDebugStateForCurrentThread();
  ++state.current_property_exchange_count;
  state.last_property_exchange_new_value = value;
  RecordRuntimeArcDebugPropertyContext(frame);
}

void RecordRuntimeArcLastPropertyExchangePreviousValue(int value) {
  RuntimeArcDebugStateForCurrentThread().last_property_exchange_previous_value =
      value;
}

void RecordRuntimeArcWeakCurrentPropertyLoadCall() {
  ++RuntimeArcDebugStateForCurrentThread().weak_current_property_load_count;
}

void RecordRuntimeArcLastWeakCurrentPropertyLoadedValue(int value) {
  RuntimeArcDebugStateForCurrentThread().last_weak_loaded_value = value;
}

void RecordRuntimeArcWeakCurrentPropertyStoreCall(int value) {
  RuntimeArcDebugState &state = RuntimeArcDebugStateForCurrentThread();
  ++state.weak_current_property_store_count;
  state.last_weak_stored_value = value;
}

}  // namespace objc3c::runtime
