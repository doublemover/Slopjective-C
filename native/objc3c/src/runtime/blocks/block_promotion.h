#pragma once

#include <cstdint>

namespace objc3c::runtime {

int PromoteRuntimeBlockI32(const void *storage, std::uint64_t storage_size_bytes,
                           int has_pointer_capture_storage);

}  // namespace objc3c::runtime
