#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <iostream>

struct ProbeBlockStorage {
  int (*invoke)(void *, int, int, int, int);
  void (*copy)(void *);
  void (*dispose)(void *);
  int *seed;
  int *owned;
};

namespace {
int g_copy_count = 0;
int g_dispose_count = 0;
int g_last_disposed_value = 0;

extern "C" void ProbeCopy(void *raw_block) {
  auto *block = static_cast<ProbeBlockStorage *>(raw_block);
  ++g_copy_count;
  if (block == nullptr || block->owned == nullptr) {
    return;
  }
}

extern "C" void ProbeDispose(void *raw_block) {
  auto *block = static_cast<ProbeBlockStorage *>(raw_block);
  ++g_dispose_count;
  if (block == nullptr || block->owned == nullptr) {
    return;
  }
  g_last_disposed_value = block->owned[0];
}

extern "C" int ProbeInvoke(void *raw_block, int delta, int, int, int) {
  auto *block = static_cast<ProbeBlockStorage *>(raw_block);
  if (block == nullptr || block->seed == nullptr || block->owned == nullptr) {
    return 0;
  }
  block->seed[0] += delta;
  return block->seed[0] + block->owned[0];
}
}  // namespace

int main() {
  objc3_runtime_reset_for_testing();

  int seed = 7;
  int owned = 11;
  ProbeBlockStorage storage{&ProbeInvoke, &ProbeCopy, &ProbeDispose, &seed,
                            &owned};

  const int handle =
      objc3_runtime_promote_block_i32(&storage, sizeof(storage), 1);
  seed = 100;
  owned = 200;

  const int first_invoke = objc3_runtime_invoke_block_i32(handle, 5, 0, 0, 0);
  const int second_invoke = objc3_runtime_invoke_block_i32(handle, 2, 0, 0, 0);
  const int dispose_count_before_final_release = g_dispose_count;
  const int final_release = objc3_runtime_release_i32(handle);
  const int invoke_after_release =
      objc3_runtime_invoke_block_i32(handle, 1, 0, 0, 0);

  std::cout << "{\n"
            << "  \"handle\": " << handle << ",\n"
            << "  \"copy_count_after_promotion\": " << g_copy_count << ",\n"
            << "  \"first_invoke_result\": " << first_invoke << ",\n"
            << "  \"second_invoke_result\": " << second_invoke << ",\n"
            << "  \"dispose_count_before_final_release\": "
            << dispose_count_before_final_release << ",\n"
            << "  \"dispose_count_after_final_release\": "
            << g_dispose_count << ",\n"
            << "  \"last_disposed_value\": " << g_last_disposed_value << ",\n"
            << "  \"final_release_result\": " << final_release << ",\n"
            << "  \"invoke_after_release_result\": " << invoke_after_release
            << "\n"
            << "}\n";
  return handle > 0 ? 0 : 1;
}
