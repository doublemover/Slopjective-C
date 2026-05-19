#pragma once

#include <cstdint>

namespace objc3c::runtime {

struct RuntimeState;

int AllocateRuntimeInstanceUnlocked(RuntimeState &state,
                                    std::uint64_t base_identity);
void DestroyRuntimeInstanceUnlocked(RuntimeState &state, int receiver);

}  // namespace objc3c::runtime
