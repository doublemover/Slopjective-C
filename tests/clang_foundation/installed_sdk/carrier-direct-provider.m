#include "carrier-direct-call.h"
#include <objc/runtime.h>
static int initialized;
int direct_initialized(void) { return __atomic_load_n(&initialized, __ATOMIC_SEQ_CST); }
@implementation DirectCalls
+ (void)initialize { __atomic_fetch_add(&initialized, 1, __ATOMIC_SEQ_CST); }
+ (id)new { return class_createInstance(self, 0); }
+ (CallStrong)echo:(CallStrong)value fail:(int)fail __objc3_throws(id<Error>) {
  return __objc3_try(throws_forward(value, 1, fail));
}
- (CallStrong)echo:(CallStrong)value key:(int)key fail:(int)fail __objc3_throws(id<Error>) {
  return __objc3_try(throws_forward(value, key, fail));
}
- (CallWeak)weak:(CallWeak)value fail:(int)fail __objc3_throws(id<Error>) {
  return __objc3_try(throws_weak(value, fail));
}
- (CallMixed)mixed:(CallMixed)value fail:(int)fail __objc3_throws(id<Error>) {
  return __objc3_try(throws_mixed(value, fail));
}
- (CallBlock)block:(CallBlock)value fail:(int)fail __objc3_throws(id<Error>) {
  return __objc3_try(throws_payload_block(value, fail));
}
- (int)command { return sel_isEqual(_cmd, sel_registerName("command")); }
- (CallStrong)native:(CallStrong)value __objc3_throws(id<Error>) {
  value = call_make(11);
  @throw call_object(12);
}
#ifdef __cplusplus
- (CallStrong)cleanup:(int)key __objc3_throws(id<Error>) {
  return __objc3_try(throws_return_cleanup(key, 0));
}
#endif
- (void)dealloc { object_dispose(self); }
@end
