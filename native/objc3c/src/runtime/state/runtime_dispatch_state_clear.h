#pragma once

namespace objc3c::runtime {

struct RuntimeState;

void ClearMethodCacheStateUnlocked(RuntimeState &state);

}  // namespace objc3c::runtime
