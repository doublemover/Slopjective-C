#include "cf-contract.h"
#include "objc3_task.hpp"
#include <assert.h>
#include <objc/runtime.h>
#include <objc/objc-arc.h>
#include <objc/blocks_runtime.h>

#if __has_feature(objc_arc)
#define TaskResults ARCTaskResults
#define initResultTask initARCResultTask
#define initDirectResultTask initARCDirectResultTask
#define newResultTask newARCResultTask
#define ownedForeignTask ownedARCForeignTask
#define borrowedForeignTask borrowedARCForeignTask
#define consumingForeignTask consumingARCForeignTask
#define nilScalarTask nilARCScalarTask
#define nilVoidTask nilARCVoidTask
#define nilPacketTask nilARCPacketTask
#define nilObjectTask nilARCObjectTask
#define nilForeignTask nilARCForeignTask
#define nilDoubleTask nilARCDoubleTask
#define nilBlockTask nilARCBlockTask
#define nilActorTask nilARCActorTask
#define nilConsumedBlockTask nilARCConsumedBlockTask
#define hop_task_result_roundtrip hop_arc_task_result_roundtrip
#define hop_task_result_nil_roundtrip hop_arc_task_result_nil_roundtrip
#define hop_task_result_reset hop_arc_task_result_reset
#define hop_task_result_entries hop_arc_task_result_entries
#define hop_task_result_completed hop_arc_task_result_completed
#define hop_task_result_created hop_arc_task_result_created
#define hop_task_result_release_owner hop_arc_task_result_release_owner
#endif

static int entries, completed, created;
static HopObject *borrowed_object;
static HopCFRef borrowed_foreign;
extern "C" void hop_task_result_release_owner(void) {
#if !__has_feature(objc_arc)
  objc_release(borrowed_object);
#endif
  borrowed_object = 0;
  if (borrowed_foreign) CFRelease(borrowed_foreign);
  borrowed_foreign = 0;
}
extern "C" void hop_task_result_reset(void) {
  assert(!borrowed_object && !borrowed_foreign);
  __atomic_store_n(&entries, 0, __ATOMIC_RELEASE);
  __atomic_store_n(&completed, 0, __ATOMIC_RELEASE);
  __atomic_store_n(&created, 0, __ATOMIC_RELEASE);
}
extern "C" int hop_task_result_entries(void) {return __atomic_load_n(&entries, __ATOMIC_ACQUIRE);}
extern "C" int hop_task_result_completed(void) {return __atomic_load_n(&completed, __ATOMIC_ACQUIRE);}
extern "C" int hop_task_result_created(void) {return __atomic_load_n(&created, __ATOMIC_ACQUIRE);}
static void enter(void) {
  objc3_executor_t *main = nullptr;
  assert(objc3_executor_get_main(&main) == OBJC3_SERVICE_OK);
  assert(objc3_executor_is_current(main));
  objc3_executor_release(main);
  __atomic_fetch_add(&entries, 1, __ATOMIC_RELEASE);
}
static void complete(void) {__atomic_fetch_add(&completed, 1, __ATOMIC_RELEASE);}
static HopObject *make_object(void) noexcept __attribute__((nothrow, ns_returns_retained));
static HopObject *make_object(void) noexcept {
  HopObject *value = [HopObject new];
  assert(value);
  __atomic_fetch_add(&created, 1, __ATOMIC_RELEASE);
  return value;
}
static HopCFRef make_foreign(void) noexcept __attribute__((nothrow, cf_returns_retained));
static HopCFRef make_foreign(void) noexcept {
  objc3_executor_t *main = nullptr;
  assert(objc3_executor_get_main(&main) == OBJC3_SERVICE_OK);
  HopCFRef value = hop_cf_source(main);
  objc3_executor_release(main);
  return value;
}
#if !__has_feature(objc_arc)
struct ObjectClaim {
  HopObject *value;
  ~ObjectClaim() {objc_release(value);}
  HopObject *take() {return std::exchange(value, nullptr);}
};
#endif
struct ForeignClaim {
  HopCFRef value;
  ~ForeignClaim() {if (value) CFRelease(value);}
  HopCFRef take() {return std::exchange(value, nullptr);}
};
typedef int (^TaskResultWork)(void) __attribute__((objc_unsafe_sendable));
@interface HopObject (TaskResults)
- (instancetype)initResultTask:(int)kind __objc3_async __attribute__((nothrow, objc_executor(main)));
- (instancetype)initDirectResultTask __objc3_async __attribute__((objc_direct, nothrow, objc_executor(main)));
+ (instancetype)newResultTask __objc3_async __attribute__((nothrow, objc_executor(main)));
+ (HopCFRef)ownedForeignTask __objc3_async __attribute__((nothrow, cf_returns_retained, objc_executor(main)));
+ (HopCFRef)borrowedForeignTask __objc3_async __attribute__((nothrow, cf_returns_not_retained, objc_executor(main)));
- (HopCFRef)consumingForeignTask:(__attribute__((cf_consumed)) HopCFRef)value __objc3_async
    __attribute__((nothrow, ns_consumes_self, cf_returns_retained, objc_executor(main)));
