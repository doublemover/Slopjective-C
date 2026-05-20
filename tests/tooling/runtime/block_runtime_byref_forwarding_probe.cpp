#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/blocks/block_byref_cells.h"
#include "runtime/blocks/block_descriptor.h"

#include <cstdint>
#include <iostream>

struct ProbeBlockStorage {
  const objc3c::runtime::RuntimeBlockDescriptor *descriptor;
  void (*copy)(void *);
  void (*dispose)(void *);
  objc3c::runtime::RuntimeBlockByrefCell *seed;
  objc3c::runtime::RuntimeBlockByrefCell *payload;
};

namespace {
int g_copy_count = 0;
int g_dispose_count = 0;
int g_last_disposed_value = 0;
int g_byref_destroy_count = 0;
int g_byref_destroyed_value_sum = 0;

std::uintptr_t AddressOf(const void *ptr) {
  return reinterpret_cast<std::uintptr_t>(ptr);
}

objc3c::runtime::RuntimeBlockByrefCell *Forwarded(
    objc3c::runtime::RuntimeBlockByrefCell *cell) {
  return objc3c::runtime::ResolveRuntimeBlockByrefForwardingCell(cell);
}

extern "C" void ProbeByrefDestroy(void *raw_cell) {
  auto *cell =
      static_cast<objc3c::runtime::RuntimeBlockByrefCell *>(raw_cell);
  ++g_byref_destroy_count;
  if (cell != nullptr) {
    g_byref_destroyed_value_sum += cell->value;
  }
}

extern "C" void ProbeCopy(void *raw_block) {
  auto *block = static_cast<ProbeBlockStorage *>(raw_block);
  ++g_copy_count;
  if (block == nullptr || block->payload == nullptr) {
    return;
  }
}

extern "C" void ProbeDispose(void *raw_block) {
  auto *block = static_cast<ProbeBlockStorage *>(raw_block);
  ++g_dispose_count;
  if (block == nullptr || block->payload == nullptr) {
    return;
  }
  auto *payload = Forwarded(block->payload);
  if (payload != nullptr) {
    g_last_disposed_value = payload->value;
  }
}

extern "C" int ProbeInvoke(void *raw_block, int delta, int, int, int) {
  auto *block = static_cast<ProbeBlockStorage *>(raw_block);
  if (block == nullptr || block->seed == nullptr || block->payload == nullptr) {
    return 0;
  }
  auto *seed = Forwarded(block->seed);
  auto *payload = Forwarded(block->payload);
  if (seed == nullptr || payload == nullptr) {
    return 0;
  }
  seed->value += delta;
  return seed->value + payload->value;
}

const objc3c::runtime::RuntimeBlockDescriptor &ProbeDescriptor() {
  static const objc3c::runtime::RuntimeBlockDescriptor descriptor{
      sizeof(ProbeBlockStorage),
      2,
      1,
      objc3c::runtime::kRuntimeBlockDescriptorPointerCaptureStorageFlag |
          objc3c::runtime::kRuntimeBlockDescriptorCopyHelperFlag |
          objc3c::runtime::kRuntimeBlockDescriptorDisposeHelperFlag |
          objc3c::runtime::kRuntimeBlockDescriptorByrefForwardingCellsFlag,
      0,
      &ProbeInvoke};
  return descriptor;
}
}  // namespace

