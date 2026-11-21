#include "async_api.h"
#include <objc/runtime.h>
#include <objc3c/runtime/objc3_async_c.h>
#include <stdbool.h>

@protocol Error @end

__attribute__((objc_root_class))
@interface SwiftBridgeRoot {
  Class isa;
}
+ (id)new;
- (void)dealloc;
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

static _Atomic(int) retired_sync_errors;

@interface SwiftSyncFailure : SwiftBridgeRoot <Error>
@end
@implementation SwiftSyncFailure
- (void)dealloc {
  __c11_atomic_fetch_add(&retired_sync_errors, 1, __ATOMIC_RELAXED);
#if !__has_feature(objc_arc)
  [super dealloc];
#endif
}
@end

#if !__has_feature(objc_arc)
extern id objc_autorelease(id);
extern void objc_release(id);
#endif

static int swift_bridge_checked(int value) throws(id<Error>) {
  if (value < 0) {
    id<Error> error = [SwiftSyncFailure new];
#if !__has_feature(objc_arc)
    error = objc_autorelease(error);
#endif
    __objc3_throw(error);
  }
  return value + 1;
}

static void swift_bridge_checked_void(bool succeed) throws(id<Error>) {
  (void)__objc3_try(swift_bridge_checked(succeed ? 0 : -1));
}

static int increment(int value) async { return value + 1; }

static bool swift_bridge_scalars(bool enabled, char plain, signed char byte, unsigned char narrow,
                                 short small, unsigned short count, int integer, unsigned int flags,
                                 long offset, unsigned long length, long long minimum,
                                 unsigned long long maximum, float fraction, double real) async {
  return enabled && plain == 42 && byte == -8 && narrow == 250 && small == -11 && count == 60000 &&
      integer == -1 && flags == 7 && offset == -42 && length == 41 &&
      minimum == -9223372036854775807LL - 1 && maximum == 18446744073709551615ULL &&
      fraction == 1.25f && real == 0.5;
}

static void swift_bridge_void(void) async {}

enum SwiftBridgeChoice : signed char { SwiftBridgeAbsent = -1, SwiftBridgeAnswer = 42 };
static enum SwiftBridgeChoice swift_bridge_enum(enum SwiftBridgeChoice value) async {
  return value == SwiftBridgeAbsent ? SwiftBridgeAnswer : SwiftBridgeAbsent;
}

typedef float SwiftBridgeFloatVector __attribute__((ext_vector_type(4)));
typedef int SwiftBridgeIntVector __attribute__((vector_size(8)));

struct SwiftBridgeMetadata {
  enum SwiftBridgeChoice choice;
  bool accepted;
};
struct SwiftBridgeRecord {
  struct SwiftBridgeMetadata metadata;
  long values[4];
  SwiftBridgeIntVector projection;
};

static struct SwiftBridgeRecord swift_bridge_record(struct SwiftBridgeRecord value) async {
  value.metadata.choice = SwiftBridgeAnswer;
  value.metadata.accepted = true;
  value.values[0] += 43;
  value.projection += (SwiftBridgeIntVector){10, -10};
  return value;
}

static struct SwiftBridgeRecord swift_bridge_checked_record(struct SwiftBridgeRecord value)
    throws(id<Error>) {
  value.values[2] = __objc3_try(swift_bridge_checked((int)value.values[2]));
  return value;
}

static SwiftBridgeFloatVector swift_bridge_vector(SwiftBridgeFloatVector value) async {
  return value + (SwiftBridgeFloatVector){1, 2, 3, 4};
}

static SwiftBridgeIntVector swift_bridge_checked_vector(SwiftBridgeIntVector value)
    throws(id<Error>) {
  value[0] = __objc3_try(swift_bridge_checked(value[0]));
  value[1] *= 7;
  return value;
}

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
  if (!error) return 0;
  Class type = object_getClass((__bridge id)error);
  if (type == objc_getClass("SwiftBridgeFailure")) return 1;
  return type == objc_getClass("SwiftSyncFailure") ? 3 : 0;
}

int objc3_swift_sync_errors_retired(void) {
  return __c11_atomic_load(&retired_sync_errors, __ATOMIC_RELAXED);
}
