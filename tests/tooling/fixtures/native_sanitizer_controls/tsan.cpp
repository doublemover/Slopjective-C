#include <atomic>
#include <thread>
std::atomic<bool> start{false};
#ifdef OBJC3_DETECTOR_BAD
volatile int shared_value = 0;
#else
std::atomic<int> shared_value{0};
#endif
__attribute__((noinline)) void objc3_tsan_detector() {
  while (!start.load(std::memory_order_acquire)) std::this_thread::yield();
  for (int i = 0; i != 10000; ++i) {
#ifdef OBJC3_DETECTOR_BAD
    shared_value = shared_value + 1;
#else
    shared_value.fetch_add(1, std::memory_order_relaxed);
#endif
  }
}
int main() {
  std::thread first(objc3_tsan_detector), second(objc3_tsan_detector);
  start.store(true, std::memory_order_release);
  first.join(); second.join();
  return 0;
}