int main() {
  objc3_runtime_reset_for_testing();

  objc3c::runtime::RuntimeBlockByrefCell seed_cell;
  objc3c::runtime::RuntimeBlockByrefCell payload_cell;
  objc3c::runtime::InitializeRuntimeBlockStackByrefCell(
      &seed_cell, 7, &ProbeByrefDestroy);
  objc3c::runtime::InitializeRuntimeBlockStackByrefCell(
      &payload_cell, 11, &ProbeByrefDestroy);
  ProbeBlockStorage storage{&ProbeDescriptor(), &ProbeCopy, &ProbeDispose,
                            &seed_cell, &payload_cell};
  ProbeBlockStorage second_storage{&ProbeDescriptor(), &ProbeCopy,
                                   &ProbeDispose, &seed_cell, &payload_cell};

  const int handle =
      objc3_runtime_promote_block_i32(&storage, sizeof(storage), 1);
  const int copy_count_after_promotion = g_copy_count;
  const auto *seed_forwarding_after_first_promotion = Forwarded(&seed_cell);
  const auto *payload_forwarding_after_first_promotion =
      Forwarded(&payload_cell);
  const int second_handle =
      objc3_runtime_promote_block_i32(&second_storage, sizeof(second_storage),
                                      1);
  const auto *seed_forwarding_after_second_promotion = Forwarded(&seed_cell);
  const auto *payload_forwarding_after_second_promotion =
      Forwarded(&payload_cell);
  seed_cell.value = 100;
  payload_cell.value = 200;

  const int first_invoke = objc3_runtime_invoke_block_i32(handle, 5, 0, 0, 0);
  const int second_invoke =
      objc3_runtime_invoke_block_i32(second_handle, 2, 0, 0, 0);
  Forwarded(&seed_cell)->value = 30;
  const int forwarded_stack_write_invoke =
      objc3_runtime_invoke_block_i32(handle, 1, 0, 0, 0);
  const int dispose_count_before_final_release = g_dispose_count;
  const int final_release = objc3_runtime_release_i32(handle);
  const int dispose_count_after_final_release = g_dispose_count;
  const int byref_destroy_count_after_final_release = g_byref_destroy_count;
  const int invoke_after_release =
      objc3_runtime_invoke_block_i32(handle, 1, 0, 0, 0);
  const int second_final_release = objc3_runtime_release_i32(second_handle);
  const int second_invoke_after_release =
      objc3_runtime_invoke_block_i32(second_handle, 1, 0, 0, 0);

  std::cout << "{\n"
            << "  \"handle\": " << handle << ",\n"
            << "  \"second_handle\": " << second_handle << ",\n"
            << "  \"copy_count_after_promotion\": "
            << copy_count_after_promotion << ",\n"
            << "  \"copy_count_after_second_promotion\": " << g_copy_count
            << ",\n"
            << "  \"seed_stack_address\": " << AddressOf(&seed_cell) << ",\n"
            << "  \"seed_forwarding_address_after_first_promotion\": "
            << AddressOf(seed_forwarding_after_first_promotion) << ",\n"
            << "  \"seed_forwarding_address_after_second_promotion\": "
            << AddressOf(seed_forwarding_after_second_promotion) << ",\n"
            << "  \"payload_stack_address\": " << AddressOf(&payload_cell)
            << ",\n"
            << "  \"payload_forwarding_address_after_first_promotion\": "
            << AddressOf(payload_forwarding_after_first_promotion) << ",\n"
            << "  \"payload_forwarding_address_after_second_promotion\": "
            << AddressOf(payload_forwarding_after_second_promotion) << ",\n"
            << "  \"stack_seed_forwarding_updated\": "
            << (seed_forwarding_after_first_promotion != &seed_cell ? 1 : 0)
            << ",\n"
            << "  \"stack_payload_forwarding_updated\": "
            << (payload_forwarding_after_first_promotion != &payload_cell ? 1
                                                                          : 0)
            << ",\n"
            << "  \"second_handle_shared_seed_forwarding\": "
            << (seed_forwarding_after_first_promotion ==
                        seed_forwarding_after_second_promotion
                    ? 1
                    : 0)
            << ",\n"
            << "  \"second_handle_shared_payload_forwarding\": "
            << (payload_forwarding_after_first_promotion ==
                        payload_forwarding_after_second_promotion
                    ? 1
                    : 0)
            << ",\n"
            << "  \"first_invoke_result\": " << first_invoke << ",\n"
            << "  \"second_invoke_result\": " << second_invoke << ",\n"
            << "  \"forwarded_stack_write_invoke_result\": "
            << forwarded_stack_write_invoke << ",\n"
            << "  \"dispose_count_before_final_release\": "
            << dispose_count_before_final_release << ",\n"
            << "  \"dispose_count_after_final_release\": "
            << dispose_count_after_final_release << ",\n"
            << "  \"last_disposed_value\": " << g_last_disposed_value << ",\n"
            << "  \"byref_destroy_count_after_final_release\": "
            << byref_destroy_count_after_final_release << ",\n"
            << "  \"final_release_result\": " << final_release << ",\n"
            << "  \"invoke_after_release_result\": " << invoke_after_release
            << ",\n"
            << "  \"second_final_release_result\": " << second_final_release
            << ",\n"
            << "  \"second_invoke_after_release_result\": "
            << second_invoke_after_release << ",\n"
            << "  \"byref_destroy_count_after_second_final_release\": "
            << g_byref_destroy_count << ",\n"
            << "  \"byref_destroyed_value_sum\": "
            << g_byref_destroyed_value_sum
            << "\n"
            << "}\n";
  return handle > 0 && second_handle > 0 ? 0 : 1;
}
