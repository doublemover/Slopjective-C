#include <objc3_darwin.h>
extern "C" int puts(const char *message);

static int destroyed;
static int unwound;

__attribute__((objc_root_class))
@interface ARCUnwindWitness
- (int)answer;
@end

@implementation ARCUnwindWitness
- (int)answer { return 42; }
- (void)dealloc { ++destroyed; }
@end

struct ScopedCleanup {
  ~ScopedCleanup() { ++unwound; }
};

extern "C" int darwin_cpp_boundary(int payload, bool raise);

static int call_boundary(bool raise, ARCUnwindWitness *__weak &observer) {
  ScopedCleanup cleanup;
  @autoreleasepool {
    const objc3_darwin_class *cls = objc3_darwin_class_v1("ARCUnwindWitness");
    if (!cls)
      return -1;
    __attribute__((objc_precise_lifetime)) ARCUnwindWitness *owner =
        (__bridge_transfer ARCUnwindWitness *)objc3_darwin_allocate_v1(cls);
    observer = owner;
    int (^read)(void) = ^{ return [owner answer]; };
    return darwin_cpp_boundary(read(), raise);
  }
}

int main() {
  ARCUnwindWitness *__weak observer = nullptr;
  if (call_boundary(false, observer) != 42 ||
      observer || destroyed != 1 || unwound != 1)
    return 1;
  try {
    (void)call_boundary(true, observer);
    return 2;
  } catch (int payload) {
    if (payload != 42 || observer || destroyed != 2 || unwound != 2)
      return 3;
  }
  puts("owned Objective-C++ Darwin ARC and C++ unwinding: PASS");
  return 0;
}
