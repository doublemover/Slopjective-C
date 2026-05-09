#pragma once

#include "runtime/blocks/block_runtime_state.h"

namespace objc3c::runtime {

inline void ResetRuntimeBlockDebugState(RuntimeBlockDebugState &state) {
  state = RuntimeBlockDebugState{};
}

inline void CopyRuntimeBlockDebugStateToBlockArcSnapshot(
    const RuntimeBlockDebugState &state,
    objc3_runtime_block_arc_runtime_abi_snapshot *snapshot) {
  snapshot->block_promote_call_count = state.promote_call_count;
  snapshot->block_invoke_call_count = state.invoke_call_count;
  snapshot->last_promoted_block_handle = state.last_promoted_block_handle;
  snapshot->last_promote_has_pointer_capture_storage =
      state.last_promote_has_pointer_capture_storage;
  snapshot->last_invoked_block_handle = state.last_invoked_block_handle;
  snapshot->last_block_invoke_result = state.last_block_invoke_result;
}

}  // namespace objc3c::runtime
