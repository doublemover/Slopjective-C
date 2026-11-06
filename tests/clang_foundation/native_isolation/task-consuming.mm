#include "provider.h"
#include "objc3_task.hpp"
#include <assert.h>
#include <objc/runtime.h>
#include <objc/objc-arc.h>
#include <objc/blocks_runtime.h>
#if __has_feature(objc_arc)
#define ManualTaskConsumption ARCTaskConsumption
#define manualTaskSelf arcTaskSelf
#define manualTaskClass arcTaskClass
#define manualTaskBoth arcTaskBoth
#define hop_mrc_task_roundtrip hop_arc_task_roundtrip
#define hop_mrc_unstarted_roundtrip hop_arc_unstarted_roundtrip
#define hop_mrc_failed_copy_roundtrip hop_arc_failed_copy_roundtrip
#endif

// The native callee owns its handoff claim. RAII balances it if cancellation
// throws from a later await as well as on an ordinary source return.
#if !__has_feature(objc_arc)
struct ManualObjectClaim {
  HopObject *value;
  ~ManualObjectClaim() {objc_release(value);}
};
struct ManualBlockClaim {
  int (^value)(void);
  ~ManualBlockClaim() {_Block_release(value);}
};
#endif
typedef int (^ManualTaskWork)(void) __attribute__((objc_unsafe_sendable));
static int manual_task_object(__attribute__((ns_consumed)) HopObject *value)
    noexcept __objc3_async __attribute__((objc_executor(main)));
static int manual_task_object(__attribute__((ns_consumed)) HopObject *value)
    noexcept __objc3_async {
#if !__has_feature(objc_arc)
  ManualObjectClaim claim{value};
#endif
  hop_mrc_enter();
  __objc3_await objc3::task_yield;
  return [value number];
}
static int manual_task_block(__attribute__((ns_consumed)) ManualTaskWork work)
    noexcept __objc3_async __attribute__((objc_executor(main)));
static int manual_task_block(__attribute__((ns_consumed)) ManualTaskWork work)
    noexcept __objc3_async {
#if !__has_feature(objc_arc)
  ManualBlockClaim claim{work};
#endif
  hop_mrc_enter();
  __objc3_await objc3::task_yield;
  return work();
}
static int manual_unstarted_task(__attribute__((ns_consumed)) HopObject *value)
    noexcept __objc3_async {
#if !__has_feature(objc_arc)
  ManualObjectClaim claim{value};
#endif
  assert(!"An unstarted task must never enter its manual body");
  return [value number];
}
struct TaskParameterCopyFailure {
  TaskParameterCopyFailure() = default;
  TaskParameterCopyFailure(const TaskParameterCopyFailure &) = delete;
  TaskParameterCopyFailure(TaskParameterCopyFailure &&) {throw 77;}
};
static int manual_failed_copy_task(__attribute__((ns_consumed)) HopObject *value,
                                  TaskParameterCopyFailure failure) __objc3_async {
  (void)failure;
  assert(!"A failed parameter copy must not enter its manual body");
  return [value number];
}

@interface HopObject (ManualTaskConsumption)
- (int)manualTaskSelf __objc3_async
    __attribute__((nothrow, ns_consumes_self, objc_executor(main)));
+ (int)manualTaskClass:(__attribute__((ns_consumed)) HopObject *)other
    __objc3_async __attribute__((nothrow, objc_executor(main)));
- (int)manualTaskBoth:(__attribute__((ns_consumed)) HopObject *)other
    __objc3_async __attribute__((nothrow, ns_consumes_self, objc_executor(main)));
#if __has_feature(objc_arc)
- (int)arcTaskReplacingSelf __objc3_async
    __attribute__((nothrow, ns_consumes_self, objc_executor(main)));
#endif
@end
@implementation HopObject (ManualTaskConsumption)
- (int)manualTaskSelf __objc3_async {
#if !__has_feature(objc_arc)
  ManualObjectClaim claim{self};
#endif
  hop_mrc_enter();
  __objc3_await objc3::task_yield;
  return [self number];
}
+ (int)manualTaskClass:(__attribute__((ns_consumed)) HopObject *)other __objc3_async {
#if !__has_feature(objc_arc)
  ManualObjectClaim claim{other};
#endif
  hop_mrc_enter();
  __objc3_await objc3::task_yield;
  return [other number];
}
- (int)manualTaskBoth:(__attribute__((ns_consumed)) HopObject *)other __objc3_async {
#if !__has_feature(objc_arc)
  ManualObjectClaim receiver{self}, argument{other};
#endif
  hop_mrc_enter();
  __objc3_await objc3::task_yield;
  return [self number]+[other number]-42;
}
#if __has_feature(objc_arc)
- (int)arcTaskReplacingSelf __objc3_async {
  hop_mrc_enter();
  self = [HopObject new];
  __objc3_await objc3::task_yield;
  return [self number];
}
#endif
@end

extern "C" int hop_mrc_task_roundtrip(int kind, objc3_executor_t *caller)
    __objc3_async_frame {
  int result = 0;
  switch (kind) {
  case 0: result = __objc3_await manual_task_object(hop_mrc_source(caller)); break;
  case 1: {
    HopObject *object = hop_mrc_source(caller);
#if __has_feature(objc_arc)
    ManualTaskWork work = ^{return [object number];};
#else
    ManualTaskWork work = (ManualTaskWork)_Block_copy(^{return [object number];});
    objc_release(object);
#endif
    result = __objc3_await manual_task_block(work);
    break;
  }
  case 2: result = __objc3_await [hop_mrc_source(caller) manualTaskSelf]; break;
  case 3: result = __objc3_await [HopObject manualTaskClass:hop_mrc_source(caller)]; break;
  case 4:
    result = __objc3_await [hop_mrc_source(caller) manualTaskBoth:hop_mrc_source(caller)];
    break;
#if __has_feature(objc_arc)
  case 5: result = __objc3_await [hop_mrc_source(caller) arcTaskReplacingSelf]; break;
#endif
  default: assert(0);
  }
  assert(objc3_executor_is_current(caller));
  return result;
}
extern "C" int hop_mrc_unstarted_roundtrip(objc3_executor_t *caller)
    __objc3_async_frame {
  {
    auto abandoned = manual_unstarted_task(hop_mrc_source(caller));
    (void)abandoned;
  }
  assert(objc3_executor_is_current(caller));
  return 42;
}
extern "C" int hop_mrc_failed_copy_roundtrip(objc3_executor_t *caller)
    __objc3_async_frame {
  bool caught = false;
  try {
    // The prvalue is the ABI parameter. Its move into the coroutine throws
    // after entry adoption, before promise construction or initial suspension.
    auto failed = manual_failed_copy_task(hop_mrc_source(caller), TaskParameterCopyFailure{});
    (void)failed;
  } catch (int value) {
    assert(value == 77);
    caught = true;
  }
  assert(caught && objc3_executor_is_current(caller));
  return 42;
}
