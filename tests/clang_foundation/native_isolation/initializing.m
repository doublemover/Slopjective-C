#include "cf-contract.h"
#include <assert.h>
#include <objc/runtime.h>
#include <objc/objc-arc.h>
#include <objc/blocks_runtime.h>
#if __has_feature(objc_arc)
#define Initialization ARCInitialization
#define initFrame initARCFrame
#define newFrame newARCFrame
#define hop_initializing_roundtrip hop_arc_initializing_roundtrip
#define hop_initializing_reset hop_arc_initializing_reset
#define hop_initializing_entries hop_arc_initializing_entries
#define hop_initializing_completed hop_arc_initializing_completed
#define hop_initializing_created hop_arc_initializing_created
#define hop_initializing_nil_roundtrip hop_arc_initializing_nil_roundtrip
#define initDirectFrame initARCDirectFrame
#define nilScalarFrame nilARCScalarFrame
#define nilVoidFrame nilARCVoidFrame
#define nilRecordFrame nilARCRecordFrame
#define nilForeignFrame nilARCForeignFrame
#define nilObjectFrame nilARCObjectFrame
#define nilDoubleFrame nilARCDoubleFrame
#define nilBlockFrame nilARCBlockFrame
#define nilEmptyFrame nilARCEmptyFrame
#define nilActorFrame nilARCActorFrame
#define nilForeignSync nilARCForeignSync
#define nilObjectSync nilARCObjectSync
#define nilConsumedBlock nilARCConsumedBlock
#endif

static int entries, completed, created;
static _Thread_local int fail_allocation;
void hop_initializing_reset(int failure) {
  __atomic_store_n(&entries, 0, __ATOMIC_RELEASE);
  __atomic_store_n(&completed, 0, __ATOMIC_RELEASE);
  __atomic_store_n(&created, 0, __ATOMIC_RELEASE);
  fail_allocation = failure;
}
int hop_initializing_entries(void) {return __atomic_load_n(&entries, __ATOMIC_ACQUIRE);}
int hop_initializing_completed(void) {return __atomic_load_n(&completed, __ATOMIC_ACQUIRE);}
int hop_initializing_created(void) {return __atomic_load_n(&created, __ATOMIC_ACQUIRE);}
static void enter(void) {
  objc3_executor_t *main = 0;
  assert(objc3_executor_get_main(&main) == OBJC3_SERVICE_OK);
  assert(objc3_executor_is_current(main));
  objc3_executor_release(main);
  __atomic_fetch_add(&entries, 1, __ATOMIC_RELEASE);
}
static HopObject *make_object(void) __attribute__((nothrow, ns_returns_retained));
static HopObject *make_object(void) {
  HopObject *object = [HopObject new];
  assert(object);
  __atomic_fetch_add(&created, 1, __ATOMIC_RELEASE);
  return object;
}
static void arm_failure(void) {
  if (fail_allocation == 1) hop_fail_next_frame_allocation();
  if (fail_allocation == 2) hop_fail_second_frame_allocation();
  fail_allocation = 0;
}
static HopObject *source_object(objc3_executor_t *caller)
    __attribute__((nothrow, ns_returns_retained));
static HopObject *source_object(objc3_executor_t *caller) {
  assert(objc3_executor_is_current(caller));
  HopObject *object = make_object();
  arm_failure();
  return object;
}
static void pause_frame(void) __objc3_async_frame {return;}
#if !__has_feature(objc_arc)
static void release_claim(HopObject **claim) {objc_release(*claim);}
#endif
typedef int (^NilInitializationWork)(void) __attribute__((objc_unsafe_sendable));
@interface HopObject (Initialization)
- (instancetype)initFrame:(int)kind __objc3_async_frame
    __attribute__((nothrow, objc_executor(main)));
+ (instancetype)newFrame __objc3_async_frame
    __attribute__((nothrow, objc_executor(main)));
