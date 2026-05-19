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
  snapshot->last_descriptor_address = state.last_descriptor_address;
  snapshot->last_descriptor_invoke_address =
      state.last_descriptor_invoke_address;
  snapshot->last_descriptor_storage_size_bytes =
      state.last_descriptor_storage_size_bytes;
  snapshot->last_descriptor_capture_count = state.last_descriptor_capture_count;
  snapshot->last_descriptor_storage_word_count =
      state.last_descriptor_storage_word_count;
  snapshot->last_descriptor_flags = state.last_descriptor_flags;
  snapshot->last_invoke_plan_storage_word_count =
      state.last_invoke_plan_storage_word_count;
  snapshot->last_promoted_block_handle = state.last_promoted_block_handle;
  snapshot->last_promote_has_pointer_capture_storage =
      state.last_promote_has_pointer_capture_storage;
  snapshot->last_descriptor_parameter_count =
      state.last_descriptor_parameter_count;
  snapshot->last_descriptor_has_invoke = state.last_descriptor_has_invoke;
  snapshot->last_descriptor_has_copy_helper =
      state.last_descriptor_has_copy_helper;
  snapshot->last_descriptor_has_dispose_helper =
      state.last_descriptor_has_dispose_helper;
  snapshot->last_descriptor_has_pointer_capture_storage =
      state.last_descriptor_has_pointer_capture_storage;
  snapshot->last_invoke_plan_has_descriptor =
      state.last_invoke_plan_has_descriptor;
  snapshot->last_invoke_plan_has_invoke = state.last_invoke_plan_has_invoke;
  snapshot->last_invoke_plan_was_runnable =
      state.last_invoke_plan_was_runnable;
  snapshot->last_invoked_block_handle = state.last_invoked_block_handle;
  snapshot->last_block_invoke_result = state.last_block_invoke_result;
}

}  // namespace objc3c::runtime
