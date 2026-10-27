#include "provider.h"
#include <assert.h>
#include <stddef.h>
#if !__has_feature(objc_arc)
#error "Consuming isolation requires ARC adoption"
#endif

static int consumed_arguments, consumed_entries;
static _Thread_local int fail_after_source, fail_frame_allocation;
extern int posix_memalign(void **, size_t, size_t);
void *aligned_alloc(size_t alignment, size_t size) {
  if (fail_frame_allocation) {
    fail_frame_allocation=0;
    return 0;
  }
  void *memory=0;
  return posix_memalign(&memory,alignment,size)==0 ? memory : 0;
}
void hop_fail_next_frame_allocation(void) { fail_frame_allocation=1; }
void hop_consumed_reset(int fail_factory) {
  __atomic_store_n(&consumed_arguments,0,__ATOMIC_RELEASE);
  __atomic_store_n(&consumed_entries,0,__ATOMIC_RELEASE);
  fail_after_source=fail_factory;
  fail_frame_allocation=0;
}
int hop_consumed_arguments(void) {
  return __atomic_load_n(&consumed_arguments,__ATOMIC_ACQUIRE);
}
int hop_consumed_entries(void) {
  return __atomic_load_n(&consumed_entries,__ATOMIC_ACQUIRE);
}
static void consumed_on_main(void) {
  objc3_executor_t *destination=0;
  assert(objc3_executor_get_main(&destination)==OBJC3_SERVICE_OK);
  assert(objc3_executor_is_current(destination));
  objc3_executor_release(destination);
  __atomic_fetch_add(&consumed_entries,1,__ATOMIC_RELEASE);
}
static HopObject *consumed_source(objc3_executor_t *caller)
    __attribute__((nothrow, ns_returns_retained));
static HopObject *consumed_source(objc3_executor_t *caller) {
  assert(objc3_executor_is_current(caller));
  HopObject *value=[HopObject new];
  assert(value);
  __atomic_fetch_add(&consumed_arguments,1,__ATOMIC_RELEASE);
  if (fail_after_source) {
    fail_after_source=0;
    fail_frame_allocation=1;
  }
  return value;
}
static int consume_object(__attribute__((ns_consumed)) HopObject *value)
    __attribute__((nothrow, objc_executor(main)));
static int consume_object(__attribute__((ns_consumed)) HopObject *value) {
  consumed_on_main();
  return [value number];
}
typedef int (^ConsumedWork)(void) __attribute__((objc_unsafe_sendable));
static int consume_work(__attribute__((ns_consumed)) ConsumedWork work)
    __attribute__((nothrow, objc_executor(main)));
static int consume_work(__attribute__((ns_consumed)) ConsumedWork work) {
  consumed_on_main();
  return work();
}
static int consume_frame(__attribute__((ns_consumed)) HopObject *value)
    __objc3_async_frame __attribute__((nothrow, objc_executor(main)));
static int consume_frame(__attribute__((ns_consumed)) HopObject *value) __objc3_async_frame {
  consumed_on_main();
  return [value number];
}
static void consume_void(__attribute__((ns_consumed)) HopObject *value)
    __attribute__((nothrow, objc_executor(main)));
static void consume_void(__attribute__((ns_consumed)) HopObject *value) {
  consumed_on_main();
  assert([value number]==42);
}
static int drop_sender(HopObject *__strong *slot) {
  *slot=0;
  return 0;
}

@interface HopObject (Consumption)
+ (int)consumeClassObject:(__attribute__((ns_consumed)) HopObject *)value
    __attribute__((nothrow, objc_executor(main)));
+ (int)consumeClassWork:(__attribute__((ns_consumed)) ConsumedWork)work
    __attribute__((nothrow, objc_executor(main)));
- (int)consumeAfter:(int)delta
    __attribute__((ns_consumes_self, nothrow, objc_executor(main)));
- (int)consumeObject:(__attribute__((ns_consumed)) HopObject *)value after:(int)delta
    __attribute__((ns_consumes_self, nothrow, objc_executor(main)));
- (int)readAfter:(int)delta __attribute__((nothrow, objc_executor(main)));
- (int)consumeSelf __attribute__((ns_consumes_self, nothrow, objc_executor(main)));
- (int)consumeBoth:(__attribute__((ns_consumed)) HopObject *)value
    __attribute__((ns_consumes_self, nothrow, objc_executor(main)));
