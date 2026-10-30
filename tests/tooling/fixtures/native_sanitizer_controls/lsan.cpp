#include <cstdlib>
#include <pthread.h>
__attribute__((noinline)) void *objc3_lsan_detector(void *) {
  void *allocation = std::malloc(73);
  if (!allocation) std::abort();
  asm volatile("" : : "r"(allocation) : "memory");
#ifndef OBJC3_DETECTOR_BAD
  std::free(allocation);
#endif
  return nullptr;
}
int main() {
  pthread_t worker;
  if (pthread_create(&worker, nullptr, objc3_lsan_detector, nullptr)) return 2;
  if (pthread_join(worker, nullptr)) return 2;
  return 0;
}
