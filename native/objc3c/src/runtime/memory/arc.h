#pragma once

namespace objc3c::runtime {

struct RuntimeState;

bool RuntimeArcValueIsRetainable(int value);
void RetainRuntimeValueUnlocked(RuntimeState &state, int value);
void ReleaseRuntimeValueUnlocked(RuntimeState &state, int value);

}  // namespace objc3c::runtime
