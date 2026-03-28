#include <cstdio>

#include "runtime/objc3_runtime.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

extern "C" void CloseFd(int) {}
extern "C" void ReleaseTemp(int) {}
extern "C" int CFRetain(int value) { return value; }
extern "C" void CFRelease(int) {}
extern "C" int CFAutorelease(int value) { return value; }

int main() {
  objc3_runtime_reset_for_testing();

  objc3_runtime_push_autoreleasepool_scope();
  const int retained = objc3_runtime_retain_i32(77);
  const int autoreleased = objc3_runtime_autorelease_i32(retained);
  const int released = objc3_runtime_release_i32(retained);

  objc3_runtime_memory_management_state_snapshot before_pop{};
  objc3_runtime_arc_debug_state_snapshot arc_before_pop{};
  const int memory_before_status =
      objc3_runtime_copy_memory_management_state_for_testing(&before_pop);
  const int arc_before_status =
      objc3_runtime_copy_arc_debug_state_for_testing(&arc_before_pop);

  objc3_runtime_pop_autoreleasepool_scope();

  objc3_runtime_memory_management_state_snapshot after_pop{};
  objc3_runtime_arc_debug_state_snapshot arc_after_pop{};
  const int memory_after_status =
      objc3_runtime_copy_memory_management_state_for_testing(&after_pop);
  const int arc_after_status =
      objc3_runtime_copy_arc_debug_state_for_testing(&arc_after_pop);

  std::printf("retained=%d\n", retained);
  std::printf("autoreleased=%d\n", autoreleased);
  std::printf("released=%d\n", released);
  std::printf("memory_before_status=%d\n", memory_before_status);
  std::printf("arc_before_status=%d\n", arc_before_status);
  std::printf("memory_after_status=%d\n", memory_after_status);
  std::printf("arc_after_status=%d\n", arc_after_status);
  std::printf("before_depth=%llu\n",
              static_cast<unsigned long long>(before_pop.autoreleasepool_depth));
  std::printf("before_queued=%llu\n",
              static_cast<unsigned long long>(before_pop.queued_autorelease_value_count));
  std::printf("before_drained=%llu\n",
              static_cast<unsigned long long>(before_pop.drained_autorelease_value_count));
  std::printf("before_last_autoreleased=%d\n", before_pop.last_autoreleased_value);
  std::printf("after_depth=%llu\n",
              static_cast<unsigned long long>(after_pop.autoreleasepool_depth));
  std::printf("after_queued=%llu\n",
              static_cast<unsigned long long>(after_pop.queued_autorelease_value_count));
  std::printf("after_drained=%llu\n",
              static_cast<unsigned long long>(after_pop.drained_autorelease_value_count));
  std::printf("after_last_drained=%d\n", after_pop.last_drained_autorelease_value);
  std::printf("retain_call_count=%llu\n",
              static_cast<unsigned long long>(arc_after_pop.retain_call_count));
  std::printf("release_call_count=%llu\n",
              static_cast<unsigned long long>(arc_after_pop.release_call_count));
  std::printf("autorelease_call_count=%llu\n",
              static_cast<unsigned long long>(arc_after_pop.autorelease_call_count));
  std::printf("autoreleasepool_push_count=%llu\n",
              static_cast<unsigned long long>(arc_after_pop.autoreleasepool_push_count));
  std::printf("autoreleasepool_pop_count=%llu\n",
              static_cast<unsigned long long>(arc_after_pop.autoreleasepool_pop_count));
  std::printf("last_retain_value=%d\n", arc_after_pop.last_retain_value);
  std::printf("last_release_value=%d\n", arc_after_pop.last_release_value);
  std::printf("last_autorelease_value=%d\n", arc_after_pop.last_autorelease_value);

  if (retained != 77 || autoreleased != 77 || released != 77 ||
      memory_before_status != OBJC3_RUNTIME_REGISTRATION_STATUS_OK ||
      arc_before_status != OBJC3_RUNTIME_REGISTRATION_STATUS_OK ||
      memory_after_status != OBJC3_RUNTIME_REGISTRATION_STATUS_OK ||
      arc_after_status != OBJC3_RUNTIME_REGISTRATION_STATUS_OK ||
      before_pop.autoreleasepool_depth != 1 ||
      before_pop.queued_autorelease_value_count != 1 ||
      before_pop.last_autoreleased_value != 77 ||
      after_pop.autoreleasepool_depth != 0 ||
      after_pop.drained_autorelease_value_count != 1 ||
      after_pop.last_drained_autorelease_value != 77 ||
      arc_after_pop.retain_call_count != 1 ||
      arc_after_pop.release_call_count != 1 ||
      arc_after_pop.autorelease_call_count != 1 ||
      arc_after_pop.autoreleasepool_push_count != 1 ||
      arc_after_pop.autoreleasepool_pop_count != 1 ||
      arc_after_pop.last_retain_value != 77 ||
      arc_after_pop.last_release_value != 77 ||
      arc_after_pop.last_autorelease_value != 77) {
    return 1;
  }
  return 0;
}
