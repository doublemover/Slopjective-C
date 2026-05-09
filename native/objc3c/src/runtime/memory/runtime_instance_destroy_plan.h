#pragma once

#include <vector>

namespace objc3c::runtime {

struct RuntimeInstanceRecord;
struct RuntimeState;

std::vector<int> RuntimeInstanceOwnedValuesToReleaseUnlocked(
    RuntimeState &state,
    const RuntimeInstanceRecord &instance);

}  // namespace objc3c::runtime