- (int)nilScalarTask:(__attribute__((ns_consumed)) HopObject *)value __objc3_async __attribute__((nothrow, objc_executor(main)));
- (void)nilVoidTask:(__attribute__((ns_consumed)) HopObject *)value __objc3_async __attribute__((nothrow, objc_executor(main)));
- (HopPacket)nilPacketTask:(__attribute__((ns_consumed)) HopObject *)value __objc3_async __attribute__((nothrow, objc_executor(main)));
- (HopObject *)nilObjectTask:(__attribute__((ns_consumed)) HopObject *)value __objc3_async __attribute__((nothrow, ns_returns_retained, objc_executor(main)));
- (HopCFRef)nilForeignTask:(__attribute__((cf_consumed)) HopCFRef)value __objc3_async __attribute__((nothrow, cf_returns_retained, objc_executor(main)));
- (double)nilDoubleTask __objc3_async __attribute__((nothrow, objc_executor(main)));
- (TaskResultWork)nilBlockTask __objc3_async __attribute__((nothrow, objc_executor(main)));
- (int)nilConsumedBlockTask:(__attribute__((ns_consumed)) TaskResultWork)value __objc3_async __attribute__((nothrow, objc_executor(main)));
@end
@implementation HopObject (TaskResults)
- (instancetype)initResultTask:(int)kind __objc3_async {
  enter();
#if !__has_feature(objc_arc)
  ObjectClaim claim{self};
#endif
  if (kind == 3) {
#if !__has_feature(objc_arc)
    objc_release(claim.value);
#endif
    self = make_object();
#if !__has_feature(objc_arc)
    claim.value = self;
#endif
  }
  __objc3_await objc3::task_yield;
  complete();
  if (kind == 1) return nil;
  if (kind == 2) {
#if !__has_feature(objc_arc)
    objc_release(claim.value);
#endif
    self = make_object();
#if !__has_feature(objc_arc)
    claim.value = self;
#endif
  }
#if !__has_feature(objc_arc)
  (void)claim.take(); // The retained result transfers this claim to the promise.
#endif
  return self;
}
- (instancetype)initDirectResultTask __objc3_async {assert(0); return nil;}
+ (instancetype)newResultTask __objc3_async {
  enter();
  __objc3_await objc3::task_yield;
  complete();
  return make_object();
}
+ (HopCFRef)ownedForeignTask __objc3_async {
  enter();
  ForeignClaim claim{make_foreign()};
  __objc3_await objc3::task_yield;
  complete();
  return claim.take();
}
+ (HopCFRef)borrowedForeignTask __objc3_async {
  enter();
  __objc3_await objc3::task_yield;
  complete();
  return borrowed_foreign;
}
- (HopCFRef)consumingForeignTask:(__attribute__((cf_consumed)) HopCFRef)value __objc3_async {
#if !__has_feature(objc_arc)
  ObjectClaim receiver{self};
#endif
  ForeignClaim claim{value};
  enter();
  __objc3_await objc3::task_yield;
  complete();
  return claim.take();
}
- (int)nilScalarTask:(__attribute__((ns_consumed)) HopObject *)value __objc3_async {assert(0); return 0;}
- (void)nilVoidTask:(__attribute__((ns_consumed)) HopObject *)value __objc3_async {assert(0); return;}
- (HopPacket)nilPacketTask:(__attribute__((ns_consumed)) HopObject *)value __objc3_async {assert(0); return {};}
- (HopObject *)nilObjectTask:(__attribute__((ns_consumed)) HopObject *)value __objc3_async {assert(0); return nil;}
- (HopCFRef)nilForeignTask:(__attribute__((cf_consumed)) HopCFRef)value __objc3_async {assert(0); return nullptr;}
- (double)nilDoubleTask __objc3_async {assert(0); return 0;}
- (TaskResultWork)nilBlockTask __objc3_async {assert(0); return nil;}
- (int)nilConsumedBlockTask:(__attribute__((ns_consumed)) TaskResultWork)value __objc3_async {assert(0); return 0;}
@end
@interface HopCounter (TaskResults)
- (int)nilActorTask:(__attribute__((ns_consumed)) HopObject *)value __objc3_async __attribute__((nothrow));
@end
@implementation HopCounter (TaskResults)
- (int)nilActorTask:(__attribute__((ns_consumed)) HopObject *)value __objc3_async {assert(0); return 0;}
@end

