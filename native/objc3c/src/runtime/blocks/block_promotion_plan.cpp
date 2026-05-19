#include "runtime/blocks/block_promotion_plan.h"

#include "runtime/blocks/block_capture_storage.h"
#include "runtime/blocks/block_descriptor.h"
#include "runtime/blocks/block_pointer_capture_storage.h"
#include "runtime/blocks/block_record.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <utility>

namespace objc3c::runtime {
namespace {

std::size_t RuntimeBlockStorageWordCount(std::size_t storage_size_bytes) {
  return (storage_size_bytes + sizeof(std::uint64_t) - 1u) /
         sizeof(std::uint64_t);
}

bool RuntimeBlockPointerCaptureHeaderIsPresent(
    const RuntimeBlockRecord &record) {
  return record.storage_size_bytes >=
         sizeof(void *) * kRuntimeBlockPointerCaptureHeaderSlotCount;
}

bool RuntimeBlockDescriptorCaptureCountMatches(
    const RuntimeBlockRecord &record,
    const RuntimeBlockDescriptor &descriptor) {
  if (record.has_pointer_capture_storage) {
    const std::size_t header_size =
        sizeof(void *) * kRuntimeBlockPointerCaptureHeaderSlotCount;
    if (record.storage_size_bytes < header_size ||
        (record.storage_size_bytes - header_size) % sizeof(void *) != 0u) {
      return false;
    }
    return descriptor.capture_count ==
           (record.storage_size_bytes - header_size) / sizeof(void *);
  }
  if (record.storage_size_bytes < sizeof(void *) ||
      (record.storage_size_bytes - sizeof(void *)) % sizeof(std::int32_t) !=
          0u) {
    return false;
  }
  const std::uint64_t required_payload_size =
      descriptor.capture_count * sizeof(std::int32_t);
  return required_payload_size <= record.storage_size_bytes - sizeof(void *);
}

void CopyRuntimeBlockRawStorage(RuntimeBlockRecord &record,
                                const void *storage) {
  record.storage_words.assign(
      RuntimeBlockStorageWordCount(record.storage_size_bytes), 0u);
  std::memcpy(record.storage_words.data(), storage, record.storage_size_bytes);
}

bool LoadRuntimeBlockDescriptor(RuntimeBlockRecord &record) {
  const RuntimeBlockDescriptor *descriptor = nullptr;
  std::memcpy(&descriptor, record.storage_words.data(), sizeof(descriptor));
  if (descriptor == nullptr || descriptor->invoke == nullptr ||
      descriptor->storage_size_bytes != record.storage_size_bytes ||
      descriptor->parameter_count > 4u) {
    return false;
  }

  const bool descriptor_uses_pointer_capture_storage =
      (descriptor->flags &
       kRuntimeBlockDescriptorPointerCaptureStorageFlag) != 0u;
  const bool descriptor_has_copy_helper =
      (descriptor->flags & kRuntimeBlockDescriptorCopyHelperFlag) != 0u;
  const bool descriptor_has_dispose_helper =
      (descriptor->flags & kRuntimeBlockDescriptorDisposeHelperFlag) != 0u;
  if (descriptor_uses_pointer_capture_storage !=
      record.has_pointer_capture_storage) {
    return false;
  }
  if ((!descriptor_uses_pointer_capture_storage &&
       (descriptor_has_copy_helper || descriptor_has_dispose_helper)) ||
      !RuntimeBlockDescriptorCaptureCountMatches(record, *descriptor)) {
    return false;
  }

  record.descriptor = descriptor;
  record.descriptor_storage_size_bytes = descriptor->storage_size_bytes;
  record.descriptor_capture_count = descriptor->capture_count;
  record.descriptor_parameter_count = descriptor->parameter_count;
  record.descriptor_flags = descriptor->flags;
  record.invoke = descriptor->invoke;
  return true;
}

bool LoadRuntimeBlockCopyDisposeHelpers(RuntimeBlockRecord &record) {
  if (!record.has_pointer_capture_storage) {
    return true;
  }
  if (!RuntimeBlockPointerCaptureHeaderIsPresent(record)) {
    return false;
  }

  const unsigned char *const storage_bytes = RuntimeBlockStorageBytes(record);
  std::memcpy(&record.copy_helper, storage_bytes + sizeof(void *),
              sizeof(record.copy_helper));
  std::memcpy(&record.dispose_helper, storage_bytes + sizeof(void *) * 2u,
              sizeof(record.dispose_helper));
  if ((record.descriptor_flags & kRuntimeBlockDescriptorCopyHelperFlag) != 0u &&
      record.copy_helper == nullptr) {
    return false;
  }
  if ((record.descriptor_flags &
       kRuntimeBlockDescriptorDisposeHelperFlag) != 0u &&
      record.dispose_helper == nullptr) {
    return false;
  }
  return true;
}

bool PromoteRuntimeBlockPointerCaptures(RuntimeBlockRecord &record) {
  if (!record.has_pointer_capture_storage) {
    return true;
  }
  if (!PromotePointerCaptureCellsIntoRuntimeOwnedStorage(record)) {
    return false;
  }
  if (record.copy_helper != nullptr) {
    record.copy_helper(record.storage_words.data());
  }
  return true;
}

}  // namespace

bool BuildRuntimeBlockRecord(int block_handle,
                             const void *storage,
                             std::uint64_t storage_size_bytes,
                             int has_pointer_capture_storage,
                             RuntimeBlockRecord *record) {
  if (record == nullptr || storage == nullptr ||
      !RuntimeBlockStorageSizeIsSupported(
          static_cast<std::size_t>(storage_size_bytes))) {
    return false;
  }

  RuntimeBlockRecord built;
  built.block_handle = block_handle;
  built.has_pointer_capture_storage =
      RuntimeBlockCaptureStorageNeedsCopyDispose(has_pointer_capture_storage);
  built.storage_size_bytes =
      static_cast<std::size_t>(std::max<std::uint64_t>(storage_size_bytes, 1u));
  CopyRuntimeBlockRawStorage(built, storage);
  if (!LoadRuntimeBlockDescriptor(built)) {
    return false;
  }
  if (!LoadRuntimeBlockCopyDisposeHelpers(built)) {
    return false;
  }
  if (!PromoteRuntimeBlockPointerCaptures(built)) {
    return false;
  }
  if (built.invoke == nullptr) {
    return false;
  }

  *record = std::move(built);
  return true;
}

}  // namespace objc3c::runtime
