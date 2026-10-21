#import <objc/runtime.h>

#include "runtime/platform/objc3_native_object_adapter.h"

static int destroyed;

__attribute__((objc_root_class))
@interface H3WeakRoot { Class isa; }
+ (instancetype)new;
- (void)dealloc;
@end

@implementation H3WeakRoot
+ (instancetype)new { return class_createInstance(self, 0); }
- (void)dealloc { ++destroyed; object_dispose(self); }
@end

int main() {
  objc3_native_weak_slot slot{};
  H3WeakRoot *owner = [H3WeakRoot new];
  objc3_native_weak_init(&slot, (__bridge void *)owner);
  void *borrowed = objc3_native_weak_store(&slot, (__bridge void *)owner);
  if (borrowed != (__bridge void *)owner) return 1;
  void *loaded = objc3_native_weak_load_retained(&slot);
  if (loaded == nullptr) return 2;
  objc3_native_object_release(loaded);
  owner = nil;
  if (objc3_native_weak_load_retained(&slot) != nullptr || destroyed != 1)
    return 3;
  objc3_native_weak_destroy(&slot);
  return 0;
}