static HopObject *owned_task(void) noexcept __objc3_async __attribute__((nothrow, ns_returns_retained, objc_executor(main)));
static HopObject *owned_task(void) noexcept __objc3_async {
  enter();
  __objc3_await objc3::task_yield;
  complete();
  return make_object();
}
static HopObject *borrowed_task(void) noexcept __objc3_async __attribute__((nothrow, ns_returns_not_retained, objc_executor(main)));
static HopObject *borrowed_task(void) noexcept __objc3_async {
  enter();
  __objc3_await objc3::task_yield;
  complete();
  return borrowed_object;
}
static HopCFRef foreign_task(int kind) noexcept __objc3_async __attribute__((nothrow, cf_returns_retained, objc_executor(main)));
static HopCFRef foreign_task(int kind) noexcept __objc3_async {
  enter();
  ForeignClaim claim{kind == 9 ? nullptr : make_foreign()};
  __objc3_await objc3::task_yield;
  complete();
  return claim.take();
}
static HopCFRef foreign_borrowed_task(void) noexcept __objc3_async __attribute__((nothrow, cf_returns_not_retained, objc_executor(main)));
static HopCFRef foreign_borrowed_task(void) noexcept __objc3_async {
  enter();
  __objc3_await objc3::task_yield;
  complete();
  return borrowed_foreign;
}
static HopCFRef raw_foreign_task(void) noexcept __objc3_async __attribute__((objc_executor(main)));
static HopCFRef raw_foreign_task(void) noexcept __objc3_async {
  enter();
  __objc3_await objc3::task_yield;
  complete();
  return borrowed_foreign;
}
static TaskResultWork owned_block_task(void) noexcept __objc3_async
    __attribute__((ns_returns_retained, objc_executor(main)));
static TaskResultWork owned_block_task(void) noexcept __objc3_async {
  enter();
  HopObject *object = make_object();
#if !__has_feature(objc_arc)
  ObjectClaim claim{object};
#endif
  __objc3_await objc3::task_yield;
  complete();
#if __has_feature(objc_arc)
  return ^{return [object number];};
#else
  return (TaskResultWork)_Block_copy(^{return [object number];});
#endif
}
static HopCFRef claim_foreign(HopCFRef value) {return value ? static_cast<HopCFRef>(CFRetain(value)) : nullptr;}

// Keep one source body for both callers: native-frame and ordinary C++ task
// awaits must honor the producer's descriptor and the logical related type.
#define TASK_RESULT_BODY \
  if (kind == 6) borrowed_object = make_object(); \
  if (kind == 8 || kind == 11 || kind == 13) borrowed_foreign = hop_cf_source(caller); \
  int result = 0; \
  if (kind == 14) { \
    TaskResultWork value = CLAIM_BLOCK(__objc3_await owned_block_task()); \
    result = value(); \
    RELEASE_BLOCK(value); \
  } else if (kind <= 6) { \
    HopObject *value; \
    if (kind < 4) value = CLAIM_OBJECT(__objc3_await [make_object() initResultTask:kind]); \
    else if (kind == 4) value = CLAIM_OBJECT(__objc3_await owned_task()); \
    else if (kind == 5) value = CLAIM_OBJECT(__objc3_await [HopObject newResultTask]); \
    else value = CLAIM_OBJECT(__objc3_await borrowed_task()); \
    result = value ? [value number] : 0; \
    RELEASE_OBJECT(value); \
  } else { \
    HopCFRef value; \
    if (kind == 7 || kind == 9) value = claim_foreign(__objc3_await foreign_task(kind)); \
    else if (kind == 8) value = claim_foreign(__objc3_await foreign_borrowed_task()); \
    else if (kind == 10) value = claim_foreign(__objc3_await [HopObject ownedForeignTask]); \
    else if (kind == 11) value = claim_foreign(__objc3_await [HopObject borrowedForeignTask]); \
    else if (kind == 13) value = claim_foreign(__objc3_await raw_foreign_task()); \
    else value = claim_foreign(__objc3_await [make_object() consumingForeignTask:hop_cf_source(caller)]); \
    result = hop_cf_value(value); \
    if (value) CFRelease(value); \
  } \
  assert(objc3_executor_is_current(caller)); \
  return result;
