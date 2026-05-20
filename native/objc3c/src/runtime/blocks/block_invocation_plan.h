#pragma once

#include "runtime/blocks/block_runtime_records.h"
#include "runtime/state/runtime_state_records.h"

#include <cstdint>
#include <vector>

namespace objc3c::runtime {

struct RuntimeBlockInvocationPlan {
  const RuntimeBlockDescriptor *descriptor = nullptr;
  RuntimeBlockInvokeFn invoke = nullptr;
  std::vector<std::uint64_t> storage_words;
};

RuntimeBlockInvocationPlan BuildRuntimeBlockInvocationPlanUnlocked(
    const RuntimeState &state,
    int block_handle);
bool RuntimeBlockInvocationPlanIsRunnable(
    const RuntimeBlockInvocationPlan &plan);
int InvokeRuntimeBlockInvocationPlan(RuntimeBlockInvocationPlan &plan,
                                     int a0,
                                     int a1,
                                     int a2,
                                     int a3);

}  // namespace objc3c::runtime
