#pragma once

#include "runtime/memory/runtime_ownership_snapshot_contracts.h"

#include <cstdint>

namespace objc3c::runtime {

struct RuntimeBlockDebugState {
  std::uint64_t promote_call_count = 0;
  std::uint64_t invoke_call_count = 0;
  std::uint64_t last_descriptor_address = 0;
  std::uint64_t last_descriptor_invoke_address = 0;
  std::uint64_t last_descriptor_storage_size_bytes = 0;
  std::uint64_t last_descriptor_capture_count = 0;
  std::uint64_t last_descriptor_storage_word_count = 0;
  std::uint64_t last_descriptor_flags = 0;
  std::uint64_t last_invoke_plan_storage_word_count = 0;
  int last_promoted_block_handle = 0;
  int last_promote_has_pointer_capture_storage = 0;
  int last_descriptor_parameter_count = 0;
  int last_descriptor_has_invoke = 0;
  int last_descriptor_has_copy_helper = 0;
  int last_descriptor_has_dispose_helper = 0;
  int last_descriptor_has_pointer_capture_storage = 0;
  int last_invoke_plan_has_descriptor = 0;
  int last_invoke_plan_has_invoke = 0;
  int last_invoke_plan_was_runnable = 0;
  int last_invoked_block_handle = 0;
  int last_block_invoke_result = 0;
};

RuntimeBlockDebugState &RuntimeBlockDebugStateForCurrentThread();
void ResetRuntimeBlockDebugStateForTesting();
void CopyRuntimeBlockFieldsToBlockArcSnapshot(
    objc3_runtime_block_arc_runtime_abi_snapshot *snapshot);

}  // namespace objc3c::runtime
