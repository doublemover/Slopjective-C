#include "carrier-throws-call.h"
#include <objc/runtime.h>
static int created, destroyed;
__attribute__((objc_root_class))
@interface TransportError <Error> { Class isa; }
+ (id)new;
@end
@implementation TransportError
+ (id)new { ++created; return class_createInstance(self, 0); }
- (void)dealloc { ++destroyed; object_dispose(self); }
@end
int throws_created(void) { return created; }
int throws_destroyed(void) { return destroyed; }
static void fail_now(void) throws(id<Error>) {
  // The transport must survive this pool and every caller cleanup.
  @autoreleasepool { throwError((id<Error>)[TransportError new]); }
}
CallStrong throws_value(CallStrong value, int key, int fail) throws(id<Error>) {
  if (fail) {
    value = call_make(key);
    tryValue(fail_now());
  }
  return value;
}
CallStrong throws_forward(CallStrong value, int key, int fail) throws(id<Error>) {
  @autoreleasepool { return tryValue(throws_value(value, key, fail)); }
}
CallWeak throws_weak(CallWeak value, int fail) throws(id<Error>) {
  if (fail) tryValue(fail_now());
  return value;
}
CallUnsafe throws_unsafe(CallUnsafe value) throws(id<Error>) { return value; }
CallOutcome throws_outcome(CallOutcome value, int fail) throws(id<Error>) {
  if (fail) tryValue(fail_now());
  return value;
}
CallMixed throws_mixed(CallMixed value, int fail) throws(id<Error>) {
  if (fail) tryValue(fail_now());
  return value;
}
CallBlock throws_payload_block(CallBlock value, int fail) throws(id<Error>) {
  if (fail) tryValue(fail_now());
  return value;
}
int throws_scalar(CallStrong value, int key, int fail) throws(id<Error>) {
  if (fail) { value = call_make(key); tryValue(fail_now()); }
  return hasValue(value);
}
ThrowsBlock throws_make_block(id capture) {
  return ^CallStrong(CallStrong value, int key, int fail) throws(id<Error>) {
    if (!capture) __builtin_trap();
    return tryValue(throws_value(value, key, fail));
  };
}
CallStrong throws_many(CallStrong a, CallStrong b, CallStrong c, CallStrong d,
                       CallStrong e, CallStrong f, CallStrong g, int fail)
    throws(id<Error>) {
  if (!hasValue(a) || !hasValue(b) || !hasValue(c) ||
      !hasValue(d) || !hasValue(e) || !hasValue(f)) __builtin_trap();
  if (fail) tryValue(fail_now());
  return g;
}
@implementation ThrowsCalls
+ (id)new { return class_createInstance(self, 0); }
+ (CallStrong)make:(int)key fail:(int)fail throws(id<Error>) {
  return tryValue(throws_value(call_make(key), key + 1, fail));
}
- (CallStrong)value:(CallStrong)value key:(int)key fail:(int)fail throws(id<Error>) {
  return tryValue(throws_forward(value, key, fail));
}
- (CallStrong)alternate:(CallStrong)value key:(int)key fail:(int)fail throws(id<Error>) {
  return tryValue(throws_value(value, key, fail));
}
- (CallWeak)weak:(CallWeak)value fail:(int)fail throws(id<Error>) {
  return tryValue(throws_weak(value, fail));
}
- (CallStrong)native:(CallStrong)value throws(id<Error>) { @throw [TransportError new]; }
- (void)dealloc { object_dispose(self); }
@end
@implementation ThrowsSubclass
- (CallStrong)value:(CallStrong)value key:(int)key fail:(int)fail throws(id<Error>) {
  return tryValue([super value:value key:key fail:fail]);
}
@end
#ifdef __cplusplus
CallStrong ThrowsDerived::apply(CallStrong value, int key, int fail) throws(id<Error>) {
  return tryValue(throws_value(value, key, fail));
}
CallStrong ThrowsDerived::make(CallStrong value, int key, int fail) throws(id<Error>) {
  return tryValue(throws_value(value, key, fail));
}
struct ThrowOnExit {
  ~ThrowOnExit() noexcept(false) { throw 71; }
};
CallStrong throws_return_cleanup(int key, int mode) throws(id<Error>) {
  ThrowOnExit cleanup;
  if (mode) tryValue(fail_now());
  return call_make(key);
}
CallWeak throws_weak_return_cleanup(id object) {
  ThrowOnExit cleanup;
  return Some(id __weak, object);
}
#endif
