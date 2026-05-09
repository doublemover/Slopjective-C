#pragma once

#include "runtime/selectors/keypath_descriptor.h"
#include "runtime/state/runtime_state_records.h"

#include <cstdint>
#include <utility>

namespace objc3c::runtime {

inline KeyPathSlot *FindRuntimeKeyPathSlotUnlocked(
    RuntimeState &state,
    std::uint64_t stable_id) {
  const auto found = state.keypath_slots.find(stable_id);
  if (found == state.keypath_slots.end()) {
    return nullptr;
  }
  return &found->second;
}

inline const KeyPathSlot *FindRuntimeKeyPathSlotUnlocked(
    const RuntimeState &state,
    std::uint64_t stable_id) {
  const auto found = state.keypath_slots.find(stable_id);
  if (found == state.keypath_slots.end()) {
    return nullptr;
  }
  return &found->second;
}

inline void RecordRuntimeKeyPathMaterializationUnlocked(
    RuntimeState &state,
    std::uint64_t stable_id,
    std::uint64_t registration_order_ordinal,
    const char *profile) {
  state.last_materialized_keypath_handle = stable_id;
  state.last_materialized_keypath_registration_order_ordinal =
      registration_order_ordinal;
  state.last_materialized_keypath_profile = profile == nullptr ? "" : profile;
}

inline bool InsertImageBackedRuntimeKeyPathSlotUnlocked(
    RuntimeState &state,
    const EmittedKeyPathDescriptor &descriptor,
    std::uint64_t registration_order_ordinal) {
  KeyPathSlot slot;
  slot.stable_id = descriptor.stable_id;
  slot.root_name_storage = descriptor.root_name;
  slot.component_path_storage = descriptor.component_path;
  slot.profile_storage = descriptor.profile;
  slot.generic_metadata_replay_key_storage =
      RuntimeKeyPathGenericMetadataReplayKey(descriptor);
  slot.root_is_self = descriptor.root_is_self;
  slot.component_count =
      CountRuntimeKeyPathComponents(descriptor.component_path);
  slot.metadata_provider_count = 1;
  slot.first_registration_order_ordinal = registration_order_ordinal;
  slot.last_registration_order_ordinal = registration_order_ordinal;
  if (slot.component_count == 0) {
    return false;
  }

  state.keypath_slots.emplace(descriptor.stable_id, std::move(slot));
  ++state.image_backed_keypath_count;
  RecordRuntimeKeyPathMaterializationUnlocked(
      state, descriptor.stable_id, registration_order_ordinal,
      descriptor.profile);
  return true;
}

inline void MergeImageBackedRuntimeKeyPathSlotUnlocked(
    RuntimeState &state,
    KeyPathSlot &slot,
    const EmittedKeyPathDescriptor &descriptor,
    std::uint64_t registration_order_ordinal) {
  if (!RuntimeKeyPathDescriptorMatchesSlot(slot, descriptor) &&
      !slot.ambiguous) {
    slot.ambiguous = true;
    ++state.ambiguous_keypath_handle_count;
  }

  ++slot.metadata_provider_count;
  slot.last_registration_order_ordinal = registration_order_ordinal;
  RecordRuntimeKeyPathMaterializationUnlocked(
      state, descriptor.stable_id, registration_order_ordinal,
      slot.profile_storage.c_str());
}

}  // namespace objc3c::runtime
