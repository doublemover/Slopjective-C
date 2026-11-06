#include "cf-contract.h"
#include "objc3_task.hpp"
#include <assert.h>
#include <objc/runtime.h>
#include <objc/objc-arc.h>
#if __has_feature(objc_arc)
#define CFConsumption CFARCConsumption
#define cfConsume cfARCConsume
#define cfConsumeFrame cfARCConsumeFrame
#define cfConsumeTask cfARCConsumeTask
#define cfClass cfARCClass
#define cfClassFrame cfARCClassFrame
#define cfClassTask cfARCClassTask
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
@end
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
  if (kind == 0) {
    auto unused = cf_unstarted(hop_cf_source(caller));
    (void)unused;
  } else {
    bool caught = false;
    try {
      auto failed = cf_failed_copy(hop_cf_source(caller), ForeignParameterCopyFailure{});
      (void)failed;
    } catch (int value) {assert(value == 81); caught = true;}
    assert(caught);
  }
  assert(objc3_executor_is_current(caller));
  return 42;
}
