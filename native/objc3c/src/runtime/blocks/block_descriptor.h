#pragma once

#include <cstdint>

namespace objc3c::runtime {

using RuntimeBlockInvokeFn = int (*)(void *, int, int, int, int);

constexpr std::uint32_t kRuntimeBlockDescriptorPointerCaptureStorageFlag =
    1u << 0u;
constexpr std::uint32_t kRuntimeBlockDescriptorCopyHelperFlag = 1u << 1u;
constexpr std::uint32_t kRuntimeBlockDescriptorDisposeHelperFlag = 1u << 2u;

struct RuntimeBlockDescriptor {
  std::uint64_t storage_size_bytes = 0;
  std::uint64_t capture_count = 0;
  std::uint32_t parameter_count = 0;
  std::uint32_t flags = 0;
  std::uint32_t reserved = 0;
  RuntimeBlockInvokeFn invoke = nullptr;
};

}  // namespace objc3c::runtime
