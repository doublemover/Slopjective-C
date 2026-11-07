#include <stddef.h>
extern int posix_memalign(void **, size_t, size_t);
static int reject_frame_allocation, frame_allocations;
void nil_reject_frame_allocation(int reject) { reject_frame_allocation = reject; }
int nil_frame_allocations(void) { return frame_allocations; }
void *aligned_alloc(size_t alignment, size_t size) {
  ++frame_allocations;
  if (reject_frame_allocation) return NULL;
  void *memory = NULL;
  return posix_memalign(&memory, alignment, size) == 0 ? memory : NULL;
}
