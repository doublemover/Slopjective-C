#pragma once

namespace objc3c::runtime {

struct RuntimeState;

void SeedDispatchIntentFastPathCacheUnlocked(RuntimeState &state);

}  // namespace objc3c::runtime
