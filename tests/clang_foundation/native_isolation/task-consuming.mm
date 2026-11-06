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
#define hop_mrc_ordinary_roundtrip hop_arc_ordinary_roundtrip
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
static objc3::task<int> ordinary_unstarted(__attribute__((ns_consumed)) HopObject *value) {
#if !__has_feature(objc_arc)
  ManualObjectClaim claim{value};
#endif
  assert(!"An unstarted ordinary coroutine must not enter its body");
  co_return [value number];
}
static objc3::task<int> ordinary_failed_copy(__attribute__((ns_consumed)) HopObject *value,
                                           TaskParameterCopyFailure failure) {
  (void)failure;
  assert(!"A failed ordinary coroutine copy must not enter its body");
  co_return [value number];
}
static objc3::task<int> ordinary_body(__attribute__((ns_consumed)) HopObject *value,
                                    objc3_executor_t *caller) {
#if !__has_feature(objc_arc)
  ManualObjectClaim claim{value};
#endif
  hop_mrc_enter_on(caller);
  co_await objc3::task_yield;
  assert(objc3_executor_is_current(caller));
  co_return [value number];
}
static ManualTaskWork ordinary_block_source(objc3_executor_t *caller)
    __attribute__((ns_returns_retained));
static ManualTaskWork ordinary_block_source(objc3_executor_t *caller) {
  HopObject *object = hop_mrc_source(caller);
#if __has_feature(objc_arc)
  return ^{return [object number];};
#else
  ManualTaskWork work = (ManualTaskWork)_Block_copy(^{return [object number];});
  objc_release(object);
  return work;
#endif
}
static objc3::task<int> ordinary_unstarted_block(__attribute__((ns_consumed)) ManualTaskWork work) {
#if !__has_feature(objc_arc)
  ManualBlockClaim claim{work};
#endif
  assert(!"An unstarted ordinary Block coroutine must not enter its body");
  co_return work();
}
static objc3::task<int> ordinary_failed_block_copy(__attribute__((ns_consumed)) ManualTaskWork work,
                                                 TaskParameterCopyFailure failure) {
  (void)failure;
  assert(!"A failed ordinary Block coroutine copy must not enter its body");
  co_return work();
}
static objc3::task<int> ordinary_block_body(__attribute__((ns_consumed)) ManualTaskWork work,
                                          objc3_executor_t *caller) {
#if !__has_feature(objc_arc)
  ManualBlockClaim claim{work};
#endif
  hop_mrc_enter_on(caller);
  co_await objc3::task_yield;
  assert(objc3_executor_is_current(caller));
  co_return work();
}
static int run_ordinary(objc3_executor_t *caller, bool block) __objc3_async {
  if (block) {
    int result = co_await ordinary_block_body(ordinary_block_source(caller), caller);
    return result;
  }
  int result = co_await ordinary_body(hop_mrc_source(caller), caller);
  return result;
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
extern "C" int hop_mrc_ordinary_roundtrip(int kind, objc3_executor_t *caller)
    __objc3_async_frame {
  if (kind % 4 == 0) {
    if (kind < 4) {
      auto unused = ordinary_unstarted(hop_mrc_source(caller));
      (void)unused;
    } else {
      auto unused = ordinary_unstarted_block(ordinary_block_source(caller));
      (void)unused;
    }
  } else if (kind % 4 == 1) {
    bool caught = false;
    try {
      if (kind < 4) {
        auto failed = ordinary_failed_copy(hop_mrc_source(caller), TaskParameterCopyFailure{});
        (void)failed;
      } else {
        auto failed = ordinary_failed_block_copy(ordinary_block_source(caller), TaskParameterCopyFailure{});
        (void)failed;
      }
    } catch (int value) {assert(value == 77); caught = true;}
    assert(caught);
  } else {
    return __objc3_await run_ordinary(caller, kind >= 4);
  }
  assert(objc3_executor_is_current(caller));
  return 42;
}
