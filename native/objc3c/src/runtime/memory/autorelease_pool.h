#pragma once

#include "runtime/memory/dispatch_frame_state.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_thread_records.h"

#include <cstdint>
#include <vector>

namespace objc3c::runtime {

bool RuntimeAutoreleasePoolCanEnqueue(int value);
void ResetRuntimeAutoreleasepoolStateForTesting();
void PushRuntimeAutoreleasePoolFrame();
std::vector<int> PopRuntimeAutoreleasePoolFrameValues();
std::uint64_t RuntimeAutoreleasePoolDepth();
std::uint64_t RuntimeAutoreleasePoolMaxDepth();
std::uint64_t CountQueuedAutoreleaseValues();
std::uint64_t RuntimeAutoreleasePoolDrainedValueCount();
void RecordRuntimeAutoreleasePoolDrainedValue(int value);
int RuntimeLastAutoreleasedValue();
int RuntimeLastDrainedAutoreleaseValue();
void EnqueueAutoreleaseValue(int value);

}  // namespace objc3c::runtime
