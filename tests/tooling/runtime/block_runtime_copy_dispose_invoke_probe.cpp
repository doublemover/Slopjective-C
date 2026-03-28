#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdio>

namespace {

struct ProbeCaptureState {
  int base = 0;
  int copy_count = 0;
  int dispose_count = 0;
};

struct ProbeBlockStorage {
  int (*invoke)(void *, int, int, int, int) = nullptr;
  void (*copy)(void *) = nullptr;
  void (*dispose)(void *) = nullptr;
  ProbeCaptureState *capture = nullptr;
};

extern "C" int ProbeInvoke(void *storage, int a0, int a1, int a2, int a3) {
  auto *block = static_cast<ProbeBlockStorage *>(storage);
  if (block == nullptr || block->capture == nullptr) {
    return -1;
  }
  return block->capture->base + a0 + a1 + a2 + a3 +
         block->capture->copy_count * 100 +
         block->capture->dispose_count * 1000;
}

extern "C" void ProbeCopy(void *storage) {
  auto *block = static_cast<ProbeBlockStorage *>(storage);
  if (block == nullptr || block->capture == nullptr) {
    return;
  }
  ++block->capture->copy_count;
}

extern "C" void ProbeDispose(void *storage) {
  auto *block = static_cast<ProbeBlockStorage *>(storage);
  if (block == nullptr || block->capture == nullptr) {
    return;
  }
  ++block->capture->dispose_count;
}

}  // namespace

int main() {
  objc3_runtime_reset_for_testing();

  ProbeCaptureState capture{7, 0, 0};
  ProbeBlockStorage block{&ProbeInvoke, &ProbeCopy, &ProbeDispose, &capture};

  const int handle =
      objc3_runtime_promote_block_i32(&block, sizeof(block), 1);
  const int copy_count_after_promotion = capture.copy_count;
  const int invoke_result =
      handle > 0 ? objc3_runtime_invoke_block_i32(handle, 1, 2, 3, 4) : 0;
  const int retain_result = handle > 0 ? objc3_runtime_retain_i32(handle) : 0;
  const int release_result =
      handle > 0 ? objc3_runtime_release_i32(handle) : 0;
  const int dispose_count_before_final_release = capture.dispose_count;
  const int final_release_result =
      handle > 0 ? objc3_runtime_release_i32(handle) : 0;
  const int dispose_count_after_final_release = capture.dispose_count;
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
  std::printf("\"invoke_after_release_result\":%d",
              invoke_after_release_result);
  std::printf("}");
  return handle > 0 ? 0 : 1;
}
