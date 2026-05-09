#include "runtime/storage/current_property_context.h"

#include "runtime/memory/arc_debug_state.h"
#include "runtime/memory/autorelease_pool.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"
#include "runtime/storage/property_accessors.h"
#include "runtime/storage/runtime_instance_records.h"

#include <mutex>

namespace objc3c::runtime {

int ReadCurrentPropertyI32() {
  RuntimeDispatchFrame *frame = CurrentRuntimeDispatchFrame();
  RuntimeArcDebugState &arc_debug = RuntimeArcDebugStateForCurrentThread();
  ++arc_debug.current_property_read_count;
  RecordRuntimeArcDebugPropertyContext(frame);
  if (frame == nullptr || frame->runtime_property_accessor == nullptr ||
      frame->receiver == 0) {
    arc_debug.last_property_read_value = 0;
    return 0;
  }
  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  const auto instance_it =
      state.runtime_instances_by_receiver.find(frame->receiver);
  if (instance_it == state.runtime_instances_by_receiver.end()) {
    arc_debug.last_property_read_value = 0;
    return 0;
  }
  int value = 0;
  const int result =
      ReadRuntimeManagedPropertyValueUnlocked(
          state, instance_it->second, *frame->runtime_property_accessor, value)
          ? value
          : 0;
  arc_debug.last_property_read_value = result;
  return result;
}

void WriteCurrentPropertyI32(int value) {
  RuntimeDispatchFrame *frame = CurrentRuntimeDispatchFrame();
  RuntimeArcDebugState &arc_debug = RuntimeArcDebugStateForCurrentThread();
  ++arc_debug.current_property_write_count;
  arc_debug.last_property_written_value = value;
  RecordRuntimeArcDebugPropertyContext(frame);
  if (frame == nullptr || frame->runtime_property_accessor == nullptr ||
      frame->receiver == 0) {
    return;
  }
  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  const auto instance_it =
      state.runtime_instances_by_receiver.find(frame->receiver);
  if (instance_it == state.runtime_instances_by_receiver.end()) {
    return;
  }
  (void)WriteRuntimeManagedPropertyValueUnlocked(
      state, instance_it->second, *frame->runtime_property_accessor, value);
}

int ExchangeCurrentPropertyI32(int value) {
  RuntimeDispatchFrame *frame = CurrentRuntimeDispatchFrame();
  RuntimeArcDebugState &arc_debug = RuntimeArcDebugStateForCurrentThread();
  ++arc_debug.current_property_exchange_count;
  arc_debug.last_property_exchange_new_value = value;
  RecordRuntimeArcDebugPropertyContext(frame);
  if (frame == nullptr || frame->runtime_property_accessor == nullptr ||
      frame->receiver == 0) {
    arc_debug.last_property_exchange_previous_value = 0;
    return 0;
  }
  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  const auto instance_it =
      state.runtime_instances_by_receiver.find(frame->receiver);
  if (instance_it == state.runtime_instances_by_receiver.end()) {
    arc_debug.last_property_exchange_previous_value = 0;
    return 0;
  }
  int previous_value = 0;
  const int result =
      ExchangeRuntimeManagedPropertyValueUnlocked(
          state, instance_it->second, *frame->runtime_property_accessor, value,
          previous_value)
          ? previous_value
          : 0;
  arc_debug.last_property_exchange_previous_value = result;
  return result;
}

}  // namespace objc3c::runtime

extern "C" int objc3_runtime_read_current_property_i32(void) {
  return objc3c::runtime::ReadCurrentPropertyI32();
}

extern "C" void objc3_runtime_write_current_property_i32(int value) {
  objc3c::runtime::WriteCurrentPropertyI32(value);
}

extern "C" int objc3_runtime_exchange_current_property_i32(int value) {
  return objc3c::runtime::ExchangeCurrentPropertyI32(value);
}
