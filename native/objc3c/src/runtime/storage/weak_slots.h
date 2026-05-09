#pragma once

#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/weak_slot_target.h"

#include <cstddef>

namespace objc3c::runtime {

void RemoveWeakSlotRefUnlocked(RuntimeState &state, int target_receiver,
                               int owner_receiver, std::size_t offset,
                               std::size_t size);
void RegisterWeakSlotRefUnlocked(RuntimeState &state, int target_receiver,
                                 int owner_receiver, std::size_t offset,
                                 std::size_t size);
void RemoveWeakSlotRefsOwnedByReceiverUnlocked(RuntimeState &state,
                                               int owner_receiver);
void ZeroWeakSlotRefsForTargetUnlocked(RuntimeState &state, int target_receiver);

}  // namespace objc3c::runtime
