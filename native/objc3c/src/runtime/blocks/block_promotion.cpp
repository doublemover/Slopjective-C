#include "runtime/blocks/block_promotion.h"

#include "runtime/blocks/block_handle_allocation.h"
#include "runtime/blocks/block_record.h"
#include "runtime/blocks/block_promotion_plan.h"
#include "runtime/blocks/block_runtime_records.h"
#include "runtime/blocks/block_runtime_state.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <utility>

namespace objc3c::runtime {
namespace {

void CaptureRuntimeBlockDescriptorDebugFields(
    RuntimeBlockDebugState &debug_state,
    const RuntimeBlockRecord &record) {
  debug_state.last_descriptor_address =
      reinterpret_cast<std::uintptr_t>(record.descriptor);
  debug_state.last_descriptor_invoke_address =
      reinterpret_cast<std::uintptr_t>(record.invoke);
  debug_state.last_descriptor_storage_size_bytes =
      record.descriptor_storage_size_bytes;
  debug_state.last_descriptor_capture_count = record.descriptor_capture_count;
  debug_state.last_descriptor_storage_word_count = record.storage_words.size();
  debug_state.last_descriptor_flags = record.descriptor_flags;
  debug_state.last_descriptor_parameter_count =
      static_cast<int>(record.descriptor_parameter_count);
  debug_state.last_descriptor_has_invoke = record.invoke != nullptr ? 1 : 0;
  debug_state.last_descriptor_has_copy_helper =
      record.copy_helper != nullptr ? 1 : 0;
  debug_state.last_descriptor_has_dispose_helper =
      record.dispose_helper != nullptr ? 1 : 0;
  debug_state.last_descriptor_has_pointer_capture_storage =
      record.has_pointer_capture_storage ? 1 : 0;
}

}  // namespace

int PromoteRuntimeBlockI32(const void *storage,
                           std::uint64_t storage_size_bytes,
                           int has_pointer_capture_storage) {
  // block-runtime allocation/copy-dispose/invoke anchor: promotion admits
  // pointer-capture storage, preserves helper pointers, and publishes one
  // runtime-owned block record without exposing a public block layout.
  if (storage == nullptr ||
      !RuntimeBlockStorageSizeIsSupported(
          static_cast<std::size_t>(storage_size_bytes))) {
    return 0;
  }
  RuntimeBlockDebugState &debug_state =
      RuntimeBlockDebugStateForCurrentThread();
  ++debug_state.promote_call_count;
  debug_state.last_promote_has_pointer_capture_storage =
      has_pointer_capture_storage != 0 ? 1 : 0;
  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  const int block_handle = AllocateRuntimeBlockHandleUnlocked(state);
  RuntimeBlockRecord record;
  if (!BuildRuntimeBlockRecord(block_handle, storage, storage_size_bytes,
                               has_pointer_capture_storage, &record)) {
    return 0;
  }
  CaptureRuntimeBlockDescriptorDebugFields(debug_state, record);
  state.runtime_blocks_by_handle.emplace(block_handle, std::move(record));
  debug_state.last_promoted_block_handle = block_handle;
  return block_handle;
}

}  // namespace objc3c::runtime
