// Sustained ARC/MRC ownership of Objective-C objects captured by copied Blocks.
// Counters describe observable contracts and explicit alias operations, never
// optimizer-dependent compiler-generated retain/release counts.
#include <objc/runtime.h>
#include <objc/blocks_runtime.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <thread>

static unsigned long long checksum, operations, allocations, destructions, live_values;
static unsigned long long retained_aliases, released_aliases, block_copies, block_invocations, weak_zeroed;
static void require(bool condition) { if (!condition) std::exit(4); }
extern "C" void *bench_retain(void *) __asm__("objc_retain");
extern "C" void bench_release(void *) __asm__("objc_release");

__attribute__((objc_root_class))
@interface BenchValue {
  Class isa;
@public
  unsigned value;
}
+ (instancetype)newWithValue:(unsigned)value;
- (void)dealloc;
@end
@implementation BenchValue
+ (instancetype)newWithValue:(unsigned)number {
  BenchValue *result = class_createInstance(self, 0);
  require(result != nullptr);result->value = number;
  ++allocations;++live_values;++operations;return result;
}
- (void)dealloc {
  require(live_values != 0);--live_values;++destructions;++operations;
  object_dispose(self);
}
@end

typedef void (^Action)(void);
static void work(unsigned size, unsigned seed) {
  for (unsigned i = 0; i < size; ++i) {
    __weak BenchValue *observer = nullptr;
    @autoreleasepool {
      BenchValue *object = [BenchValue newWithValue:i + seed];
      observer = object;
#if __has_feature(objc_arc)
      void *alias = bench_retain((__bridge void *)object);
#else
      void *alias = bench_retain(object);
#endif
      ++retained_aliases;++operations;
      bench_release(alias);++released_aliases;++operations;
#if __has_feature(objc_arc)
      Action action = (__bridge_transfer Action)_Block_copy((__bridge const void *)^{
        checksum += object->value;++block_invocations;++operations;
      });
#else
      Action action = (Action)_Block_copy(^{
        checksum += object->value;++block_invocations;++operations;
      });
#endif
      ++block_copies;++operations;
#if !__has_feature(objc_arc)
      bench_release(object);
#endif
      object = nullptr;
      require(observer != nullptr);
      action();
#if !__has_feature(objc_arc)
      _Block_release(action);
#endif
      action = nullptr;++operations;
    }
    require(observer == nullptr);++weak_zeroed;++operations;
    require(live_values == 0);
  }
}
static unsigned argument(const char *text, unsigned minimum, unsigned maximum) {
  char *end = nullptr;const auto value = std::strtoul(text, &end, 10);
  if (!*text || *end || value < minimum || value > maximum) std::exit(2);
  return static_cast<unsigned>(value);
}
int main(int argc, char **argv) {
  if (argc != 4 && argc != 5) return 2;
  const unsigned size = argument(argv[1], 1, 2048), rounds = argument(argv[2], 1, 128);
  const unsigned seed = argument(argv[3], 0, 1024), delay = argc == 5 ? argument(argv[4], 0, 1) : 0;
  const auto start = std::chrono::steady_clock::now();
  for (unsigned round = 0; round < rounds; ++round) {
    work(size, seed);
    if (delay) std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  const auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::steady_clock::now()-start).count();
  std::printf("{\"checksum\":%llu,\"completed_operations\":%llu,\"nanoseconds\":%lld,"
    "\"payload_copies\":0,\"destructions\":%llu,\"resumes\":0,\"callbacks\":0,\"live_values\":%llu,"
    "\"allocations\":%llu,\"retained_aliases\":%llu,\"released_aliases\":%llu,"
    "\"block_copies\":%llu,\"block_invocations\":%llu,\"weak_zeroed\":%llu}\n",
    checksum, operations, static_cast<long long>(nanos), destructions, live_values,
    allocations, retained_aliases, released_aliases, block_copies, block_invocations, weak_zeroed);
}
