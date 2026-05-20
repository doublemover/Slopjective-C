#include "runtime/blocks/block_byref_cells.h"

#include "runtime/state/runtime_state_records.h"

#include <cstddef>
#include <memory>

namespace objc3c::runtime {
namespace {

RuntimeBlockByrefCell MakeRuntimeBlockHeapByrefCell(
    std::int32_t value,
    std::uint64_t flags,
    std::uint64_t ordinal,
    RuntimeBlockByrefCellDestroyFn destroy_helper) {
  RuntimeBlockByrefCell cell;
  cell.value = value;
  cell.forwarding = nullptr;
  cell.flags = flags | kRuntimeBlockByrefCellHeapFlag;
  cell.size_bytes = sizeof(RuntimeBlockByrefCell);
  cell.ordinal = ordinal;
  cell.destroy_helper = destroy_helper;
  cell.magic = kRuntimeBlockByrefCellMagic;
  return cell;
}

std::shared_ptr<RuntimeBlockByrefCell> AllocateRuntimeBlockHeapByrefCell(
    RuntimeBlockByrefCell cell) {
  auto *raw_cell = new RuntimeBlockByrefCell(cell);
  raw_cell->forwarding = raw_cell;
  return std::shared_ptr<RuntimeBlockByrefCell>(
      raw_cell, [](RuntimeBlockByrefCell *owned_cell) {
        if (owned_cell == nullptr) {
          return;
        }
        if (owned_cell->destroy_helper != nullptr) {
          owned_cell->destroy_helper(owned_cell);
        }
        delete owned_cell;
      });
}

std::shared_ptr<RuntimeBlockByrefCell> FindLiveForwardedCell(
    RuntimeState &state,
    RuntimeBlockByrefCell *forwarding_cell) {
  if (forwarding_cell == nullptr) {
    return nullptr;
  }
  const auto found =
      state.runtime_block_byref_cells_by_heap_address.find(forwarding_cell);
  if (found == state.runtime_block_byref_cells_by_heap_address.end()) {
    return nullptr;
  }
  auto shared = found->second.lock();
  if (!shared) {
    state.runtime_block_byref_cells_by_heap_address.erase(found);
  }
  return shared;
}

void RememberForwardedCell(RuntimeState &state,
                           const std::shared_ptr<RuntimeBlockByrefCell> &cell) {
  if (cell) {
    state.runtime_block_byref_cells_by_heap_address[cell.get()] = cell;
  }
}

}  // namespace

void InitializeRuntimeBlockStackByrefCell(
    RuntimeBlockByrefCell *cell,
    std::int32_t value,
    RuntimeBlockByrefCellDestroyFn destroy_helper) {
  if (cell == nullptr) {
    return;
  }
  cell->value = value;
  cell->reserved = 0u;
  cell->forwarding = cell;
  cell->flags = 0u;
  cell->size_bytes = sizeof(RuntimeBlockByrefCell);
  cell->ordinal = 0u;
  cell->destroy_helper = destroy_helper;
  cell->magic = kRuntimeBlockByrefCellMagic;
}

RuntimeBlockByrefCell *ResolveRuntimeBlockByrefForwardingCell(
    RuntimeBlockByrefCell *cell) {
  if (cell == nullptr || cell->forwarding == nullptr) {
    return cell;
  }
  return cell->forwarding;
}

const RuntimeBlockByrefCell *ResolveRuntimeBlockByrefForwardingCell(
    const RuntimeBlockByrefCell *cell) {
  if (cell == nullptr || cell->forwarding == nullptr) {
    return cell;
  }
  return cell->forwarding;
}

bool RuntimeBlockByrefCellIsWellFormed(const RuntimeBlockByrefCell *cell) {
  return cell != nullptr && cell->magic == kRuntimeBlockByrefCellMagic &&
         cell->size_bytes >= sizeof(RuntimeBlockByrefCell) &&
         cell->forwarding != nullptr;
}

std::shared_ptr<RuntimeBlockByrefCell> PromoteRuntimeBlockByrefCell(
    RuntimeState &state,
    RuntimeBlockByrefCell *source_cell) {
  if (!RuntimeBlockByrefCellIsWellFormed(source_cell)) {
    return nullptr;
  }

  RuntimeBlockByrefCell *forwarding_cell =
      ResolveRuntimeBlockByrefForwardingCell(source_cell);
  if (forwarding_cell != source_cell) {
    return FindLiveForwardedCell(state, forwarding_cell);
  }

  auto promoted = AllocateRuntimeBlockHeapByrefCell(
      MakeRuntimeBlockHeapByrefCell(source_cell->value, 0u,
                                    state.next_runtime_block_byref_cell_ordinal++,
                                    source_cell->destroy_helper));
  source_cell->forwarding = promoted.get();
  source_cell->flags |= kRuntimeBlockByrefCellForwardedFlag;
  RememberForwardedCell(state, promoted);
  return promoted;
}

std::shared_ptr<RuntimeBlockByrefCell> PromoteRuntimeBlockRawPointerCell(
    void *source_cell) {
  if (source_cell == nullptr) {
    return nullptr;
  }
  std::int32_t value = 0;
  static_assert(sizeof(value) == sizeof(int));
  value = *static_cast<int *>(source_cell);
  return AllocateRuntimeBlockHeapByrefCell(MakeRuntimeBlockHeapByrefCell(
      value, kRuntimeBlockByrefCellRawPointerAdapterFlag, 0u, nullptr));
}

}  // namespace objc3c::runtime
