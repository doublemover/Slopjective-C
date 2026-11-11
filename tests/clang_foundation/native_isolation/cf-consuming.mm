#include "cf-contract.h"
#include "objc3_task.hpp"
#include <assert.h>
#include <objc/runtime.h>
#include <objc/objc-arc.h>
#include <objc/blocks_runtime.h>
#if __has_feature(objc_arc)
#define CFConsumption CFARCConsumption
#define cfConsume cfARCConsume
#define cfConsumeFrame cfARCConsumeFrame
#define cfConsumeTask cfARCConsumeTask
#define cfClass cfARCClass
#define cfClassFrame cfARCClassFrame
#define cfClassTask cfARCClassTask
#define cfOrdinaryTask cfARCOrdinaryTask
#define cfOrdinaryReplacingTask cfARCOrdinaryReplacingTask
#define hop_cf_roundtrip hop_cf_arc_roundtrip
#define hop_cf_aux_roundtrip hop_cf_arc_aux_roundtrip
#endif
struct ForeignClaim {
  HopCFRef value;
  ~ForeignClaim() {if (value) CFRelease(value);}
};
#if !__has_feature(objc_arc)
struct ForeignReceiverClaim {
  HopObject *value;
  ~ForeignReceiverClaim() {objc_release(value);}
};
#endif
static void cf_pause(void) noexcept __objc3_async {
  __objc3_await objc3::task_yield;
  return;
}
static int cf_sync(__attribute__((cf_consumed)) HopCFRef value) noexcept
    __attribute__((objc_executor(main)));
static int cf_sync(__attribute__((cf_consumed)) HopCFRef value) noexcept {
  ForeignClaim claim{value};
  hop_cf_enter();
  return hop_cf_value(value);
}
static int cf_frame(__attribute__((cf_consumed)) HopCFRef value) noexcept
    __objc3_async_frame __attribute__((objc_executor(main)));
static int cf_frame(__attribute__((cf_consumed)) HopCFRef value) noexcept __objc3_async_frame {
  ForeignClaim claim{value};
  hop_cf_enter();
  __objc3_await cf_pause();
  return hop_cf_value(value);
}
static int cf_task(__attribute__((cf_consumed)) HopCFRef value) noexcept
    __objc3_async __attribute__((objc_executor(main)));
