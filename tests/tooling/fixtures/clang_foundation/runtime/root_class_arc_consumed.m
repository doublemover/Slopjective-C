#import <objc/runtime.h>

__attribute__((objc_root_class))
@interface H3ConsumedRoot { Class isa; }
+ (instancetype)new;
- (int)value;
- (void)dealloc;
@end

@implementation H3ConsumedRoot
+ (instancetype)new { return class_createInstance(self, 0); }
- (int)value { return 19; }
- (void)dealloc { object_dispose(self); }
@end

/* ns_consumed transfers one +1 for the call, not the caller's binding. */
static void ObserveConsumed(H3ConsumedRoot * __attribute__((ns_consumed)) value) {
  (void)[value value];
}

int main(void) {
  H3ConsumedRoot *value = [H3ConsumedRoot new];
  ObserveConsumed(value);
  return [value value] == 19 ? 0 : 1;
}
