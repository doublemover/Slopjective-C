#pragma once

#include "runtime/state/runtime_state_records.h"

namespace objc3c::runtime {

bool RuntimeBlockHandleIsAvailableUnlocked(const RuntimeState &state,
                                           int block_handle);
int AllocateRuntimeBlockHandleUnlocked(RuntimeState &state);

}  // namespace objc3c::runtime
