#include "runtime/storage/weak_slots.h"

#include "runtime/storage/runtime_instance_records.h"
#include "runtime/storage/weak_slot_ref_match.h"

#include <algorithm>

namespace objc3c::runtime {

void ZeroWeakSlotRefsForTargetUnlocked(RuntimeState &state,
                                       int target_receiver) {
  if (!RuntimeWeakSlotTargetIsTrackable(target_receiver)) {
    return;
  }
  const auto found =
      state.weak_slot_refs_by_target_receiver.find(target_receiver);
  if (found == state.weak_slot_refs_by_target_receiver.end()) {
    return;
  }
  for (const RuntimeWeakSlotRef &ref : found->second) {
    const auto owner_it =
        state.runtime_instances_by_receiver.find(ref.owner_receiver);
    if (owner_it == state.runtime_instances_by_receiver.end()) {
      continue;
    }
    RuntimeInstanceRecord &owner = owner_it->second;
    if (!RuntimeWeakSlotRefIsAddressableInOwner(ref, owner)) {
      continue;
    }
    std::fill(owner.storage_bytes.begin() +
                  static_cast<std::ptrdiff_t>(ref.offset),
              owner.storage_bytes.begin() +
                  static_cast<std::ptrdiff_t>(ref.offset + ref.size),
              0);
  }
  state.weak_slot_refs_by_target_receiver.erase(found);
}

}  // namespace objc3c::runtime
