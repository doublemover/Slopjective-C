#pragma once

#include "runtime/blocks/block_byref_cells.h"
#include "runtime/blocks/block_descriptor.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace objc3c::runtime {

using RuntimeBlockCopyHelperFn = void (*)(void *);
using RuntimeBlockDisposeHelperFn = void (*)(void *);

// Private runtime block state. The compiler/runtime contract remains an opaque
// copied storage blob plus an internal descriptor; no public block-object
// memory layout is promised.
struct RuntimeBlockRecord {
  int block_handle = 0;
  bool has_pointer_capture_storage = false;
  bool has_byref_forwarding_cells = false;
  std::size_t storage_size_bytes = 0;
  std::vector<std::uint64_t> storage_words;
  std::vector<std::shared_ptr<RuntimeBlockByrefCell>> promoted_capture_cells;
  const RuntimeBlockDescriptor *descriptor = nullptr;
  std::uint64_t descriptor_storage_size_bytes = 0;
  std::uint64_t descriptor_capture_count = 0;
  std::uint32_t descriptor_parameter_count = 0;
  std::uint32_t descriptor_flags = 0;
  RuntimeBlockInvokeFn invoke = nullptr;
  RuntimeBlockCopyHelperFn copy_helper = nullptr;
  RuntimeBlockDisposeHelperFn dispose_helper = nullptr;
  std::uint64_t retain_count = 1;
};

}  // namespace objc3c::runtime
