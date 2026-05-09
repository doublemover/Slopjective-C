#include "runtime/storage/current_property_context.h"

#include "runtime/memory/arc_debug_state.h"
#include "runtime/memory/autorelease_pool.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"
#include "runtime/storage/property_accessors.h"
#include "runtime/storage/runtime_instance_records.h"

#include <cstddef>
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

int BindCurrentPropertyContextForTesting(
    int receiver, const char *class_name, const char *property_name) {
  if (receiver == 0 || class_name == nullptr || class_name[0] == '\0' ||
      property_name == nullptr || property_name[0] == '\0') {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  const auto instance_it = state.runtime_instances_by_receiver.find(receiver);
  const auto found = state.realized_class_node_indices_by_name.find(class_name);
  if (instance_it == state.runtime_instances_by_receiver.end() ||
      found == state.realized_class_node_indices_by_name.end() ||
      found->second.empty()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }
  const std::size_t node_index = found->second.front();
  if (node_index >= state.realized_class_nodes.size()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  const RealizedClassNode &start_node = state.realized_class_nodes[node_index];
  const RealizedClassNode *resolved_node = nullptr;
  bool inherited = false;
  bool used_cache = false;
  const RealizedPropertyAccessor *accessor =
      FindRuntimePropertyAccessorByNameUnlocked(
          state, start_node, property_name, resolved_node, inherited,
          used_cache);
  state.last_property_query_used_cache = used_cache;
  if (accessor == nullptr || resolved_node == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeDispatchFrame *testing_frame = SetRuntimeTestingDispatchFrame(
      receiver, instance_it->second.base_identity, accessor);
  RecordRuntimeArcDebugPropertyContext(testing_frame);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

void ClearCurrentPropertyContextForTesting() {
  ClearRuntimeTestingDispatchFrame();
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

extern "C" int objc3_runtime_bind_current_property_context_for_testing(
    int receiver, const char *class_name, const char *property_name) {
  return objc3c::runtime::BindCurrentPropertyContextForTesting(
      receiver, class_name, property_name);
}

extern "C" void objc3_runtime_clear_current_property_context_for_testing(void) {
  objc3c::runtime::ClearCurrentPropertyContextForTesting();
}
