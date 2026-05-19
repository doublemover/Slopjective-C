#pragma once

#include "runtime/blocks/block_runtime_records.h"

#include <cstdint>
#include <vector>

namespace objc3c::runtime {

struct RuntimeState;

int AllocateRuntimeInstanceUnlocked(RuntimeState &state,
                                    std::uint64_t base_identity);
void DestroyRuntimeInstanceUnlocked(
    RuntimeState &state, int receiver,
    std::vector<RuntimeBlockRecord> *records_to_dispose = nullptr);

}  // namespace objc3c::runtime
