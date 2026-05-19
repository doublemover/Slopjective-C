#pragma once

#include "runtime/blocks/block_runtime_records.h"

#include <vector>

namespace objc3c::runtime {

struct RuntimeState;

bool RuntimeArcValueIsRetainable(int value);
void RetainRuntimeValueUnlocked(RuntimeState &state, int value);
void ReleaseRuntimeValueUnlocked(
    RuntimeState &state, int value,
    std::vector<RuntimeBlockRecord> *records_to_dispose = nullptr);

}  // namespace objc3c::runtime
