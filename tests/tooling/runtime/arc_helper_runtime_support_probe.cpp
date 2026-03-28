#include "runtime/objc3_runtime.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdio>

int main() {
  objc3_runtime_reset_for_testing();

  objc3_runtime_memory_management_state_snapshot inside{};
  objc3_runtime_memory_management_state_snapshot after{};

  const int retained = objc3_runtime_retain_i32(9);
  objc3_runtime_push_autoreleasepool_scope();
  const int autoreleased = objc3_runtime_autorelease_i32(retained);
  (void)objc3_runtime_copy_memory_management_state_for_testing(&inside);
  objc3_runtime_pop_autoreleasepool_scope();
  (void)objc3_runtime_copy_memory_management_state_for_testing(&after);
  const int released = objc3_runtime_release_i32(retained);

  std::printf("{");
  std::printf("\"retained\":%d,", retained);
  std::printf("\"autoreleased\":%d,", autoreleased);
  std::printf("\"released\":%d,", released);
  std::printf("\"inside_depth\":%llu,", static_cast<unsigned long long>(inside.autoreleasepool_depth));
  std::printf("\"inside_queue_count\":%llu,", static_cast<unsigned long long>(inside.queued_autorelease_value_count));
  std::printf("\"inside_last_autoreleased\":%d,", inside.last_autoreleased_value);
  std::printf("\"after_depth\":%llu,", static_cast<unsigned long long>(after.autoreleasepool_depth));
  std::printf("\"after_drained_count\":%llu,", static_cast<unsigned long long>(after.drained_autorelease_value_count));
  std::printf("\"after_last_drained\":%d", after.last_drained_autorelease_value);
  std::printf("}\n");
  return 0;
}