static int cf_task(__attribute__((cf_consumed)) HopCFRef value) noexcept __objc3_async {
  ForeignClaim claim{value};
  hop_cf_enter();
  __objc3_await cf_pause();
  return hop_cf_value(value);
}
typedef int CFConsumedSignature(__attribute__((cf_consumed)) HopCFRef) noexcept;
typedef int CFConsumedFrameSignature(__attribute__((cf_consumed)) HopCFRef) noexcept __objc3_async_frame;
typedef int CFConsumedTaskSignature(__attribute__((cf_consumed)) HopCFRef) noexcept __objc3_async;
static CFConsumedSignature cf_aliased __attribute__((objc_executor(main)));
static CFConsumedFrameSignature cf_aliased_frame __attribute__((objc_executor(main)));
static CFConsumedTaskSignature cf_aliased_task __attribute__((objc_executor(main)));
static int cf_aliased(__attribute__((cf_consumed)) HopCFRef value) noexcept {
  ForeignClaim claim{value}; hop_cf_enter(); return hop_cf_value(value);
}
static int cf_aliased_frame(__attribute__((cf_consumed)) HopCFRef value) noexcept __objc3_async_frame {
  ForeignClaim claim{value}; hop_cf_enter();
  __objc3_await cf_pause();
  return hop_cf_value(value);
}
static int cf_aliased_task(__attribute__((cf_consumed)) HopCFRef value) noexcept __objc3_async {
  ForeignClaim claim{value}; hop_cf_enter();
  __objc3_await cf_pause();
  return hop_cf_value(value);
}
static int cf_unstarted(__attribute__((cf_consumed)) HopCFRef value) __objc3_async {
  ForeignClaim claim{value};
  assert(!"An unused CF task must never enter its body");
  return hop_cf_value(value);
}
struct ForeignParameterCopyFailure {
  ForeignParameterCopyFailure() = default;
  ForeignParameterCopyFailure(const ForeignParameterCopyFailure &) = delete;
  ForeignParameterCopyFailure(ForeignParameterCopyFailure &&) {throw 81;}
};
static int cf_failed_copy(__attribute__((cf_consumed)) HopCFRef value,
                          ForeignParameterCopyFailure failure) __objc3_async {
  ForeignClaim claim{value}; (void)failure;
  assert(!"A failed CF task parameter copy must never enter its body");
  return hop_cf_value(value);
}
static objc3::task<int> cf_ordinary_unstarted(__attribute__((cf_consumed)) HopCFRef value) {
  ForeignClaim claim{value};
  assert(!"An unused ordinary CF coroutine must never enter its body");
  co_return hop_cf_value(value);
}
static objc3::task<int> cf_ordinary_failed_copy(__attribute__((cf_consumed)) HopCFRef value,
                                              ForeignParameterCopyFailure failure) {
  ForeignClaim claim{value}; (void)failure;
  assert(!"A failed ordinary CF coroutine must never enter its body");
  co_return hop_cf_value(value);
}
static objc3::task<int> cf_ordinary_body(__attribute__((cf_consumed)) HopCFRef value,
                                       objc3_executor_t *caller) {
  ForeignClaim claim{value};
  hop_cf_enter_on(caller);
  co_await objc3::task_yield;
  assert(objc3_executor_is_current(caller));
  co_return hop_cf_value(value);
}
static int cf_run_ordinary(objc3_executor_t *caller, int kind) __objc3_async;
static int cf_run_ordinary_function(objc3_executor_t *caller) __objc3_async {
  int result = co_await cf_ordinary_body(hop_cf_source(caller), caller);
  return result;
}
struct OrdinaryBlockParameter {
  bool fail;
  explicit OrdinaryBlockParameter(bool fail) : fail(fail) {}
  OrdinaryBlockParameter(const OrdinaryBlockParameter &) = delete;
  OrdinaryBlockParameter(OrdinaryBlockParameter &&source) : fail(source.fail) {
    if (fail) throw 81;
  }
};
typedef objc3::task<int> (^OrdinaryCFWork)(__attribute__((cf_consumed)) HopCFRef,
                                        OrdinaryBlockParameter);
#if !__has_feature(objc_arc)
struct OrdinaryCFWorkClaim {
  OrdinaryCFWork work;
  ~OrdinaryCFWorkClaim() {_Block_release(work);}
};
#endif
static objc3::task<int> cf_make_ordinary_block(objc3_executor_t *caller, int mode) {
  HopObject *object = hop_mrc_source(caller);
#if !__has_feature(objc_arc)
  ForeignReceiverClaim original{object};
#endif
  OrdinaryCFWork literal = ^objc3::task<int>(__attribute__((cf_consumed)) HopCFRef value,
                                            OrdinaryBlockParameter parameter) {
    ForeignClaim claim{value};
    assert(mode != 0 && !parameter.fail);
    hop_cf_enter_on(caller);
    co_await objc3::task_yield;
    assert(objc3_executor_is_current(caller));
    co_return hop_cf_value(value)+[object number]-42;
  };
#if !__has_feature(objc_arc)
  OrdinaryCFWork work = (OrdinaryCFWork)_Block_copy(literal);
  OrdinaryCFWorkClaim owner{work};
#else
  OrdinaryCFWork work = literal;
#endif
  return work(hop_cf_source(caller), OrdinaryBlockParameter(mode == 1));
}
static int cf_run_ordinary_block(objc3_executor_t *caller) __objc3_async {
  auto task = cf_make_ordinary_block(caller, 2);
  int result = co_await std::move(task);
  return result;
}
@interface HopObject (CFConsumption)
+ (int)cfClass:(__attribute__((cf_consumed)) HopCFRef)value
    __attribute__((nothrow, objc_executor(main)));
