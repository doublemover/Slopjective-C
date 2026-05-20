#include "runtime/blocks/block_pointer_capture_storage.h"

#include "runtime/blocks/block_pointer_capture_cells.h"
#include "runtime/blocks/block_pointer_capture_layout.h"

namespace objc3c::runtime {

unsigned char *RuntimeBlockStorageBytes(RuntimeBlockRecord &record) {
  return reinterpret_cast<unsigned char *>(record.storage_words.data());
}

const unsigned char *RuntimeBlockStorageBytes(const RuntimeBlockRecord &record) {
  return reinterpret_cast<const unsigned char *>(record.storage_words.data());
}

std::size_t RuntimeBlockPointerCaptureSlotCount(
    const RuntimeBlockRecord &record) {
  if (!record.has_pointer_capture_storage) {
    return 0u;
  }
  return RuntimeBlockPointerCaptureSlotCountFromLayout(record);
}

bool PromotePointerCaptureCellsIntoRuntimeOwnedStorage(
    RuntimeState &state,
    RuntimeBlockRecord &record) {
  // Escaping pointer-capture blocks must own the captured cell storage instead
  // of borrowing stack-cell addresses after promotion.
  if (!record.has_pointer_capture_storage) {
    return true;
  }
  if (!RuntimeBlockPointerCaptureHeaderIsComplete(record)) {
    return false;
  }
  if (!RuntimeBlockPointerCapturePayloadIsAligned(record)) {
    return false;
  }
  const std::size_t capture_slot_count =
      RuntimeBlockPointerCaptureSlotCount(record);
  record.promoted_capture_cells.clear();
  record.promoted_capture_cells.reserve(capture_slot_count);
  for (std::size_t slot_index = 0; slot_index < capture_slot_count;
       ++slot_index) {
    if (!PromoteRuntimeBlockPointerCaptureCell(state, record, slot_index)) {
      return false;
    }
  }
  return true;
}

}  // namespace objc3c::runtime