- (HopObject *)consumeAndReturn
    __attribute__((ns_consumes_self, ns_returns_retained, nothrow, objc_executor(main)));
@end
@implementation HopObject (Consumption)
+ (int)consumeClassObject:(__attribute__((ns_consumed)) HopObject *)value {
  consumed_on_main();
  return [value number];
}
+ (int)consumeClassWork:(__attribute__((ns_consumed)) ConsumedWork)work {
  consumed_on_main();
  return work();
}
- (int)consumeAfter:(int)delta {
  consumed_on_main();
  return [self number]+delta;
}
- (int)consumeObject:(__attribute__((ns_consumed)) HopObject *)value after:(int)delta {
  consumed_on_main();
  return [self number]+[value number]-42+delta;
}
- (int)readAfter:(int)delta {
  consumed_on_main();
  return [self number]+delta;
}
- (int)consumeSelf {
  consumed_on_main();
  return [self number];
}
- (int)consumeBoth:(__attribute__((ns_consumed)) HopObject *)value {
  consumed_on_main();
  return [self number]+[value number]-42;
}
- (HopObject *)consumeAndReturn {
  consumed_on_main();
  return self;
}
@end

int hop_consumed_roundtrip(int kind, objc3_executor_t *caller)
    __objc3_async_frame {
  int result=0;
  switch (kind) {
  case 0: result=__objc3_await consume_object(consumed_source(caller)); break;
  case 1: result=__objc3_await [consumed_source(caller) consumeSelf]; break;
  case 2: {
    HopObject *object=consumed_source(caller);
    ConsumedWork work=^{return [object number];};
    result=__objc3_await consume_work(work);
    break;
  }
  case 3: result=__objc3_await consume_frame(consumed_source(caller)); break;
  case 4:
    result=__objc3_await [consumed_source(caller) consumeBoth:consumed_source(caller)];
    break;
  case 5: {
    HopObject *value=__objc3_await [consumed_source(caller) consumeAndReturn];
    result=[value number];
    break;
  }
  case 6:
    __objc3_await consume_void(consumed_source(caller));
    result=42;
    break;
  case 7: {
    HopObject *receiver=consumed_source(caller);
    result=__objc3_await [receiver consumeAfter:drop_sender(&receiver)];
    assert(!receiver);
    break;
  }
  case 8: {
    HopObject *argument=consumed_source(caller);
    result=__objc3_await [consumed_source(caller) consumeObject:argument
                                                      after:drop_sender(&argument)];
    assert(!argument);
    break;
  }
  case 9: {
    HopObject *receiver=consumed_source(caller);
    result=__objc3_await [receiver readAfter:drop_sender(&receiver)];
    assert(!receiver);
    break;
  }
  case 10:
    result=__objc3_await [HopObject consumeClassObject:consumed_source(caller)];
    break;
  case 11: {
    HopObject *object=consumed_source(caller);
    ConsumedWork work=^{return [object number];};
    result=__objc3_await [HopObject consumeClassWork:work];
    break;
  }
  case 12: {
    HopObject *receiver=consumed_source(caller);
    result=__objc3_await [receiver consumeAfter:hop_throwing_argument()];
    break;
  }
  default: assert(!"unknown consuming isolation case");
  }
  assert(objc3_executor_is_current(caller));
  return result;
}

@interface HopSuperBase (SuperConsumption)
- (int)consumeSuper:(int)value
    __attribute__((ns_consumes_self, nothrow, objc_executor(main)));
@end
@implementation HopSuperBase (SuperConsumption)
- (int)consumeSuper:(int)value {
  hop_super_enter_main();
  return [self number]+value-1;
}
@end
@interface HopSuperChild (SuperConsumption)
- (int)consumedSuperRoundtrip:(objc3_executor_t *)caller __objc3_async_frame;
@end
@implementation HopSuperChild (SuperConsumption)
- (int)consumedSuperRoundtrip:(objc3_executor_t *)caller __objc3_async_frame {
  int value=__objc3_await [super consumeSuper:hop_super_argument(caller)];
  assert(objc3_executor_is_current(caller));
  return value;
}
@end
objc3_async_frame hop_super_consumed_frame(objc3_executor_t *caller) {
  HopSuperChild *receiver=[HopSuperChild new];
  return [receiver consumedSuperRoundtrip:caller];
}
