#pragma once

namespace objc3c::runtime {

struct RuntimeState;

void ClearLiveRegistrationStateUnlocked(RuntimeState &state);

}  // namespace objc3c::runtime
