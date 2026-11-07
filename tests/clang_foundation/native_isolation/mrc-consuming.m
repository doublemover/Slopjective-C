#include "provider.h"
#include <assert.h>
#include <objc/runtime.h>
#include <objc/objc-arc.h>
#include <objc/blocks_runtime.h>
#if __has_feature(objc_arc)
#error "MRC boundary witness must compile without ARC"
#endif

static int mrc_arguments, mrc_entries;
static _Thread_local int mrc_fail;
extern void hop_fail_next_frame_allocation(void);
extern void hop_fail_second_frame_allocation(void);
void hop_mrc_reset(int fail) {
  __atomic_store_n(&mrc_arguments, 0, __ATOMIC_RELEASE);
  __atomic_store_n(&mrc_entries, 0, __ATOMIC_RELEASE);
  mrc_fail = fail;
}
int hop_mrc_arguments(void) {return __atomic_load_n(&mrc_arguments, __ATOMIC_ACQUIRE);}
int hop_mrc_entries(void) {return __atomic_load_n(&mrc_entries, __ATOMIC_ACQUIRE);}
void hop_mrc_enter(void) {
  objc3_executor_t *main = 0;
  assert(objc3_executor_get_main(&main) == OBJC3_SERVICE_OK);
  hop_mrc_enter_on(main);
  objc3_executor_release(main);
}
void hop_mrc_enter_on(objc3_executor_t *destination) {
  assert(objc3_executor_is_current(destination));
  __atomic_fetch_add(&mrc_entries, 1, __ATOMIC_RELEASE);
}
HopObject *hop_mrc_source(objc3_executor_t *caller)
    __attribute__((nothrow, ns_returns_retained));
HopObject *hop_mrc_source(objc3_executor_t *caller) {
  assert(objc3_executor_is_current(caller));
  HopObject *object = [HopObject new];
  assert(object);
  __atomic_fetch_add(&mrc_arguments, 1, __ATOMIC_RELEASE);
  if (mrc_fail) {
    int fail = mrc_fail;
    mrc_fail = 0;
    if (fail == 2) hop_fail_second_frame_allocation();
    else hop_fail_next_frame_allocation();
  }
  return object;
}
static int mrc_consume(__attribute__((ns_consumed)) HopObject *object)
    __attribute__((nothrow, objc_executor(main)));
static int mrc_consume(__attribute__((ns_consumed)) HopObject *object) {
  hop_mrc_enter();
  int value = [object number];
  objc_release(object); // Native MRC callee balances its incoming claim.
  return value;
}
typedef int (^MRCWork)(void) __attribute__((objc_unsafe_sendable));
static int mrc_consume_work(__attribute__((ns_consumed)) MRCWork work)
    __attribute__((nothrow, objc_executor(main)));
static int mrc_consume_work(__attribute__((ns_consumed)) MRCWork work) {
  hop_mrc_enter();
  int value = work();
  _Block_release(work);
  return value;
}
static int mrc_consume_frame(__attribute__((ns_consumed)) HopObject *object)
    __objc3_async_frame __attribute__((nothrow, objc_executor(main)));
static int mrc_consume_frame(__attribute__((ns_consumed)) HopObject *object)
    __objc3_async_frame {
  hop_mrc_enter();
  int value = [object number];
  objc_release(object);
  return value;
}
static int mrc_consume_work_frame(__attribute__((ns_consumed)) MRCWork work)
    __objc3_async_frame __attribute__((nothrow, objc_executor(main)));
static int mrc_consume_work_frame(__attribute__((ns_consumed)) MRCWork work)
    __objc3_async_frame {
  hop_mrc_enter();
  int value = work();
  _Block_release(work);
  return value;
}
@interface HopObject (MRCConsumption)
- (int)mrcConsumeSelf __attribute__((nothrow, ns_consumes_self, objc_executor(main)));
- (int)mrcConsumeObject:(__attribute__((ns_consumed)) HopObject *)other
    __attribute__((nothrow, ns_consumes_self, objc_executor(main)));
+ (int)mrcConsumeClassObject:(__attribute__((ns_consumed)) HopObject *)other
    __attribute__((nothrow, objc_executor(main)));
