#pragma once

#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_thread_records.h"

#include <cstdint>
#include <vector>

namespace objc3c::runtime {

RuntimeDispatchFrame *CurrentRuntimeDispatchFrame();
void PushRuntimeDispatchFrame(int receiver, std::uint64_t base_identity,
                              const RealizedPropertyAccessor *accessor);
std::vector<int> PopRuntimeDispatchFrameAutoreleaseValues();
RuntimeDispatchFrame *SetRuntimeTestingDispatchFrame(
    int receiver, std::uint64_t base_identity,
    const RealizedPropertyAccessor *accessor);
void ClearRuntimeTestingDispatchFrame();
void ResetRuntimeDispatchFrameStateForTesting();
bool EnqueueRuntimeDispatchFrameAutoreleaseValue(int value);

}  // namespace objc3c::runtime
