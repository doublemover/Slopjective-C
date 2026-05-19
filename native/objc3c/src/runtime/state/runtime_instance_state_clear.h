#pragma once

namespace objc3c::runtime {

struct RuntimeState;

void ClearRuntimeInstanceStateUnlocked(RuntimeState &state);

}  // namespace objc3c::runtime
