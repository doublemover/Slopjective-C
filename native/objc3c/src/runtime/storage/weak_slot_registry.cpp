#include "runtime/storage/weak_slots.h"

#include "runtime/storage/weak_slot_ref_match.h"

#include <algorithm>
#include <vector>

namespace objc3c::runtime {

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
            return RuntimeWeakSlotRefMatchesStorage(ref, owner_receiver,
                                                    offset, size);
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
        return RuntimeWeakSlotRefMatchesStorage(ref, owner_receiver, offset,
                                                size);
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
                                return RuntimeWeakSlotRefIsOwnedByReceiver(
                                    ref, owner_receiver);
                              }),
               refs.end());
    if (refs.empty()) {
      it = state.weak_slot_refs_by_target_receiver.erase(it);
    } else {
      ++it;
    }
  }
}

}  // namespace objc3c::runtime