+ (int)cfClassFrame:(__attribute__((cf_consumed)) HopCFRef)value
    __objc3_async_frame __attribute__((nothrow, objc_executor(main)));
+ (int)cfClassTask:(__attribute__((cf_consumed)) HopCFRef)value
    __objc3_async __attribute__((nothrow, objc_executor(main)));
- (int)cfConsume:(__attribute__((cf_consumed)) HopCFRef)value
    __attribute__((nothrow, ns_consumes_self, objc_executor(main)));
- (int)cfConsumeFrame:(__attribute__((cf_consumed)) HopCFRef)value
    __objc3_async_frame __attribute__((nothrow, ns_consumes_self, objc_executor(main)));
- (int)cfConsumeTask:(__attribute__((cf_consumed)) HopCFRef)value
    __objc3_async __attribute__((nothrow, ns_consumes_self, objc_executor(main)));
- (objc3::task<int>)cfOrdinaryTask:(__attribute__((cf_consumed)) HopCFRef)value
    caller:(objc3_executor_t *)caller __attribute__((ns_consumes_self));
#if __has_feature(objc_arc)
- (objc3::task<int>)cfOrdinaryReplacingTask:(__attribute__((cf_consumed)) HopCFRef)value
    caller:(objc3_executor_t *)caller __attribute__((ns_consumes_self));
