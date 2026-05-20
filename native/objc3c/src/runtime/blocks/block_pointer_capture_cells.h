#pragma once

#include "runtime/blocks/block_byref_cells.h"
#include "runtime/blocks/block_pointer_capture_layout.h"

#include <cstring>
#include <utility>

namespace objc3c::runtime {

inline void *LoadRuntimeBlockPointerCaptureCellAddress(
    RuntimeBlockRecord &record,
    std::size_t slot_index) {
  void *capture_cell = nullptr;
  std::memcpy(&capture_cell,
              RuntimeBlockPointerCaptureSlotAddress(record, slot_index),
              sizeof(capture_cell));
  return capture_cell;
}

inline void StoreRuntimeBlockPointerCaptureCellAddress(
    RuntimeBlockRecord &record,
    std::size_t slot_index,
    void *capture_cell) {
  std::memcpy(RuntimeBlockPointerCaptureSlotAddress(record, slot_index),
              &capture_cell, sizeof(capture_cell));
}

inline bool PromoteRuntimeBlockPointerCaptureCell(RuntimeState &state,
                                                  RuntimeBlockRecord &record,
                                                  std::size_t slot_index) {
  void *capture_cell =
      LoadRuntimeBlockPointerCaptureCellAddress(record, slot_index);
  if (capture_cell == nullptr) {
    record.promoted_capture_cells.push_back(nullptr);
    return true;
  }

  std::shared_ptr<RuntimeBlockByrefCell> promoted_cell;
  if (record.has_byref_forwarding_cells) {
    promoted_cell = PromoteRuntimeBlockByrefCell(
        state, static_cast<RuntimeBlockByrefCell *>(capture_cell));
    if (!promoted_cell) {
      return false;
    }
  } else {
    promoted_cell = PromoteRuntimeBlockRawPointerCell(capture_cell);
    if (!promoted_cell) {
      return false;
    }
  }
  StoreRuntimeBlockPointerCaptureCellAddress(record, slot_index,
                                             promoted_cell.get());
  record.promoted_capture_cells.push_back(std::move(promoted_cell));
  return true;
}

}  // namespace objc3c::runtime
