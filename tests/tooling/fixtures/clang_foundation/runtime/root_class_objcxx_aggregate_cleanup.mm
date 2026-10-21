#import <objc/runtime.h>

#include "runtime/platform/objc3_native_strong_ref.hpp"
#include <stdexcept>

using objc3::runtime::platform::StrongRef;

static int objectDestroyed;

__attribute__((objc_root_class))
@interface H3CppRoot { Class isa; }
+ (instancetype)new;
- (void)dealloc;
@end

@implementation H3CppRoot
+ (instancetype)new { return class_createInstance(self, 0); }
- (void)dealloc { ++objectDestroyed; object_dispose(self); }
@end

struct OwnedAggregate {
  StrongRef object;
  int *cleanupCount;
  ~OwnedAggregate() { ++*cleanupCount; }
};

static void ThrowAcrossAggregate(objc3_native_object_t object,
                                 int *cleanupCount) {
  OwnedAggregate aggregate{StrongRef(object), cleanupCount};
  throw std::runtime_error("exercise ObjC++ aggregate cleanup");
}

int main() {
  int cleanupCount = 0;
  H3CppRoot *object = [H3CppRoot new];
  {
    StrongRef owner((__bridge void *)object);
    object = nil;
    try {
      ThrowAcrossAggregate(owner.get(), &cleanupCount);
    } catch (const std::runtime_error &) {
    }
    if (cleanupCount != 1 || objectDestroyed != 0) return 1;
  }
  return objectDestroyed == 1 ? 0 : 2;
}
