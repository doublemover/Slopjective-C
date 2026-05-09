#include "runtime/blocks/block_handle_allocation.h"

namespace objc3c::runtime {

bool RuntimeBlockHandleIsAvailableUnlocked(const RuntimeState &state,
                                           int block_handle) {
  return block_handle > 0 &&
         state.runtime_instances_by_receiver.find(block_handle) ==
             state.runtime_instances_by_receiver.end() &&
         state.runtime_blocks_by_handle.find(block_handle) ==
             state.runtime_blocks_by_handle.end();
}

int AllocateRuntimeBlockHandleUnlocked(RuntimeState &state) {
  int block_handle = state.next_runtime_block_handle;
  while (!RuntimeBlockHandleIsAvailableUnlocked(state, block_handle)) {
    ++block_handle;
  }
  state.next_runtime_block_handle = block_handle + 1;
  return block_handle;
}

}  // namespace objc3c::runtime
