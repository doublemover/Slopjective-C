#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <utility>
#include <vector>

namespace objc3c::runtime {

using RuntimeBlockInvokeFn = int (*)(void *, int, int, int, int);
using RuntimeBlockCopyHelperFn = void (*)(void *);
using RuntimeBlockDisposeHelperFn = void (*)(void *);

// Private runtime block state. The compiler/runtime contract remains an opaque
// copied storage blob plus a private invoke pointer; no public block-object
// memory layout is promised.
struct RuntimeBlockRecord {
  int block_handle = 0;
  bool has_pointer_capture_storage = false;
  std::size_t storage_size_bytes = 0;
  std::vector<std::uint64_t> storage_words;
  std::vector<std::unique_ptr<int>> promoted_capture_cells;
  RuntimeBlockInvokeFn invoke = nullptr;
  RuntimeBlockCopyHelperFn copy_helper = nullptr;
  RuntimeBlockDisposeHelperFn dispose_helper = nullptr;
  std::uint64_t retain_count = 1;
};

constexpr std::size_t kRuntimeBlockPointerCaptureHeaderSlotCount = 3u;

inline unsigned char *RuntimeBlockStorageBytes(RuntimeBlockRecord &record) {
  return reinterpret_cast<unsigned char *>(record.storage_words.data());
}

[[maybe_unused]] inline const unsigned char *RuntimeBlockStorageBytes(
    const RuntimeBlockRecord &record) {
  return reinterpret_cast<const unsigned char *>(record.storage_words.data());
}

inline std::size_t RuntimeBlockPointerCaptureSlotCount(
    const RuntimeBlockRecord &record) {
  if (!record.has_pointer_capture_storage) {
    return 0u;
  }
  const std::size_t header_size =
      sizeof(void *) * kRuntimeBlockPointerCaptureHeaderSlotCount;
  if (record.storage_size_bytes < header_size) {
    return 0u;
  }
  const std::size_t payload_size = record.storage_size_bytes - header_size;
  return payload_size / sizeof(void *);
}

inline bool PromotePointerCaptureCellsIntoRuntimeOwnedStorage(
    RuntimeBlockRecord &record) {
  // Escaping pointer-capture blocks must own the captured cell storage instead
  // of borrowing stack-cell addresses after promotion.
  if (!record.has_pointer_capture_storage) {
    return true;
  }
  const std::size_t header_size =
      sizeof(void *) * kRuntimeBlockPointerCaptureHeaderSlotCount;
  if (record.storage_size_bytes < header_size) {
    return false;
  }
  const std::size_t payload_size = record.storage_size_bytes - header_size;
  if (payload_size % sizeof(void *) != 0u) {
    return false;
  }
  const std::size_t capture_slot_count =
      RuntimeBlockPointerCaptureSlotCount(record);
  unsigned char *const storage_bytes = RuntimeBlockStorageBytes(record);
  record.promoted_capture_cells.clear();
  record.promoted_capture_cells.reserve(capture_slot_count);
  for (std::size_t slot_index = 0; slot_index < capture_slot_count;
       ++slot_index) {
    void *capture_cell = nullptr;
    std::memcpy(&capture_cell,
                storage_bytes + header_size + slot_index * sizeof(void *),
                sizeof(capture_cell));
    if (capture_cell == nullptr) {
      record.promoted_capture_cells.push_back(nullptr);
      continue;
    }
    auto promoted_cell = std::make_unique<int>(0);
    std::memcpy(promoted_cell.get(), capture_cell, sizeof(int));
    void *promoted_cell_ptr = promoted_cell.get();
    std::memcpy(storage_bytes + header_size + slot_index * sizeof(void *),
                &promoted_cell_ptr, sizeof(promoted_cell_ptr));
    record.promoted_capture_cells.push_back(std::move(promoted_cell));
  }
  return true;
}

}  // namespace objc3c::runtime
