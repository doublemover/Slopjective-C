#include "provider.h"
#include <stddef.h>

// Link this separate Objective-C C17 translation unit into each client. Only
// the next native factory allocation on this thread is rejected; runtime task
// creation/publication subsequently uses the ordinary allocator path.
extern int posix_memalign(void **, size_t, size_t);
static _Thread_local int fail_native_frame_allocation;
void *aligned_alloc(size_t alignment, size_t size) {
  if (fail_native_frame_allocation) {
    fail_native_frame_allocation = 0;
    return 0;
  }
  void *allocation = 0;
  return posix_memalign(&allocation, alignment, size) == 0 ? allocation : 0;
}
objc3_async_frame native_c_force_oom(void) {
  fail_native_frame_allocation = 1;
  objc3_async_frame result = native_c_leaf(42, 0);
  fail_native_frame_allocation = 0;
  return result;
}
