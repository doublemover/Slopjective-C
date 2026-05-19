#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdio>

namespace {

struct ProbeBlockStorage {
  int (*invoke)(void *, int, int, int, int) = nullptr;
  void (*copy)(void *) = nullptr;
  void (*dispose)(void *) = nullptr;
  int *captured_base = nullptr;
};

int g_copy_count = 0;
int g_dispose_count = 0;
int g_post_release_callback_count = 0;
bool g_allow_invoke_storage_access = true;

extern "C" int ProbeInvoke(void *storage, int a0, int a1, int a2, int a3) {
  if (!g_allow_invoke_storage_access) {
    ++g_post_release_callback_count;
    return -777;
  }
  auto *block = static_cast<ProbeBlockStorage *>(storage);
  if (block == nullptr || block->captured_base == nullptr) {
    return -1;
  }
  return *block->captured_base + a0 + a1 + a2 + a3 + g_copy_count * 100 +
         g_dispose_count * 1000;
}

extern "C" void ProbeCopy(void *storage) {
  auto *block = static_cast<ProbeBlockStorage *>(storage);
  if (block == nullptr || block->captured_base == nullptr) {
    return;
  }
  ++g_copy_count;
}

extern "C" void ProbeDispose(void *storage) {
  auto *block = static_cast<ProbeBlockStorage *>(storage);
  if (block == nullptr || block->captured_base == nullptr) {
    return;
  }
  ++g_dispose_count;
}

}  // namespace

int main() {
  objc3_runtime_reset_for_testing();
  g_copy_count = 0;
  g_dispose_count = 0;
  g_post_release_callback_count = 0;
  g_allow_invoke_storage_access = true;

  int captured_base = 7;
  ProbeBlockStorage block{&ProbeInvoke, &ProbeCopy, &ProbeDispose,
                          &captured_base};

  const int handle =
      objc3_runtime_promote_block_i32(&block, sizeof(block), 1);
  captured_base = 99;
  const int copy_count_after_promotion = g_copy_count;
  const int invoke_result =
      handle > 0 ? objc3_runtime_invoke_block_i32(handle, 1, 2, 3, 4) : 0;
  const int retain_result = handle > 0 ? objc3_runtime_retain_i32(handle) : 0;
  const int release_result =
      handle > 0 ? objc3_runtime_release_i32(handle) : 0;
  const int dispose_count_before_final_release = g_dispose_count;
  const int final_release_result =
      handle > 0 ? objc3_runtime_release_i32(handle) : 0;
  const int dispose_count_after_final_release = g_dispose_count;
  g_allow_invoke_storage_access = false;
  const int invoke_after_release_result =
      handle > 0 ? objc3_runtime_invoke_block_i32(handle, 9, 0, 0, 0) : 0;

  std::printf("{");
  std::printf("\"handle\":%d,", handle);
  std::printf("\"pointer_capture_enabled\":1,");
  std::printf("\"copy_helper_present\":1,");
  std::printf("\"dispose_helper_present\":1,");
  std::printf("\"copy_count_after_promotion\":%d,",
              copy_count_after_promotion);
  std::printf("\"invoke_result\":%d,", invoke_result);
  std::printf("\"retain_result\":%d,", retain_result);
  std::printf("\"release_result\":%d,", release_result);
  std::printf("\"dispose_count_before_final_release\":%d,",
              dispose_count_before_final_release);
  std::printf("\"final_release_result\":%d,", final_release_result);
  std::printf("\"dispose_count_after_final_release\":%d,",
              dispose_count_after_final_release);
  std::printf("\"post_release_callback_count\":%d,",
              g_post_release_callback_count);
  std::printf("\"invoke_after_release_result\":%d",
              invoke_after_release_result);
  std::printf("}");
  return handle > 0 ? 0 : 1;
}