- (instancetype)initDirectFrame __objc3_async_frame __attribute__((objc_direct,nothrow,objc_executor(main)));
- (int)nilScalarFrame:(__attribute__((ns_consumed)) HopObject *)value __objc3_async_frame __attribute__((nothrow,objc_executor(main)));
- (void)nilVoidFrame:(__attribute__((ns_consumed)) HopObject *)value __objc3_async_frame __attribute__((nothrow,objc_executor(main)));
- (HopPacket)nilRecordFrame:(__attribute__((ns_consumed)) HopObject *)value __objc3_async_frame __attribute__((nothrow,objc_executor(main)));
- (HopCFRef)nilForeignFrame:(__attribute__((cf_consumed)) HopCFRef)value __objc3_async_frame __attribute__((nothrow,cf_returns_retained,objc_executor(main)));
- (HopObject *)nilObjectFrame:(__attribute__((ns_consumed)) HopObject *)value __objc3_async_frame __attribute__((nothrow,ns_returns_retained,objc_executor(main)));
- (double)nilDoubleFrame:(__attribute__((ns_consumed)) HopObject *)value __objc3_async_frame __attribute__((nothrow,objc_executor(main)));
- (NilInitializationWork)nilBlockFrame:(__attribute__((ns_consumed)) HopObject *)value __objc3_async_frame __attribute__((nothrow,objc_executor(main)));
- (void)nilEmptyFrame __objc3_async_frame __attribute__((nothrow,objc_executor(main)));
- (HopCFRef)nilForeignSync:(__attribute__((cf_consumed)) HopCFRef)value __attribute__((cf_returns_retained,nothrow,objc_executor(main)));
- (int)nilObjectSync:(__attribute__((ns_consumed)) HopObject *)value __attribute__((nothrow,objc_executor(main)));
- (int)nilConsumedBlock:(__attribute__((ns_consumed)) NilInitializationWork)work __objc3_async_frame __attribute__((nothrow,objc_executor(main)));
@end
@implementation HopObject (Initialization)
- (instancetype)initFrame:(int)kind __objc3_async_frame {
  enter();
#if !__has_feature(objc_arc)
  // Manual body ownership must cover cancellation just as ordinary early exit.
  HopObject *claim __attribute__((cleanup(release_claim))) = self;
#endif
  if (kind == 3) {
#if !__has_feature(objc_arc)
    objc_release(claim);
#endif
    self = make_object();
#if !__has_feature(objc_arc)
    claim = self;
#endif
  }
  __objc3_await pause_frame();
  __atomic_fetch_add(&completed, 1, __ATOMIC_RELEASE);
  if (kind == 1) return 0;
  if (kind == 2) {
#if !__has_feature(objc_arc)
    objc_release(claim);
#endif
    self = make_object();
#if !__has_feature(objc_arc)
    claim = self;
#endif
  }
#if !__has_feature(objc_arc)
  claim = 0; // Transfer the manual +1 into the declared retained result.
#endif
  return self;
}
+ (instancetype)newFrame __objc3_async_frame {
  enter();
  __objc3_await pause_frame();
  __atomic_fetch_add(&completed, 1, __ATOMIC_RELEASE);
  return make_object();
}
- (instancetype)initDirectFrame __objc3_async_frame {assert(0);return 0;}
- (int)nilScalarFrame:(__attribute__((ns_consumed)) HopObject *)value __objc3_async_frame {assert(0);return 0;}
- (void)nilVoidFrame:(__attribute__((ns_consumed)) HopObject *)value __objc3_async_frame {assert(0);return;}
- (HopPacket)nilRecordFrame:(__attribute__((ns_consumed)) HopObject *)value __objc3_async_frame {assert(0);return (HopPacket){0};}
- (HopCFRef)nilForeignFrame:(__attribute__((cf_consumed)) HopCFRef)value __objc3_async_frame {assert(0);return 0;}
- (HopObject *)nilObjectFrame:(__attribute__((ns_consumed)) HopObject *)value __objc3_async_frame {assert(0);return 0;}
- (double)nilDoubleFrame:(__attribute__((ns_consumed)) HopObject *)value __objc3_async_frame {assert(0);return 0;}
- (NilInitializationWork)nilBlockFrame:(__attribute__((ns_consumed)) HopObject *)value __objc3_async_frame {assert(0);return 0;}
- (void)nilEmptyFrame __objc3_async_frame {assert(0);return;}
- (HopCFRef)nilForeignSync:(__attribute__((cf_consumed)) HopCFRef)value {assert(0);return 0;}
- (int)nilObjectSync:(__attribute__((ns_consumed)) HopObject *)value {assert(0);return 0;}
- (int)nilConsumedBlock:(__attribute__((ns_consumed)) NilInitializationWork)work __objc3_async_frame {assert(0);return 0;}
@end
static HopObject *retained_frame(void) __objc3_async_frame
    __attribute__((nothrow, ns_returns_retained, objc_executor(main)));
