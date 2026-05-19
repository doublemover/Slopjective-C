#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace objc3c::runtime {

using RuntimeBlockInvokeFn = int (*)(void *, int, int, int, int);
using RuntimeBlockCopyHelperFn = void (*)(void *);
using RuntimeBlockDisposeHelperFn = void (*)(void *);

// Private runtime block state. The compiler/runtime contract remains an opaque
// copied storage blob plus a private invoke pointer; no public block-object
// memory layout is promised.
struct RuntimeBlockRecord {
  int block_handle = 0;
  bool has_pointer_capture_storage = false;
  std::size_t storage_size_bytes = 0;
  std::vector<std::uint64_t> storage_words;
  std::vector<std::unique_ptr<int>> promoted_capture_cells;
  RuntimeBlockInvokeFn invoke = nullptr;
  RuntimeBlockCopyHelperFn copy_helper = nullptr;
  RuntimeBlockDisposeHelperFn dispose_helper = nullptr;
  std::uint64_t retain_count = 1;
};

}  // namespace objc3c::runtime
