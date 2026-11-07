#import <objc/runtime.h>
#include <assert.h>

extern void *claim_return(void *) __asm__("objc_retainAutoreleasedReturnValue");
extern void *enqueue_autorelease(void *) __asm__("objc_autorelease");
extern unsigned long objc_arc_autorelease_count_np(void);
static unsigned destroyed;

__attribute__((objc_root_class))
@interface NativePoolOwner { Class isa; }
+ (instancetype)new;
- (void)dealloc;
@end
@implementation NativePoolOwner
+ (instancetype)new { return class_createInstance(self, 0); }
- (void)dealloc { ++destroyed; object_dispose(self); }
@end

int main(void) {
  __weak NativePoolOwner *weak = nil;
  @autoreleasepool {
    // Loading an optional class must never reinterpret an already-live native
    // pool token. No selector is sent to this deliberately method-free class.
    assert(objc_getClass("NSAutoreleasePool") == Nil);
    Class optional = objc_allocateClassPair(Nil, "NSAutoreleasePool", 0);
    assert(optional);
    objc_registerClassPair(optional);
    @autoreleasepool {
      unsigned long before = objc_arc_autorelease_count_np();
      assert(claim_return(0) == 0);
      assert(objc_arc_autorelease_count_np() == before);
      NativePoolOwner *owner = [NativePoolOwner new];
      weak = owner;
      void *owned = (__bridge_retained void *)owner;
      owner = nil;
      enqueue_autorelease(owned);
      assert(weak && destroyed == 0);
    }
    assert(!weak && destroyed == 1);
  }
  assert(destroyed == 1);
}
