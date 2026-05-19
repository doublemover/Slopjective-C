#pragma once

#include "runtime/blocks/block_pointer_capture_storage.h"

#include <cstddef>

namespace objc3c::runtime {

inline std::size_t RuntimeBlockPointerCaptureHeaderSize() {
  return sizeof(void *) * kRuntimeBlockPointerCaptureHeaderSlotCount;
}

inline bool RuntimeBlockPointerCaptureHeaderIsComplete(
    const RuntimeBlockRecord &record) {
  return record.has_pointer_capture_storage &&
         record.storage_size_bytes >= RuntimeBlockPointerCaptureHeaderSize();
}

inline std::size_t RuntimeBlockPointerCapturePayloadSize(
    const RuntimeBlockRecord &record) {
  if (!RuntimeBlockPointerCaptureHeaderIsComplete(record)) {
    return 0u;
  }
  return record.storage_size_bytes - RuntimeBlockPointerCaptureHeaderSize();
}

inline bool RuntimeBlockPointerCapturePayloadIsAligned(
    const RuntimeBlockRecord &record) {
  return RuntimeBlockPointerCapturePayloadSize(record) % sizeof(void *) == 0u;
}

inline std::size_t RuntimeBlockPointerCaptureSlotCountFromLayout(
    const RuntimeBlockRecord &record) {
  if (!RuntimeBlockPointerCaptureHeaderIsComplete(record)) {
    return 0u;
  }
  return RuntimeBlockPointerCapturePayloadSize(record) / sizeof(void *);
}

inline unsigned char *RuntimeBlockPointerCaptureSlotAddress(
    RuntimeBlockRecord &record,
    std::size_t slot_index) {
  return RuntimeBlockStorageBytes(record) + RuntimeBlockPointerCaptureHeaderSize() +
         slot_index * sizeof(void *);
}

}  // namespace objc3c::runtime