#endif
@end
@implementation HopObject (CFConsumption)
+ (int)cfClass:(__attribute__((cf_consumed)) HopCFRef)value {
  ForeignClaim claim{value}; hop_cf_enter(); return hop_cf_value(value);
}
+ (int)cfClassFrame:(__attribute__((cf_consumed)) HopCFRef)value __objc3_async_frame {
  ForeignClaim claim{value}; hop_cf_enter();
  __objc3_await cf_pause();
  return hop_cf_value(value);
}
+ (int)cfClassTask:(__attribute__((cf_consumed)) HopCFRef)value __objc3_async {
  ForeignClaim claim{value}; hop_cf_enter();
  __objc3_await cf_pause();
  return hop_cf_value(value);
}
- (int)cfConsume:(__attribute__((cf_consumed)) HopCFRef)value {
  ForeignClaim claim{value};
#if !__has_feature(objc_arc)
  ForeignReceiverClaim receiver{self};
#endif
  hop_cf_enter(); return hop_cf_value(value)+[self number]-42;
}
- (int)cfConsumeFrame:(__attribute__((cf_consumed)) HopCFRef)value __objc3_async_frame {
  ForeignClaim claim{value};
#if !__has_feature(objc_arc)
  ForeignReceiverClaim receiver{self};
#endif
  hop_cf_enter(); __objc3_await cf_pause();
  return hop_cf_value(value)+[self number]-42;
}
- (int)cfConsumeTask:(__attribute__((cf_consumed)) HopCFRef)value __objc3_async {
  ForeignClaim claim{value};
#if !__has_feature(objc_arc)
  ForeignReceiverClaim receiver{self};
#endif
  hop_cf_enter(); __objc3_await cf_pause();
  return hop_cf_value(value)+[self number]-42;
}
- (objc3::task<int>)cfOrdinaryTask:(__attribute__((cf_consumed)) HopCFRef)value
    caller:(objc3_executor_t *)caller {
  ForeignClaim claim{value};
#if !__has_feature(objc_arc)
  ForeignReceiverClaim receiver{self};
#endif
  hop_cf_enter_on(caller);
  co_await objc3::task_yield;
  assert(objc3_executor_is_current(caller));
  co_return hop_cf_value(value)+[self number]-42;
}
#if __has_feature(objc_arc)
- (objc3::task<int>)cfOrdinaryReplacingTask:(__attribute__((cf_consumed)) HopCFRef)value
    caller:(objc3_executor_t *)caller {
  ForeignClaim claim{value};
  hop_cf_enter_on(caller);
  self = [HopObject new];
  co_await objc3::task_yield;
  assert(objc3_executor_is_current(caller));
  co_return hop_cf_value(value)+[self number]-42;
}
#endif
@end
static int cf_run_ordinary(objc3_executor_t *caller, int kind) __objc3_async {
  if (kind < 6) {
    return __objc3_await cf_run_ordinary_function(caller);
  }
#if __has_feature(objc_arc)
  if (kind >= 8) {
    int value = co_await [hop_mrc_source(caller) cfOrdinaryReplacingTask:hop_cf_source(caller) caller:caller];
    return value;
  }
#endif
  int value = co_await [hop_mrc_source(caller) cfOrdinaryTask:hop_cf_source(caller) caller:caller];
  return value;
}
extern "C" int hop_cf_roundtrip(int kind, objc3_executor_t *caller) __objc3_async_frame {
  int result = 0;
  switch (kind) {
  case 0: result = __objc3_await cf_sync(hop_cf_source(caller)); break;
  case 1: result = __objc3_await cf_frame(hop_cf_source(caller)); break;
  case 2: result = __objc3_await cf_task(hop_cf_source(caller)); break;
  case 3: result = __objc3_await [HopObject cfClass:hop_cf_source(caller)]; break;
  case 4: result = __objc3_await [HopObject cfClassFrame:hop_cf_source(caller)]; break;
  case 5: result = __objc3_await [HopObject cfClassTask:hop_cf_source(caller)]; break;
  case 6: result = __objc3_await [hop_mrc_source(caller) cfConsume:hop_cf_source(caller)]; break;
  case 7: result = __objc3_await [hop_mrc_source(caller) cfConsumeFrame:hop_cf_source(caller)]; break;
  case 8: result = __objc3_await [hop_mrc_source(caller) cfConsumeTask:hop_cf_source(caller)]; break;
  case 9: result = __objc3_await cf_sync(nullptr); break;
  case 10: result = __objc3_await cf_aliased(hop_cf_source(caller)); break;
  case 11: result = __objc3_await cf_aliased_frame(hop_cf_source(caller)); break;
  case 12: result = __objc3_await cf_aliased_task(hop_cf_source(caller)); break;
  default: assert(0);
  }
  assert(objc3_executor_is_current(caller));
  return result;
}
extern "C" int hop_cf_aux_roundtrip(int kind, objc3_executor_t *caller) __objc3_async_frame {
  if (kind >= 12) {
    return __objc3_await cf_run_ordinary_block(caller);
  } else if (kind == 10) {
    auto unused = cf_make_ordinary_block(caller, 0);
    (void)unused;
  } else if (kind == 11) {
    bool caught = false;
    try {
      auto failed = cf_make_ordinary_block(caller, 1);
      (void)failed;
    } catch (int value) {assert(value == 81); caught = true;}
    assert(caught);
  } else if (kind >= 4) {
    return __objc3_await cf_run_ordinary(caller, kind);
  } else if (kind % 2 == 0) {
    if (kind == 0) {
      auto unused = cf_unstarted(hop_cf_source(caller));
      (void)unused;
    } else {
      auto unused = cf_ordinary_unstarted(hop_cf_source(caller));
      (void)unused;
    }
  } else {
    bool caught = false;
    try {
      if (kind == 1) {
        auto failed = cf_failed_copy(hop_cf_source(caller), ForeignParameterCopyFailure{});
        (void)failed;
      } else {
        auto failed = cf_ordinary_failed_copy(hop_cf_source(caller), ForeignParameterCopyFailure{});
        (void)failed;
      }
    } catch (int value) {assert(value == 81); caught = true;}
    assert(caught);
  }
  assert(objc3_executor_is_current(caller));
  return 42;
}
