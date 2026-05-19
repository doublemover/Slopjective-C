#pragma once

#include "runtime/blocks/block_runtime_records.h"

#include <cstddef>

namespace objc3c::runtime {

constexpr std::size_t kRuntimeBlockPointerCaptureHeaderSlotCount = 3u;

unsigned char *RuntimeBlockStorageBytes(RuntimeBlockRecord &record);
const unsigned char *RuntimeBlockStorageBytes(const RuntimeBlockRecord &record);
std::size_t RuntimeBlockPointerCaptureSlotCount(
    const RuntimeBlockRecord &record);
bool PromotePointerCaptureCellsIntoRuntimeOwnedStorage(
    RuntimeBlockRecord &record);

}  // namespace objc3c::runtime
