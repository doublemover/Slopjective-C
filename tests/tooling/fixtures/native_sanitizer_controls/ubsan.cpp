#include <cstdlib>
__attribute__((noinline)) int objc3_ubsan_detector(int value) {
#ifndef OBJC3_DETECTOR_BAD
  value %= 1024;
#endif
  return value + 1;
}
int main(int argc, char **argv) {
  if (argc != 2) return 2;
  volatile int result = objc3_ubsan_detector(std::atoi(argv[1]));
  (void)result;
  return 0;
}