#if __has_feature(objc_arc)
#define CLAIM_OBJECT(value) (value)
#define CLAIM_BLOCK(value) (value)
#define RELEASE_BLOCK(value) ((void)0)
#define RELEASE_OBJECT(value) ((void)0)
#else
#define CLAIM_OBJECT(value) objc_retain(value)
#define CLAIM_BLOCK(value) (TaskResultWork)_Block_copy(value)
#define RELEASE_BLOCK(value) _Block_release(value)
#define RELEASE_OBJECT(value) objc_release(value)
#endif
static int result_task_caller(int kind, objc3_executor_t *caller) __objc3_async {
  TASK_RESULT_BODY
}
static int result_task_main(int kind) noexcept __objc3_async __attribute__((objc_executor(main)));
static int result_task_main(int kind) noexcept __objc3_async {
  objc3_executor_t *caller = nullptr;
  assert(objc3_executor_get_main(&caller) == OBJC3_SERVICE_OK);
  struct Lease {objc3_executor_t *value; ~Lease() {objc3_executor_release(value);}} lease{caller};
  TASK_RESULT_BODY
}
extern "C" int hop_task_result_roundtrip(int kind, int task_caller, objc3_executor_t *caller) __objc3_async_frame {
  if (task_caller == 2) return __objc3_await result_task_main(kind);
  if (task_caller) return __objc3_await result_task_caller(kind, caller);
  TASK_RESULT_BODY
}
#undef TASK_RESULT_BODY

#define NIL_TASK_BODY \
  HopObject *receiver = nil; \
  int result = 0; \
  switch (kind) { \
  case 0: result = (__objc3_await [receiver initResultTask:0]) == nil; break; \
  case 1: result = (__objc3_await [receiver nilScalarTask:make_object()]) == 0; break; \
  case 2: __objc3_await [receiver nilVoidTask:make_object()]; result = 1; break; \
  case 3: { \
    HopPacket value = __objc3_await [receiver nilPacketTask:make_object()]; \
    result = value.value == 0 && value.weights[0] == 0 && value.weights[1] == 0 && value.weights[2] == 0; break; \
  } \
  case 4: result = (__objc3_await [receiver nilObjectTask:make_object()]) == nil; break; \
  case 5: result = (__objc3_await [receiver nilForeignTask:hop_cf_source(caller)]) == nullptr; break; \
  case 6: result = (__objc3_await [receiver initDirectResultTask]) == nil; break; \
  case 7: result = (__objc3_await [receiver nilDoubleTask]) == 0; break; \
  case 8: result = (__objc3_await [receiver nilBlockTask]) == nil; break; \
  case 9: {HopCounter *actor = nil; result = (__objc3_await [actor nilActorTask:make_object()]) == 0; break;} \
  case 10: { \
    HopObject *object = make_object(); \
    TaskResultWork work = MAKE_BLOCK(object); \
    RELEASE_OBJECT(object); \
    result = (__objc3_await [receiver nilConsumedBlockTask:work]) == 0; break; \
  } \
  default: assert(0); \
  } \
  assert(result && objc3_executor_is_current(caller)); \
  return 42;
#if __has_feature(objc_arc)
#define MAKE_BLOCK(object) ^{return [object number];}
#else
#define MAKE_BLOCK(object) (TaskResultWork)_Block_copy(^{return [object number];})
#endif
static int nil_task_caller(int kind, objc3_executor_t *caller) __objc3_async {
  NIL_TASK_BODY
}
static int nil_task_main(int kind) noexcept __objc3_async __attribute__((objc_executor(main)));
static int nil_task_main(int kind) noexcept __objc3_async {
  objc3_executor_t *caller = nullptr;
  assert(objc3_executor_get_main(&caller) == OBJC3_SERVICE_OK);
  struct Lease {objc3_executor_t *value; ~Lease() {objc3_executor_release(value);}} lease{caller};
  NIL_TASK_BODY
}
extern "C" int hop_task_result_nil_roundtrip(int kind, int task_caller, objc3_executor_t *caller) __objc3_async_frame {
  if (task_caller == 2) return __objc3_await nil_task_main(kind);
  if (task_caller) return __objc3_await nil_task_caller(kind, caller);
  NIL_TASK_BODY
}
