#include "runtime/blocks/block_promotion.h"

#include "runtime/blocks/block_capture_storage.h"
#include "runtime/blocks/block_pointer_capture_storage.h"
#include "runtime/blocks/block_record.h"
#include "runtime/blocks/block_runtime_records.h"
#include "runtime/blocks/block_runtime_state.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <utility>

namespace objc3c::runtime {

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
  int block_handle = state.next_runtime_block_handle;
  while (block_handle <= 0 ||
         state.runtime_instances_by_receiver.find(block_handle) !=
             state.runtime_instances_by_receiver.end() ||
         state.runtime_blocks_by_handle.find(block_handle) !=
             state.runtime_blocks_by_handle.end()) {
    ++block_handle;
  }
  state.next_runtime_block_handle = block_handle + 1;
  RuntimeBlockRecord record;
  record.block_handle = block_handle;
  record.has_pointer_capture_storage =
      RuntimeBlockCaptureStorageNeedsCopyDispose(has_pointer_capture_storage);
  record.storage_size_bytes =
      static_cast<std::size_t>(std::max<std::uint64_t>(storage_size_bytes, 1u));
  const std::size_t storage_word_count =
      (record.storage_size_bytes + sizeof(std::uint64_t) - 1u) /
      sizeof(std::uint64_t);
  record.storage_words.assign(storage_word_count, 0u);
  std::memcpy(record.storage_words.data(), storage, record.storage_size_bytes);
  std::memcpy(&record.invoke, record.storage_words.data(),
              sizeof(record.invoke));
  if (record.has_pointer_capture_storage) {
    if (record.storage_size_bytes < sizeof(void *) * 3u) {
      return 0;
    }
    std::memcpy(
        &record.copy_helper,
        reinterpret_cast<const unsigned char *>(record.storage_words.data()) +
            sizeof(void *),
        sizeof(record.copy_helper));
    std::memcpy(
        &record.dispose_helper,
        reinterpret_cast<const unsigned char *>(record.storage_words.data()) +
            sizeof(void *) * 2u,
        sizeof(record.dispose_helper));
    if (!PromotePointerCaptureCellsIntoRuntimeOwnedStorage(record)) {
      return 0;
    }
    if (record.copy_helper != nullptr) {
      record.copy_helper(record.storage_words.data());
    }
  }
  if (record.invoke == nullptr) {
    return 0;
  }
  state.runtime_blocks_by_handle.emplace(block_handle, std::move(record));
  debug_state.last_promoted_block_handle = block_handle;
  return block_handle;
}

}  // namespace objc3c::runtime
