#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>

namespace objc3c::runtime {

struct RuntimeState;

using RuntimeBlockByrefCellDestroyFn = void (*)(void *);

constexpr std::uint64_t kRuntimeBlockByrefCellMagic =
    0x0B1C3B10C3B1F001ull;
constexpr std::uint64_t kRuntimeBlockByrefCellHeapFlag = 1ull << 0u;
constexpr std::uint64_t kRuntimeBlockByrefCellForwardedFlag = 1ull << 1u;
constexpr std::uint64_t kRuntimeBlockByrefCellRawPointerAdapterFlag =
    1ull << 2u;

// The value field intentionally stays at offset zero. Existing i32 block
// invoke thunks can continue to treat a capture slot as an i32* while the
// runtime tracks forwarding identity and shared heap lifetime around it.
struct RuntimeBlockByrefCell {
  std::int32_t value = 0;
  std::uint32_t reserved = 0;
  RuntimeBlockByrefCell *forwarding = nullptr;
  std::uint64_t flags = 0;
  std::uint64_t size_bytes = sizeof(RuntimeBlockByrefCell);
  std::uint64_t ordinal = 0;
  RuntimeBlockByrefCellDestroyFn destroy_helper = nullptr;
  std::uint64_t magic = kRuntimeBlockByrefCellMagic;
};

static_assert(offsetof(RuntimeBlockByrefCell, value) == 0u);

void InitializeRuntimeBlockStackByrefCell(
    RuntimeBlockByrefCell *cell,
    std::int32_t value,
    RuntimeBlockByrefCellDestroyFn destroy_helper = nullptr);
RuntimeBlockByrefCell *ResolveRuntimeBlockByrefForwardingCell(
    RuntimeBlockByrefCell *cell);
const RuntimeBlockByrefCell *ResolveRuntimeBlockByrefForwardingCell(
    const RuntimeBlockByrefCell *cell);
bool RuntimeBlockByrefCellIsWellFormed(const RuntimeBlockByrefCell *cell);
std::shared_ptr<RuntimeBlockByrefCell> PromoteRuntimeBlockByrefCell(
    RuntimeState &state,
    RuntimeBlockByrefCell *source_cell);
std::shared_ptr<RuntimeBlockByrefCell> PromoteRuntimeBlockRawPointerCell(
    void *source_cell);

}  // namespace objc3c::runtime
