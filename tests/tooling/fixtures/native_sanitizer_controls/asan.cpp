#include <cstdlib>
__attribute__((noinline)) int objc3_asan_detector(int index) {
  auto *values = static_cast<int *>(std::calloc(16, sizeof(int)));
  if (!values) std::abort();
#ifndef OBJC3_DETECTOR_BAD
  index %= 16;
#endif
  volatile int result = values[index];
  std::free(values);
  return result;
}
int main(int argc, char **argv) {
  if (argc != 2) return 2;
  volatile int result = objc3_asan_detector(std::atoi(argv[1]));
  (void)result;
  return 0;
}
