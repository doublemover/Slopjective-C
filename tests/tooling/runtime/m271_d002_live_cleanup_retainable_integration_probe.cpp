#include <cstdio>

#include "runtime/objc3_runtime.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

extern "C" int helperSurface(int owner, int bytes);

namespace {
int g_close_fd_count = 0;
int g_last_close_fd_value = 0;
int g_release_temp_count = 0;
int g_last_release_temp_value = 0;
}  // namespace

extern "C" void CloseFd(int value) {
  ++g_close_fd_count;
  g_last_close_fd_value = value;
}

extern "C" void ReleaseTemp(int value) {
  ++g_release_temp_count;
  g_last_release_temp_value = value;
}

extern "C" int CFRetain(int value) { return objc3_runtime_retain_i32(value); }
extern "C" void CFRelease(int value) { (void)objc3_runtime_release_i32(value); }
extern "C" int CFAutorelease(int value) {
  return objc3_runtime_autorelease_i32(value);
}

int main() {
  objc3_runtime_reset_for_testing();

  const int result = helperSurface(77, 0);

  objc3_runtime_memory_management_state_snapshot memory_state{};
  objc3_runtime_arc_debug_state_snapshot arc_state{};
  const int memory_status =
      objc3_runtime_copy_memory_management_state_for_testing(&memory_state);
  const int arc_status =
      objc3_runtime_copy_arc_debug_state_for_testing(&arc_state);

  std::printf("result=%d\n", result);
  std::printf("close_fd_count=%d\n", g_close_fd_count);
  std::printf("last_close_fd_value=%d\n", g_last_close_fd_value);
  std::printf("release_temp_count=%d\n", g_release_temp_count);
  std::printf("last_release_temp_value=%d\n", g_last_release_temp_value);
  std::printf("memory_status=%d\n", memory_status);
  std::printf("arc_status=%d\n", arc_status);
  std::printf("autoreleasepool_depth=%llu\n",
              static_cast<unsigned long long>(memory_state.autoreleasepool_depth));
  std::printf("drained_autorelease_value_count=%llu\n",
              static_cast<unsigned long long>(memory_state.drained_autorelease_value_count));
  std::printf("last_drained_autorelease_value=%d\n",
              memory_state.last_drained_autorelease_value);
  std::printf("retain_call_count=%llu\n",
              static_cast<unsigned long long>(arc_state.retain_call_count));
  std::printf("release_call_count=%llu\n",
              static_cast<unsigned long long>(arc_state.release_call_count));
  std::printf("autorelease_call_count=%llu\n",
              static_cast<unsigned long long>(arc_state.autorelease_call_count));
  std::printf("autoreleasepool_push_count=%llu\n",
              static_cast<unsigned long long>(arc_state.autoreleasepool_push_count));
  std::printf("autoreleasepool_pop_count=%llu\n",
              static_cast<unsigned long long>(arc_state.autoreleasepool_pop_count));
  std::printf("last_retain_value=%d\n", arc_state.last_retain_value);
  std::printf("last_release_value=%d\n", arc_state.last_release_value);
  std::printf("last_autorelease_value=%d\n", arc_state.last_autorelease_value);

  if (result != 77 || g_close_fd_count != 1 || g_last_close_fd_value != 4 ||
      g_release_temp_count != 1 || g_last_release_temp_value != 1 ||
      memory_status != OBJC3_RUNTIME_REGISTRATION_STATUS_OK ||
      arc_status != OBJC3_RUNTIME_REGISTRATION_STATUS_OK ||
      memory_state.autoreleasepool_depth != 0 ||
      memory_state.drained_autorelease_value_count != 1 ||
      memory_state.last_drained_autorelease_value != 77 ||
      arc_state.retain_call_count != 1 || arc_state.release_call_count != 1 ||
      arc_state.autorelease_call_count != 1 ||
      arc_state.autoreleasepool_push_count != 1 ||
      arc_state.autoreleasepool_pop_count != 1 ||
      arc_state.last_retain_value != 77 || arc_state.last_release_value != 77 ||
      arc_state.last_autorelease_value != 77) {
    return 1;
  }
  return 0;
}
