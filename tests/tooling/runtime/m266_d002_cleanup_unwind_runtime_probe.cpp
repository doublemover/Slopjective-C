#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdio>

int main() {
  objc3_runtime_reset_for_testing();

  objc3_runtime_memory_management_state_snapshot before{};
  objc3_runtime_memory_management_state_snapshot nested{};
  objc3_runtime_memory_management_state_snapshot after_inner{};
  objc3_runtime_memory_management_state_snapshot after_outer{};

  (void)objc3_runtime_copy_memory_management_state_for_testing(&before);

  const int outer = objc3_runtime_retain_i32(11);
  const int inner = objc3_runtime_retain_i32(22);
  objc3_runtime_push_autoreleasepool_scope();
  (void)objc3_runtime_autorelease_i32(outer);
  objc3_runtime_push_autoreleasepool_scope();
  (void)objc3_runtime_autorelease_i32(inner);
  (void)objc3_runtime_copy_memory_management_state_for_testing(&nested);
  objc3_runtime_pop_autoreleasepool_scope();
  (void)objc3_runtime_copy_memory_management_state_for_testing(&after_inner);
  objc3_runtime_pop_autoreleasepool_scope();
  (void)objc3_runtime_copy_memory_management_state_for_testing(&after_outer);
  (void)objc3_runtime_release_i32(inner);
  (void)objc3_runtime_release_i32(outer);

  std::printf("{");
  std::printf("\"before_depth\":%llu,", static_cast<unsigned long long>(before.autoreleasepool_depth));
  std::printf("\"nested_depth\":%llu,", static_cast<unsigned long long>(nested.autoreleasepool_depth));
  std::printf("\"nested_max_depth\":%llu,", static_cast<unsigned long long>(nested.autoreleasepool_max_depth));
  std::printf("\"nested_queue_count\":%llu,", static_cast<unsigned long long>(nested.queued_autorelease_value_count));
  std::printf("\"nested_last_autoreleased\":%d,", nested.last_autoreleased_value);
  std::printf("\"after_inner_depth\":%llu,", static_cast<unsigned long long>(after_inner.autoreleasepool_depth));
  std::printf("\"after_inner_queue_count\":%llu,", static_cast<unsigned long long>(after_inner.queued_autorelease_value_count));
  std::printf("\"after_inner_drained_count\":%llu,", static_cast<unsigned long long>(after_inner.drained_autorelease_value_count));
  std::printf("\"after_inner_last_drained\":%d,", after_inner.last_drained_autorelease_value);
  std::printf("\"after_outer_depth\":%llu,", static_cast<unsigned long long>(after_outer.autoreleasepool_depth));
  std::printf("\"after_outer_queue_count\":%llu,", static_cast<unsigned long long>(after_outer.queued_autorelease_value_count));
  std::printf("\"after_outer_drained_count\":%llu,", static_cast<unsigned long long>(after_outer.drained_autorelease_value_count));
  std::printf("\"after_outer_last_drained\":%d", after_outer.last_drained_autorelease_value);
  std::printf("}\n");
  return 0;
}
