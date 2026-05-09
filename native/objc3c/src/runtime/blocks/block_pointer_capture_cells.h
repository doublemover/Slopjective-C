#pragma once

#include "runtime/blocks/block_pointer_capture_layout.h"

#include <cstring>
#include <memory>
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

inline void PromoteRuntimeBlockPointerCaptureCell(RuntimeBlockRecord &record,
                                                  std::size_t slot_index) {
  void *capture_cell =
      LoadRuntimeBlockPointerCaptureCellAddress(record, slot_index);
  if (capture_cell == nullptr) {
    record.promoted_capture_cells.push_back(nullptr);
    return;
  }

  auto promoted_cell = std::make_unique<int>(0);
  std::memcpy(promoted_cell.get(), capture_cell, sizeof(int));
  StoreRuntimeBlockPointerCaptureCellAddress(record, slot_index,
                                             promoted_cell.get());
  record.promoted_capture_cells.push_back(std::move(promoted_cell));
}

}  // namespace objc3c::runtime
