#include "runtime/images/registration_snapshots.h"

#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"
#include "runtime/strings/borrowed_string.h"

#include <cstdint>
#include <mutex>

namespace objc3c::runtime {

int CopyRuntimeImageWalkStateForTesting(
    objc3_runtime_image_walk_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->walked_image_count = state.walked_image_count;
  snapshot->last_discovery_root_entry_count =
      state.last_discovery_root_entry_count;
  snapshot->last_walked_class_descriptor_count =
      state.last_walked_class_descriptor_count;
  snapshot->last_walked_protocol_descriptor_count =
      state.last_walked_protocol_descriptor_count;
  snapshot->last_walked_category_descriptor_count =
      state.last_walked_category_descriptor_count;
  snapshot->last_walked_property_descriptor_count =
      state.last_walked_property_descriptor_count;
  snapshot->last_walked_ivar_descriptor_count =
      state.last_walked_ivar_descriptor_count;
  snapshot->last_walked_selector_pool_count =
      state.last_walked_selector_pool_count;
  snapshot->last_walked_string_pool_count =
      state.last_walked_string_pool_count;
  snapshot->last_walked_keypath_descriptor_count =
      state.last_walked_keypath_descriptor_count;
  snapshot->last_linker_anchor_matches_discovery_root =
      state.last_linker_anchor_matches_discovery_root ? 1 : 0;
  snapshot->last_registration_used_staged_table =
      state.last_registration_used_staged_table ? 1 : 0;
  snapshot->last_walked_module_name =
      BorrowRuntimeCString(state.last_walked_module_name);
  snapshot->last_walked_translation_unit_identity_key =
      BorrowRuntimeCString(state.last_walked_translation_unit_identity_key);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int CopyRuntimeResetReplayStateForTesting(
    objc3_runtime_reset_replay_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->retained_bootstrap_image_count =
      static_cast<std::uint64_t>(state.retained_bootstrap_identity_order.size());
  snapshot->last_reset_cleared_image_local_init_state_count =
      state.last_reset_cleared_image_local_init_state_count;
  snapshot->last_replayed_image_count = state.last_replayed_image_count;
  snapshot->reset_generation = state.reset_generation;
  snapshot->replay_generation = state.replay_generation;
  snapshot->last_replay_status = state.last_replay_status;
  snapshot->last_replayed_module_name =
      BorrowRuntimeCString(state.last_replayed_module_name);
  snapshot->last_replayed_translation_unit_identity_key =
      BorrowRuntimeCString(state.last_replayed_translation_unit_identity_key);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

}  // namespace objc3c::runtime

extern "C" int objc3_runtime_copy_image_walk_state_for_testing(
    objc3_runtime_image_walk_state_snapshot *snapshot) {
  return objc3c::runtime::CopyRuntimeImageWalkStateForTesting(snapshot);
}

extern "C" int objc3_runtime_copy_reset_replay_state_for_testing(
    objc3_runtime_reset_replay_state_snapshot *snapshot) {
  return objc3c::runtime::CopyRuntimeResetReplayStateForTesting(snapshot);
}
