#include "runtime/storage/weak_slots.h"

#include <algorithm>

namespace objc3c::runtime {

bool RuntimeWeakSlotTargetIsTrackable(int receiver) {
  return receiver != 0;
}

void RemoveWeakSlotRefUnlocked(RuntimeState &state, int target_receiver,
                               int owner_receiver, std::size_t offset,
                               std::size_t size) {
  if (!RuntimeWeakSlotTargetIsTrackable(target_receiver)) {
    return;
  }
  const auto found =
      state.weak_slot_refs_by_target_receiver.find(target_receiver);
  if (found == state.weak_slot_refs_by_target_receiver.end()) {
    return;
  }
  auto &refs = found->second;
  refs.erase(
      std::remove_if(
          refs.begin(), refs.end(),
          [&](const RuntimeWeakSlotRef &ref) {
            return ref.owner_receiver == owner_receiver &&
                   ref.offset == offset && ref.size == size;
          }),
      refs.end());
  if (refs.empty()) {
    state.weak_slot_refs_by_target_receiver.erase(found);
  }
}

void RegisterWeakSlotRefUnlocked(RuntimeState &state, int target_receiver,
                                 int owner_receiver, std::size_t offset,
                                 std::size_t size) {
  if (!RuntimeWeakSlotTargetIsTrackable(target_receiver)) {
    return;
  }
  std::vector<RuntimeWeakSlotRef> &refs =
      state.weak_slot_refs_by_target_receiver[target_receiver];
  const auto duplicate =
      std::find_if(refs.begin(), refs.end(), [&](const RuntimeWeakSlotRef &ref) {
        return ref.owner_receiver == owner_receiver && ref.offset == offset &&
               ref.size == size;
      });
  if (duplicate == refs.end()) {
    refs.push_back(RuntimeWeakSlotRef{owner_receiver, offset, size});
  }
}

void RemoveWeakSlotRefsOwnedByReceiverUnlocked(RuntimeState &state,
                                               int owner_receiver) {
  for (auto it = state.weak_slot_refs_by_target_receiver.begin();
       it != state.weak_slot_refs_by_target_receiver.end();) {
    auto &refs = it->second;
    refs.erase(std::remove_if(refs.begin(), refs.end(),
                              [&](const RuntimeWeakSlotRef &ref) {
                                return ref.owner_receiver == owner_receiver;
                              }),
               refs.end());
    if (refs.empty()) {
      it = state.weak_slot_refs_by_target_receiver.erase(it);
    } else {
      ++it;
    }
  }
}

void ZeroWeakSlotRefsForTargetUnlocked(RuntimeState &state, int target_receiver) {
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
    if (ref.size == 0u || ref.offset + ref.size > owner.storage_bytes.size()) {
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
