#pragma once

#include "runtime/selectors/keypath_records.h"

#include <cstdint>

namespace objc3c::runtime {

inline const KeyPathSlot *ResolveRuntimeKeyPathQueryUnlocked(
    RuntimeState &state,
    std::uint64_t stable_id) {
  state.last_queried_keypath_handle = stable_id;
  state.last_keypath_query_found = false;
  state.last_keypath_query_ambiguous = false;
  state.last_resolved_keypath_profile.clear();

  const KeyPathSlot *slot = FindRuntimeKeyPathSlotUnlocked(state, stable_id);
  if (slot == nullptr) {
    return nullptr;
  }

  state.last_keypath_query_found = true;
  state.last_keypath_query_ambiguous = slot->ambiguous;
  state.last_resolved_keypath_profile = slot->profile_storage;
  return slot;
}

inline bool RuntimeKeyPathQueryCanUseSlot(const KeyPathSlot *slot) {
  return slot != nullptr && !slot->ambiguous;
}

}  // namespace objc3c::runtime
