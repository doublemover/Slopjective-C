#include "runtime/images/registration_snapshots.h"

#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"
#include "runtime/strings/borrowed_string.h"

#include <cstdint>
#include <mutex>

namespace objc3c::runtime {

void ClearImageWalkSnapshotUnlocked(RuntimeState &state) {
  state.last_discovery_root_entry_count = 0;
  state.last_walked_class_descriptor_count = 0;
  state.last_walked_protocol_descriptor_count = 0;
  state.last_walked_category_descriptor_count = 0;
  state.last_walked_property_descriptor_count = 0;
  state.last_walked_ivar_descriptor_count = 0;
  state.last_walked_selector_pool_count = 0;
  state.last_walked_string_pool_count = 0;
  state.last_walked_keypath_descriptor_count = 0;
  state.last_linker_anchor_matches_discovery_root = false;
  state.last_registration_used_staged_table = false;
  state.last_walked_module_name.clear();
  state.last_walked_translation_unit_identity_key.clear();
}

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
  snapshot->last_walked_string_pool_count = state.last_walked_string_pool_count;
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

int CopyRuntimeRegistrationStateForTesting(
    objc3_runtime_registration_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->abi_version = OBJC3_RUNTIME_REGISTRATION_STATE_SNAPSHOT_ABI_VERSION;
  snapshot->snapshot_size = sizeof(objc3_runtime_registration_state_snapshot);
  snapshot->registered_image_count = state.registered_image_count;
  snapshot->registered_descriptor_total = state.registered_descriptor_total;
  snapshot->next_expected_registration_order_ordinal =
      state.next_expected_registration_order_ordinal;
  snapshot->last_successful_registration_order_ordinal =
      state.last_successful_registration_order_ordinal;
  snapshot->last_registration_status = state.last_registration_status;
  (void)RuntimeCStringSnapshotOwnershipModel();
  snapshot->last_registered_module_name =
      BorrowRuntimeCString(state.last_registered_module_name);
  snapshot->last_registered_translation_unit_identity_key =
      BorrowRuntimeCString(state.last_registered_translation_unit_identity_key);
  snapshot->last_rejected_module_name =
      BorrowRuntimeCString(state.last_rejected_module_name);
  snapshot->last_rejected_translation_unit_identity_key =
      BorrowRuntimeCString(state.last_rejected_translation_unit_identity_key);
  snapshot->last_rejected_registration_order_ordinal =
      state.last_rejected_registration_order_ordinal;
  snapshot->owner_split_contract_id =
      BorrowRuntimeCString(state.owner_split_contract_id);
  snapshot->metadata_model_owner =
      BorrowRuntimeCString(state.metadata_model_owner);
  snapshot->registration_table_owner =
      BorrowRuntimeCString(state.registration_table_owner);
  snapshot->manifest_descriptor_artifact_owner =
      BorrowRuntimeCString(state.manifest_descriptor_artifact_owner);
  snapshot->bootstrap_replay_owner =
      BorrowRuntimeCString(state.bootstrap_replay_owner);
  snapshot->public_registration_api_owner =
      BorrowRuntimeCString(state.public_registration_api_owner);
  snapshot->public_dispatch_diagnostics_owner =
      BorrowRuntimeCString(state.public_dispatch_diagnostics_owner);
  snapshot->fail_closed_ownership_model =
      BorrowRuntimeCString(state.fail_closed_ownership_model);
  snapshot->runtime_owner_split_explicit =
      state.runtime_owner_split_explicit ? 1 : 0;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int CopyRuntimeResetReplayStateForTesting(
    objc3_runtime_reset_replay_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->retained_bootstrap_image_count = static_cast<std::uint64_t>(
      state.retained_bootstrap_identity_order.size());
  snapshot->live_registration_order_entry_count = static_cast<std::uint64_t>(
      state.registration_order_by_identity_key.size());
  snapshot->live_registered_metadata_entry_count = static_cast<std::uint64_t>(
      state.registered_image_metadata_by_identity_key.size());
  snapshot->live_selector_table_entry_count =
      static_cast<std::uint64_t>(state.selector_slots.size());
  snapshot->live_keypath_entry_count =
      static_cast<std::uint64_t>(state.keypath_slots.size());
  snapshot->live_realized_class_count =
      static_cast<std::uint64_t>(state.realized_class_nodes.size());
  snapshot->live_method_cache_entry_count =
      static_cast<std::uint64_t>(state.method_cache.size());
  snapshot->live_property_lookup_cache_entry_count =
      static_cast<std::uint64_t>(state.property_lookup_cache.size());
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

} // namespace objc3c::runtime

extern "C" int objc3_runtime_copy_image_walk_state_for_testing(
    objc3_runtime_image_walk_state_snapshot *snapshot) {
  return objc3c::runtime::CopyRuntimeImageWalkStateForTesting(snapshot);
}

extern "C" int objc3_runtime_copy_registration_state_for_testing(
    objc3_runtime_registration_state_snapshot *snapshot) {
  return objc3c::runtime::CopyRuntimeRegistrationStateForTesting(snapshot);
}

extern "C" int objc3_runtime_copy_reset_replay_state_for_testing(
    objc3_runtime_reset_replay_state_snapshot *snapshot) {
  return objc3c::runtime::CopyRuntimeResetReplayStateForTesting(snapshot);
}
