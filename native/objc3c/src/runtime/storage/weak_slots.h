#pragma once

#include "runtime/state/runtime_state_records.h"

#include <cstddef>

namespace objc3c::runtime {

bool RuntimeWeakSlotTargetIsTrackable(int receiver);
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