static HopObject *retained_frame(void) __objc3_async_frame {
  enter();
  __objc3_await pause_frame();
  __atomic_fetch_add(&completed, 1, __ATOMIC_RELEASE);
  return make_object();
}
int hop_initializing_roundtrip(int kind, objc3_executor_t *caller) __objc3_async_frame {
  HopObject *value;
#if __has_feature(objc_arc)
  if (kind < 4) value = __objc3_await [source_object(caller) initFrame:kind];
  else {
    arm_failure();
    if (kind == 4) value = __objc3_await retained_frame();
    else value = __objc3_await [HopObject newFrame];
  }
#else
  if (kind < 4) value = objc_retain(__objc3_await [source_object(caller) initFrame:kind]);
  else {
    arm_failure();
    if (kind == 4) value = objc_retain(__objc3_await retained_frame());
    else value = objc_retain(__objc3_await [HopObject newFrame]);
  }
#endif
  assert(objc3_executor_is_current(caller));
  int result = value ? [value number] : 0;
#if !__has_feature(objc_arc)
  objc_release(value);
#endif
  return result;
}

@interface HopCounter (Initialization)
- (int)nilActorFrame:(__attribute__((ns_consumed)) HopObject *)value __objc3_async_frame __attribute__((nothrow));
@end
@implementation HopCounter (Initialization)
- (int)nilActorFrame:(__attribute__((ns_consumed)) HopObject *)value __objc3_async_frame {assert(0);return 0;}
@end

int hop_initializing_nil_roundtrip(int kind, objc3_executor_t *caller) __objc3_async_frame {
  HopObject *receiver = 0;
  int result = 0;
  switch (kind) {
  case 0: result = (__objc3_await [receiver initFrame:0]) == 0; break;
  case 1: result = (__objc3_await [receiver nilScalarFrame:source_object(caller)]) == 0; break;
  case 2: __objc3_await [receiver nilVoidFrame:source_object(caller)]; result = 1; break;
  case 3: {
    HopPacket value = __objc3_await [receiver nilRecordFrame:source_object(caller)];
    result = value.value == 0 && value.weights[0] == 0 && value.weights[1] == 0 && value.weights[2] == 0;
    break;
  }
  case 4: result = (__objc3_await [receiver nilForeignFrame:hop_cf_source(caller)]) == 0; break;
  case 5: result = (__objc3_await [receiver initDirectFrame]) == 0; break;
  case 6: result = (__objc3_await [receiver nilObjectFrame:source_object(caller)]) == 0; break;
  case 7: result = (__objc3_await [receiver nilDoubleFrame:source_object(caller)]) == 0; break;
  case 8: result = (__objc3_await [receiver nilBlockFrame:source_object(caller)]) == 0; break;
  case 9: __objc3_await [receiver nilEmptyFrame]; result = 1; break;
  case 10: {
    HopCounter *actor_receiver = 0;
    result = (__objc3_await [actor_receiver nilActorFrame:source_object(caller)]) == 0;
    break;
  }
  case 11: result = (__objc3_await [receiver nilForeignSync:hop_cf_source(caller)]) == 0; break;
  case 12: result = (__objc3_await [receiver nilObjectSync:source_object(caller)]) == 0; break;
  case 13: {
    HopObject *object = source_object(caller);
#if __has_feature(objc_arc)
    NilInitializationWork work = ^{return [object number];};
#else
    NilInitializationWork work = (NilInitializationWork)_Block_copy(^{return [object number];});
    objc_release(object);
#endif
    result = (__objc3_await [receiver nilConsumedBlock:work]) == 0;
    break;
  }
  case 14: result = (__objc3_await [receiver nilForeignFrame:(HopCFRef)0]) == 0; break;
  case 15: result = (__objc3_await [receiver nilScalarFrame:0]) == 0; break;
  default: assert(0);
  }
  assert(objc3_executor_is_current(caller));
  return result ? 42 : 0;
}
