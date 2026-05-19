#include "runtime/blocks/block_promotion_plan.h"

#include "runtime/blocks/block_capture_storage.h"
#include "runtime/blocks/block_pointer_capture_storage.h"
#include "runtime/blocks/block_record.h"

#include <algorithm>
#include <cstddef>
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

void CopyRuntimeBlockRawStorage(RuntimeBlockRecord &record,
                                const void *storage) {
  record.storage_words.assign(
      RuntimeBlockStorageWordCount(record.storage_size_bytes), 0u);
  std::memcpy(record.storage_words.data(), storage, record.storage_size_bytes);
}

void LoadRuntimeBlockInvokePointer(RuntimeBlockRecord &record) {
  std::memcpy(&record.invoke, record.storage_words.data(),
              sizeof(record.invoke));
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
  LoadRuntimeBlockInvokePointer(built);
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
