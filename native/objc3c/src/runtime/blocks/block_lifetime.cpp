#include "runtime/blocks/block_lifetime.h"

#include "runtime/state/runtime_state_records.h"

namespace objc3c::runtime {

bool RetainRuntimeBlockHandleUnlocked(RuntimeState &state, int block_handle) {
  const auto block_it = state.runtime_blocks_by_handle.find(block_handle);
  if (block_it == state.runtime_blocks_by_handle.end()) {
    return false;
  }
  ++block_it->second.retain_count;
  return true;
}

bool ReleaseRuntimeBlockHandleUnlocked(RuntimeState &state, int block_handle) {
  const auto block_it = state.runtime_blocks_by_handle.find(block_handle);
  if (block_it == state.runtime_blocks_by_handle.end()) {
    return false;
  }
  if (block_it->second.retain_count > 1u) {
    --block_it->second.retain_count;
    return true;
  }
  if (block_it->second.dispose_helper != nullptr &&
      !block_it->second.storage_words.empty()) {
    block_it->second.dispose_helper(block_it->second.storage_words.data());
  }
  state.runtime_blocks_by_handle.erase(block_it);
  return true;
}

}  // namespace objc3c::runtime
