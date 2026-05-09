#include "runtime/blocks/block_invocation.h"
#include "runtime/blocks/block_promotion.h"

#include <cstdint>

extern "C" int objc3_runtime_promote_block_i32(
    const void *storage, std::uint64_t storage_size_bytes,
    int has_pointer_capture_storage) {
  return objc3c::runtime::PromoteRuntimeBlockI32(
      storage, storage_size_bytes, has_pointer_capture_storage);
}

extern "C" int objc3_runtime_invoke_block_i32(int block_handle, int a0, int a1,
                                              int a2, int a3) {
  return objc3c::runtime::InvokeRuntimeBlockI32(block_handle, a0, a1, a2, a3);
}
