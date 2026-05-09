#pragma once

#include "runtime/blocks/block_runtime_records.h"
#include "runtime/state/runtime_state_records.h"

#include <cstdint>
#include <vector>

namespace objc3c::runtime {

struct RuntimeBlockInvocationPlan {
  RuntimeBlockInvokeFn invoke = nullptr;
  std::vector<std::uint64_t> storage_words;
};

inline RuntimeBlockInvocationPlan BuildRuntimeBlockInvocationPlanUnlocked(
    const RuntimeState &state,
    int block_handle) {
  RuntimeBlockInvocationPlan plan;
  const auto block_it = state.runtime_blocks_by_handle.find(block_handle);
  if (block_it == state.runtime_blocks_by_handle.end() ||
      block_it->second.invoke == nullptr) {
    return plan;
  }

  plan.invoke = block_it->second.invoke;
  plan.storage_words = block_it->second.storage_words;
  return plan;
}

inline bool RuntimeBlockInvocationPlanIsRunnable(
    const RuntimeBlockInvocationPlan &plan) {
  return plan.invoke != nullptr && !plan.storage_words.empty();
}

inline int InvokeRuntimeBlockInvocationPlan(RuntimeBlockInvocationPlan &plan,
                                            int a0,
                                            int a1,
                                            int a2,
                                            int a3) {
  if (!RuntimeBlockInvocationPlanIsRunnable(plan)) {
    return 0;
  }
  return plan.invoke(plan.storage_words.data(), a0, a1, a2, a3);
}

}  // namespace objc3c::runtime
