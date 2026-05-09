#pragma once

#include "runtime/state/runtime_state_records.h"

namespace objc3c::runtime {

void ClearMethodCacheStateUnlocked(RuntimeState &state);
void ClearRealizedClassGraphUnlocked(RuntimeState &state);
void ClearRuntimeInstanceStateUnlocked(RuntimeState &state);

}  // namespace objc3c::runtime
