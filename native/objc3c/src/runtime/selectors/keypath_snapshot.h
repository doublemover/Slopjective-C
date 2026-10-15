#pragma once

#include "runtime/selectors/runtime_selector_snapshot_contracts.h"
#include "runtime/selectors/keypath_query.h"
#include "runtime/strings/borrowed_string.h"

namespace objc3c::runtime {

inline void CopyRuntimeKeyPathRegistryStateSnapshotUnlocked(
    const RuntimeState &state,
    objc3_runtime_keypath_registry_state_snapshot *snapshot) {
  snapshot->keypath_table_entry_count =
      static_cast<std::uint64_t>(state.keypath_slots.size());
  snapshot->image_backed_keypath_count = state.image_backed_keypath_count;
  snapshot->ambiguous_keypath_handle_count =
      state.ambiguous_keypath_handle_count;
  snapshot->last_materialized_handle = state.last_materialized_keypath_handle;
  snapshot->last_materialized_registration_order_ordinal =
      state.last_materialized_keypath_registration_order_ordinal;
  snapshot->last_queried_handle = state.last_queried_keypath_handle;
  snapshot->last_query_found = state.last_keypath_query_found ? 1 : 0;
  snapshot->last_query_ambiguous = state.last_keypath_query_ambiguous ? 1 : 0;
  snapshot->last_materialized_profile =
      BorrowRuntimeCString(state.last_materialized_keypath_profile);
  snapshot->last_resolved_profile =
      BorrowRuntimeCString(state.last_resolved_keypath_profile);
}

inline void InitializeRuntimeKeyPathEntrySnapshot(
    std::uint64_t stable_id,
    objc3_runtime_keypath_entry_snapshot *snapshot) {
  snapshot->found = 0;
  snapshot->ambiguous = 0;
  snapshot->root_is_self = 0;
  snapshot->stable_id = stable_id;
  snapshot->component_count = 0;
  snapshot->metadata_provider_count = 0;
  snapshot->first_registration_order_ordinal = 0;
  snapshot->last_registration_order_ordinal = 0;
  snapshot->root_name = nullptr;
  snapshot->component_path = nullptr;
  snapshot->profile = nullptr;
  snapshot->generic_metadata_replay_key = nullptr;
  snapshot->component_owner_identity_path = nullptr;
  snapshot->component_member_identity_path = nullptr;
  snapshot->component_type_identity_path = nullptr;
}

inline void CopyRuntimeKeyPathEntrySnapshotUnlocked(
    RuntimeState &state,
    std::uint64_t stable_id,
    objc3_runtime_keypath_entry_snapshot *snapshot) {
  InitializeRuntimeKeyPathEntrySnapshot(stable_id, snapshot);

  const KeyPathSlot *slot =
      ResolveRuntimeKeyPathQueryUnlocked(state, stable_id);
  if (slot == nullptr) {
    return;
  }

  snapshot->found = 1;
  snapshot->ambiguous = slot->ambiguous ? 1 : 0;
  snapshot->root_is_self = slot->root_is_self ? 1 : 0;
  snapshot->component_count = slot->component_count;
  snapshot->metadata_provider_count = slot->metadata_provider_count;
  snapshot->first_registration_order_ordinal =
      slot->first_registration_order_ordinal;
  snapshot->last_registration_order_ordinal =
      slot->last_registration_order_ordinal;
  snapshot->root_name = BorrowRuntimeCString(slot->root_name_storage);
  snapshot->component_path =
      BorrowRuntimeCString(slot->component_path_storage);
  snapshot->profile = BorrowRuntimeCString(slot->profile_storage);
  snapshot->generic_metadata_replay_key =
      slot->generic_metadata_replay_key_storage.empty()
          ? nullptr
          : BorrowRuntimeCString(slot->generic_metadata_replay_key_storage);
  snapshot->component_owner_identity_path =
      BorrowRuntimeCString(slot->component_owner_identity_path_storage);
  snapshot->component_member_identity_path =
      BorrowRuntimeCString(slot->component_member_identity_path_storage);
  snapshot->component_type_identity_path =
      BorrowRuntimeCString(slot->component_type_identity_path_storage);
}

inline int RuntimeKeyPathComponentCountForTestingUnlocked(
    RuntimeState &state,
    std::uint64_t stable_id) {
  const KeyPathSlot *slot =
      ResolveRuntimeKeyPathQueryUnlocked(state, stable_id);
  if (!RuntimeKeyPathQueryCanUseSlot(slot)) {
    return 0;
  }
  return static_cast<int>(slot->component_count);
}

inline int RuntimeKeyPathRootIsSelfForTestingUnlocked(
    RuntimeState &state,
    std::uint64_t stable_id) {
  const KeyPathSlot *slot =
      ResolveRuntimeKeyPathQueryUnlocked(state, stable_id);
  if (!RuntimeKeyPathQueryCanUseSlot(slot)) {
    return 0;
  }
  return slot->root_is_self ? 1 : 0;
}

}  // namespace objc3c::runtime