- (instancetype)initOnMain:(int)kind __attribute__((nothrow, objc_executor(main)));
- (int)mrcConsumeSelfFrame __objc3_async_frame
    __attribute__((nothrow, ns_consumes_self, objc_executor(main)));
+ (int)mrcConsumeClassFrame:(__attribute__((ns_consumed)) HopObject *)other
    __objc3_async_frame __attribute__((nothrow, objc_executor(main)));
- (int)mrcConsumeBothFrame:(__attribute__((ns_consumed)) HopObject *)other
    __objc3_async_frame __attribute__((nothrow, ns_consumes_self, objc_executor(main)));
@end
@implementation HopObject (MRCConsumption)
- (int)mrcConsumeSelf {
  hop_mrc_enter();
  int value = [self number];
  objc_release(self);
  return value;
}
- (int)mrcConsumeObject:(__attribute__((ns_consumed)) HopObject *)other {
  hop_mrc_enter();
  int value = [self number] + [other number] - 42;
  objc_release(other);
  objc_release(self);
  return value;
}
+ (int)mrcConsumeClassObject:(__attribute__((ns_consumed)) HopObject *)other {
  hop_mrc_enter();
  int value = [other number];
  objc_release(other);
  return value;
}
- (instancetype)initOnMain:(int)kind {
  hop_mrc_enter();
  if (kind == 1) {objc_release(self); return 0;}
  if (kind == 2) {
    HopObject *replacement = [HopObject new];
    objc_release(self);
    return replacement;
  }
  return self;
}
- (int)mrcConsumeSelfFrame __objc3_async_frame {
  hop_mrc_enter();
  int value = [self number];
  objc_release(self);
  return value;
}
+ (int)mrcConsumeClassFrame:(__attribute__((ns_consumed)) HopObject *)other
    __objc3_async_frame {
  hop_mrc_enter();
  int value = [other number];
  objc_release(other);
  return value;
}
- (int)mrcConsumeBothFrame:(__attribute__((ns_consumed)) HopObject *)other
    __objc3_async_frame {
  hop_mrc_enter();
  int value = [self number] + [other number] - 42;
  objc_release(self);
  objc_release(other);
  return value;
}
@end

int hop_mrc_roundtrip(int kind, objc3_executor_t *caller) __objc3_async_frame {
  int result = 0;
  switch (kind) {
  case 0: result = __objc3_await mrc_consume(hop_mrc_source(caller)); break;
  case 1: result = __objc3_await [hop_mrc_source(caller) mrcConsumeSelf]; break;
  case 2: {
    HopObject *object = hop_mrc_source(caller);
    MRCWork work = (MRCWork)_Block_copy(^{return [object number];});
    objc_release(object); // The copied block owns its captured reference.
    result = __objc3_await mrc_consume_work(work);
    break;
  }
  case 3: result = __objc3_await [HopObject mrcConsumeClassObject:hop_mrc_source(caller)]; break;
  case 4: case 5: case 6: {
    // Claim the +0 await value within the same expression, before its temporary
    // result owner is released. The native initializer transfers its own +1.
    HopObject *object = objc_retain(__objc3_await [hop_mrc_source(caller) initOnMain:kind-4]);
    result = object ? [object number] : 0;
    objc_release(object);
    break;
  }
  case 7:
    result = __objc3_await [hop_mrc_source(caller) mrcConsumeObject:hop_mrc_source(caller)];
    break;
  case 8: result = __objc3_await mrc_consume_frame(hop_mrc_source(caller)); break;
  case 9: result = __objc3_await [hop_mrc_source(caller) mrcConsumeSelfFrame]; break;
  case 10: {
    HopObject *object = hop_mrc_source(caller);
    MRCWork work = (MRCWork)_Block_copy(^{return [object number];});
    objc_release(object);
    result = __objc3_await mrc_consume_work_frame(work);
    break;
  }
  case 11: result = __objc3_await [HopObject mrcConsumeClassFrame:hop_mrc_source(caller)]; break;
  case 12:
    result = __objc3_await [hop_mrc_source(caller) mrcConsumeBothFrame:hop_mrc_source(caller)];
    break;
  default: assert(0);
  }
  assert(objc3_executor_is_current(caller));
  return result;
}
