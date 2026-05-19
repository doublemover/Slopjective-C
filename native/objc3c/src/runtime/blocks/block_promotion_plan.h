#pragma once

#include "runtime/blocks/block_runtime_records.h"

#include <cstdint>

namespace objc3c::runtime {

struct RuntimeState;

bool BuildRuntimeBlockRecord(RuntimeState &state,
                             int block_handle,
                             const void *storage,
                             std::uint64_t storage_size_bytes,
                             int has_pointer_capture_storage,
                             RuntimeBlockRecord *record);

}  // namespace objc3c::runtime
