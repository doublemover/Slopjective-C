#include "runtime/blocks/block_invocation.h"

#include "runtime/blocks/block_runtime_records.h"
#include "runtime/blocks/block_runtime_state.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"

#include <cstdint>
#include <mutex>
#include <vector>

namespace objc3c::runtime {

int InvokeRuntimeBlockI32(int block_handle, int a0, int a1, int a2, int a3) {
  // block-runtime allocation/copy-dispose/invoke anchor: invoke consumes a
  // promoted runtime block record with a concrete thunk and copied storage.
  RuntimeBlockDebugState &debug_state =
      RuntimeBlockDebugStateForCurrentThread();
  ++debug_state.invoke_call_count;
  debug_state.last_invoked_block_handle = block_handle;
  RuntimeBlockInvokeFn invoke = nullptr;
  std::vector<std::uint64_t> storage_words;
  {
    RuntimeState &state = ProcessRuntimeState();
    std::lock_guard<std::mutex> lock(state.mutex);
    const auto block_it = state.runtime_blocks_by_handle.find(block_handle);
    if (block_it == state.runtime_blocks_by_handle.end() ||
        block_it->second.invoke == nullptr) {
      return 0;
    }
    invoke = block_it->second.invoke;
    storage_words = block_it->second.storage_words;
  }
  if (invoke == nullptr || storage_words.empty()) {
    return 0;
  }
  const int result = invoke(storage_words.data(), a0, a1, a2, a3);
  debug_state.last_block_invoke_result = result;
  return result;
}

}  // namespace objc3c::runtime
