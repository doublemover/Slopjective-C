#pragma once

namespace objc3c::runtime {

struct RuntimeState;

void DestroyRuntimeInstanceUnlocked(RuntimeState &state, int receiver);

}  // namespace objc3c::runtime
