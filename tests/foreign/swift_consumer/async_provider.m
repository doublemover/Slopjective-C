#include "async_api.h"
#include <objc/runtime.h>
#include <objc3c/runtime/objc3_async_c.h>

@protocol Error @end

__attribute__((objc_root_class))
@interface SwiftBridgeRoot {
  Class isa;
}
+ (id)new;
@end

@implementation SwiftBridgeRoot
+ (id)new { return class_createInstance(self, 0); }
- (void)dealloc { object_dispose(self); }
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
@end

@interface SwiftBridgeFailure : SwiftBridgeRoot <Error>
@end
@implementation SwiftBridgeFailure
@end

#if !__has_feature(objc_arc)
extern id objc_autorelease(id);
extern void objc_release(id);
#endif

static int increment(int value) async { return value + 1; }

static int swift_bridge_work(int value) async throws(id<Error>) {
  if (value < 0) {
    id<Error> error = [SwiftBridgeFailure new];
#if !__has_feature(objc_arc)
    error = objc_autorelease(error);
#endif
    __objc3_throw(error);
  }
  return await increment(value);
}

static void swift_bridge_cancellable(void) async throws(id<Error>) {
  for (;;) {
    try objc3_task_check_cancellation();
    (void)await increment(0);
  }
}

static __attribute__((nothrow, objc_executor(named("swift.no-registered-executor"))))
int swift_bridge_cancelled(void) async {
  __builtin_trap();
}

actor class SwiftBridgeCounter : SwiftBridgeRoot {
  int total;
}
- (int)add:(int)value __attribute__((nothrow));
@end

@implementation SwiftBridgeCounter
- (int)add:(int)value {
  objc3_current_actor_facts facts = {sizeof(facts), OBJC3_ACTOR_SERVICE_VERSION};
  objc3_actor_t *actor = 0;
  if (objc3_actor_current_facts(&facts) != OBJC3_SERVICE_OK ||
      objc3_actor_get_or_create_for_object((__bridge void *)self, 0, &actor) != OBJC3_SERVICE_OK)
    __builtin_trap();
  const uint64_t identity = objc3_actor_identity(actor);
  objc3_actor_release(actor);
  if (facts.actor_identity != identity) __builtin_trap();
  return total += value;
}
@end

void * _Nonnull objc3_swift_counter_create(void) {
  SwiftBridgeCounter *counter = [SwiftBridgeCounter new];
#if __has_feature(objc_arc)
  return (__bridge_retained void *)counter;
#else
  return (__bridge void *)counter;
#endif
}

void objc3_swift_counter_release(void * _Nonnull counter) {
#if __has_feature(objc_arc)
  (void)(__bridge_transfer id)counter;
#else
  objc_release((__bridge id)counter);
#endif
}

int objc3_swift_error_code(void * _Nullable error) {
  if (objc3_async_c_error_is_cancellation((__bridge id<Error>)error)) return 2;
  return error && object_getClass((__bridge id)error) == objc_getClass("SwiftBridgeFailure") ? 1 : 0;
}
