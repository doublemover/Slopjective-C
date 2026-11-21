#include <stddef.h>

// Deterministically fail a body allocation, after the lazy frame has started.
// posix_memalign remains visible to the sanitizer allocator interceptors.
extern int posix_memalign(void **, size_t, size_t);
static _Thread_local int fail_next_storage_allocation;
void storage_fail_next_allocation(void) { fail_next_storage_allocation = 1; }
void storage_observe_allocation(void *pointer) {
  __asm__ volatile("" : : "r"(pointer) : "memory");
}
void *aligned_alloc(size_t alignment, size_t size) {
  if (fail_next_storage_allocation) {
    fail_next_storage_allocation = 0;
    return NULL;
  }
  void *memory = NULL;
  return posix_memalign(&memory, alignment, size) == 0 ? memory : NULL;
}
