#include "runtime/memory/arc_debug_state.h"

#include "runtime/metadata/runtime_emitted_records.h"

namespace objc3c::runtime {

RuntimeArcDebugState &RuntimeArcDebugStateForCurrentThread() {
  thread_local RuntimeArcDebugState state;
  return state;
}

void ResetRuntimeArcDebugStateForTesting() {
  RuntimeArcDebugStateForCurrentThread() = RuntimeArcDebugState{};
}

void RecordRuntimeArcDebugPropertyContext(const RuntimeDispatchFrame *frame) {
  RuntimeArcDebugState &state = RuntimeArcDebugStateForCurrentThread();
  state.last_property_receiver = frame != nullptr ? frame->receiver : 0;
  state.last_property_name.clear();
  state.last_property_owner_identity.clear();
  if (frame == nullptr || frame->runtime_property_accessor == nullptr ||
      frame->runtime_property_accessor->property_descriptor == nullptr) {
    return;
  }
  const EmittedPropertyDescriptor &descriptor =
      *frame->runtime_property_accessor->property_descriptor;
  if (descriptor.property_name != nullptr) {
    state.last_property_name = descriptor.property_name;
  }
  if (descriptor.declaration_owner_identity != nullptr) {
    state.last_property_owner_identity =
        descriptor.declaration_owner_identity;
  } else if (descriptor.export_owner_identity != nullptr) {
    state.last_property_owner_identity = descriptor.export_owner_identity;
  } else if (descriptor.owner_identity != nullptr) {
    state.last_property_owner_identity = descriptor.owner_identity;
  }
}

void CopyRuntimeArcDebugStateForTesting(
    objc3_runtime_arc_debug_state_snapshot *snapshot) {
  const RuntimeArcDebugState &state = RuntimeArcDebugStateForCurrentThread();
  snapshot->retain_call_count = state.retain_call_count;
  snapshot->release_call_count = state.release_call_count;
  snapshot->autorelease_call_count = state.autorelease_call_count;
  snapshot->autoreleasepool_push_count = state.autoreleasepool_push_count;
  snapshot->autoreleasepool_pop_count = state.autoreleasepool_pop_count;
  snapshot->current_property_read_count = state.current_property_read_count;
  snapshot->current_property_write_count = state.current_property_write_count;
  snapshot->current_property_exchange_count =
      state.current_property_exchange_count;
  snapshot->weak_current_property_load_count =
      state.weak_current_property_load_count;
  snapshot->weak_current_property_store_count =
      state.weak_current_property_store_count;
  snapshot->last_retain_value = state.last_retain_value;
  snapshot->last_release_value = state.last_release_value;
  snapshot->last_autorelease_value = state.last_autorelease_value;
  snapshot->last_property_read_value = state.last_property_read_value;
  snapshot->last_property_written_value = state.last_property_written_value;
  snapshot->last_property_exchange_previous_value =
      state.last_property_exchange_previous_value;
  snapshot->last_property_exchange_new_value =
      state.last_property_exchange_new_value;
  snapshot->last_weak_loaded_value = state.last_weak_loaded_value;
  snapshot->last_weak_stored_value = state.last_weak_stored_value;
  snapshot->last_property_receiver = state.last_property_receiver;
  snapshot->last_property_name =
      state.last_property_name.empty() ? nullptr : state.last_property_name.c_str();
  snapshot->last_property_owner_identity =
      state.last_property_owner_identity.empty()
          ? nullptr
          : state.last_property_owner_identity.c_str();
}

void CopyRuntimeArcFieldsToBlockArcSnapshot(
    objc3_runtime_block_arc_runtime_abi_snapshot *snapshot) {
  const RuntimeArcDebugState &state = RuntimeArcDebugStateForCurrentThread();
  snapshot->retain_call_count = state.retain_call_count;
  snapshot->release_call_count = state.release_call_count;
  snapshot->autorelease_call_count = state.autorelease_call_count;
  snapshot->autoreleasepool_push_count = state.autoreleasepool_push_count;
  snapshot->autoreleasepool_pop_count = state.autoreleasepool_pop_count;
  snapshot->current_property_read_count = state.current_property_read_count;
  snapshot->current_property_write_count = state.current_property_write_count;
  snapshot->current_property_exchange_count =
      state.current_property_exchange_count;
  snapshot->weak_current_property_load_count =
      state.weak_current_property_load_count;
  snapshot->weak_current_property_store_count =
      state.weak_current_property_store_count;
  snapshot->last_retain_value = state.last_retain_value;
  snapshot->last_release_value = state.last_release_value;
  snapshot->last_autorelease_value = state.last_autorelease_value;
}

}  // namespace objc3c::runtime

extern "C" int objc3_runtime_copy_arc_debug_state_for_testing(
    objc3_runtime_arc_debug_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  objc3c::runtime::CopyRuntimeArcDebugStateForTesting(snapshot);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
