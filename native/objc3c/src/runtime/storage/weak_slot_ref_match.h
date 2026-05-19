#pragma once

#include "runtime/state/runtime_state_records.h"

#include <cstddef>

namespace objc3c::runtime {

struct RuntimeInstanceRecord;

bool RuntimeWeakSlotRefMatchesStorage(const RuntimeWeakSlotRef &ref,
                                      int owner_receiver,
                                      std::size_t offset,
                                      std::size_t size);
bool RuntimeWeakSlotRefIsOwnedByReceiver(const RuntimeWeakSlotRef &ref,
                                         int owner_receiver);
bool RuntimeWeakSlotRefIsAddressableInOwner(const RuntimeWeakSlotRef &ref,
                                            const RuntimeInstanceRecord &owner);

}  // namespace objc3c::runtime
