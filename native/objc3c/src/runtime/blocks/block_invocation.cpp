#include "runtime/blocks/block_invocation.h"

#include "runtime/blocks/block_invocation_plan.h"
#include "runtime/blocks/block_runtime_records.h"
#include "runtime/blocks/block_runtime_state.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"

#include <mutex>

namespace objc3c::runtime {

int InvokeRuntimeBlockI32(int block_handle, int a0, int a1, int a2, int a3) {
  // block-runtime allocation/copy-dispose/invoke anchor: invoke consumes a
  // promoted runtime block record with a concrete thunk and copied storage.
  RuntimeBlockDebugState &debug_state =
      RuntimeBlockDebugStateForCurrentThread();
  ++debug_state.invoke_call_count;
  debug_state.last_invoked_block_handle = block_handle;
  RuntimeBlockInvocationPlan plan;
  {
    RuntimeState &state = ProcessRuntimeState();
    std::lock_guard<std::mutex> lock(state.mutex);
    plan = BuildRuntimeBlockInvocationPlanUnlocked(state, block_handle);
  }
  debug_state.last_invoke_plan_storage_word_count = plan.storage_words.size();
  debug_state.last_invoke_plan_has_descriptor =
      plan.descriptor != nullptr ? 1 : 0;
  debug_state.last_invoke_plan_has_invoke = plan.invoke != nullptr ? 1 : 0;
  const bool runnable = RuntimeBlockInvocationPlanIsRunnable(plan);
  debug_state.last_invoke_plan_was_runnable = runnable ? 1 : 0;
  if (!runnable) {
    return 0;
  }
  const int result = InvokeRuntimeBlockInvocationPlan(plan, a0, a1, a2, a3);
  debug_state.last_block_invoke_result = result;
  return result;
}

}  // namespace objc3c::runtime
