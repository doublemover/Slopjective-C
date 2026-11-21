#include "cf-contract.h"
#include "objc3_task.hpp"
#include <assert.h>
#include <objc/runtime.h>
#include <objc/objc-arc.h>
#if __has_feature(objc_arc)
#define hop_integrated_cf_reset hop_arc_integrated_cf_reset
#define hop_integrated_cf_entries hop_arc_integrated_cf_entries
#define hop_integrated_cf_completed hop_arc_integrated_cf_completed
#define hop_integrated_cf_created hop_arc_integrated_cf_created
#define hop_integrated_cf_release_owner hop_arc_integrated_cf_release_owner
#define hop_integrated_cf_roundtrip hop_arc_integrated_cf_roundtrip
#define hop_integrated_cf_task_roundtrip hop_arc_integrated_cf_task_roundtrip
#define hop_integrated_cf_unstarted hop_arc_integrated_cf_unstarted
#endif
// The storage is an Objective-C object. Its explicit CF claims use the same
// standard CF ABI as the opaque-pointer fixture, with independent ARC slots.
typedef const struct HopIntegratedObject *IntegratedRef
    __attribute__((NSObject, objc_unsafe_sendable));
typedef IntegratedRef IntegratedAlias;
static int entries, completed, created;
static IntegratedRef borrowed_owner;
extern "C" void hop_integrated_cf_release_owner(void) {
  if (borrowed_owner) CFRelease(borrowed_owner);
  borrowed_owner = 0;
}
extern "C" void hop_integrated_cf_reset(void) {
  assert(!borrowed_owner);
  __atomic_store_n(&entries, 0, __ATOMIC_RELEASE);
  __atomic_store_n(&completed, 0, __ATOMIC_RELEASE);
  __atomic_store_n(&created, 0, __ATOMIC_RELEASE);
}
extern "C" int hop_integrated_cf_entries(void) {return __atomic_load_n(&entries, __ATOMIC_ACQUIRE);}
extern "C" int hop_integrated_cf_completed(void) {return __atomic_load_n(&completed, __ATOMIC_ACQUIRE);}
extern "C" int hop_integrated_cf_created(void) {return __atomic_load_n(&created, __ATOMIC_ACQUIRE);}
static IntegratedRef source(void) noexcept __attribute__((nothrow, cf_returns_retained));
static IntegratedRef source(void) noexcept {
  HopObject *object = [HopObject new];
  assert(object);
  __atomic_fetch_add(&created, 1, __ATOMIC_RELEASE);
  const void *claim = CFRetain((__bridge const void *)object);
#if !__has_feature(objc_arc)
  objc_release(object);
#endif
  return static_cast<IntegratedRef>(claim);
}
struct Claim {
  const void *value;
  ~Claim() {if (value) CFRelease(value);}
};
static int number(IntegratedRef value) {
  return value ? [(__bridge HopObject *)value number] : 0;
}
static void enter(void) {
  objc3_executor_t *main = nullptr;
  assert(objc3_executor_get_main(&main) == OBJC3_SERVICE_OK);
  assert(objc3_executor_is_current(main));
  objc3_executor_release(main);
  __atomic_fetch_add(&entries, 1, __ATOMIC_RELEASE);
}
static void complete(void) {__atomic_fetch_add(&completed, 1, __ATOMIC_RELEASE);}
static int pause_frame(void) __objc3_async {__objc3_await objc3::task_yield;return 0;}
static int consume_frame(IntegratedAlias __attribute__((cf_consumed)) value, bool replace)
    noexcept __objc3_async_frame __attribute__((nothrow, objc_executor(main)));
static int consume_frame(IntegratedAlias __attribute__((cf_consumed)) value, bool replace)
    noexcept __objc3_async_frame {
  enter();
  Claim original{value};
  IntegratedRef replacement = replace ? source() : nullptr;
  Claim replacement_claim{replace ? replacement : nullptr};
  if (replace) value = replacement;
  __objc3_await pause_frame();
  complete();
  return number(value);
}
static int consume_task(IntegratedAlias __attribute__((cf_consumed)) value, bool replace)
    noexcept __objc3_async __attribute__((nothrow, objc_executor(main)));
static int consume_task(IntegratedAlias __attribute__((cf_consumed)) value, bool replace)
    noexcept __objc3_async {
  enter();
  Claim original{value};
  IntegratedRef replacement = replace ? source() : nullptr;
  Claim replacement_claim{replace ? replacement : nullptr};
  if (replace) value = replacement;
  __objc3_await pause_frame();
  complete();
  return number(value);
}
static IntegratedRef result_frame(bool borrowed, bool null_value) noexcept __objc3_async_frame
    __attribute__((nothrow, cf_returns_retained, objc_executor(main)));
static IntegratedRef result_frame(bool borrowed, bool null_value) noexcept __objc3_async_frame {
  enter();__objc3_await pause_frame();complete();
  if (null_value) return nullptr;
  return borrowed ? static_cast<IntegratedRef>(CFRetain(borrowed_owner)) : source();
}
static IntegratedRef borrowed_frame(void) noexcept __objc3_async_frame
    __attribute__((nothrow, cf_returns_not_retained, objc_executor(main)));
static IntegratedRef borrowed_frame(void) noexcept __objc3_async_frame {
  enter();__objc3_await pause_frame();complete();return borrowed_owner;
}
static IntegratedRef result_task(bool null_value) noexcept __objc3_async
    __attribute__((nothrow, cf_returns_retained, objc_executor(main)));
static IntegratedRef result_task(bool null_value) noexcept __objc3_async {
  enter();__objc3_await pause_frame();complete();return null_value ? nullptr : source();
}
static IntegratedRef borrowed_task(void) noexcept __objc3_async
    __attribute__((nothrow, cf_returns_not_retained, objc_executor(main)));
static IntegratedRef borrowed_task(void) noexcept __objc3_async {
  enter();__objc3_await pause_frame();complete();return borrowed_owner;
}
static IntegratedRef object_frame(void) noexcept __objc3_async_frame
    __attribute__((nothrow,objc_executor(main)));
static IntegratedRef object_frame(void) noexcept __objc3_async_frame {
  enter();__objc3_await pause_frame();complete();return borrowed_owner;
}
static IntegratedRef object_task(void) noexcept __objc3_async
    __attribute__((nothrow,objc_executor(main)));
static IntegratedRef object_task(void) noexcept __objc3_async {
  enter();__objc3_await pause_frame();complete();return borrowed_owner;
}
static int consume_sync(IntegratedRef __attribute__((cf_consumed)) value) noexcept
    __attribute__((nothrow,objc_executor(main)));
static int consume_sync(IntegratedRef __attribute__((cf_consumed)) value) noexcept {
  enter();Claim claim{value};int result=number(value);complete();return result;
}
static IntegratedRef owned_sync(void) noexcept
    __attribute__((nothrow,cf_returns_retained,objc_executor(main)));
static IntegratedRef owned_sync(void) noexcept {enter();complete();return source();}
static IntegratedRef borrowed_sync(void) noexcept
    __attribute__((nothrow,cf_returns_not_retained,objc_executor(main)));
static IntegratedRef borrowed_sync(void) noexcept {enter();complete();return borrowed_owner;}
static IntegratedRef object_sync(void) noexcept __attribute__((nothrow,objc_executor(main)));
static IntegratedRef object_sync(void) noexcept {enter();complete();return borrowed_owner;}
static IntegratedRef ns_source_owned(void) noexcept __attribute__((nothrow,ns_returns_retained));
static IntegratedRef ns_source_owned(void) noexcept {
  IntegratedRef value=source();Claim original{value};
#if __has_feature(objc_arc)
  return value;
#else
  return static_cast<IntegratedRef>(CFRetain(value));
#endif
}
static IntegratedRef ns_owned_sync(void) noexcept
    __attribute__((nothrow,ns_returns_retained,objc_executor(main)));
static IntegratedRef ns_owned_sync(void) noexcept {enter();complete();return ns_source_owned();}
static IntegratedRef ns_owned_frame(void) noexcept __objc3_async_frame
    __attribute__((nothrow,ns_returns_retained,objc_executor(main)));
static IntegratedRef ns_owned_frame(void) noexcept __objc3_async_frame {
  enter();__objc3_await pause_frame();complete();return ns_source_owned();
}
static IntegratedRef ns_owned_task(void) noexcept __objc3_async
    __attribute__((nothrow,ns_returns_retained,objc_executor(main)));
static IntegratedRef ns_owned_task(void) noexcept __objc3_async {
  enter();__objc3_await pause_frame();complete();return ns_source_owned();
}
#if __has_feature(objc_arc)
#define NS_CONSUMED_CLAIM
#else
struct ObjectClaim {id value;~ObjectClaim(){objc_release(value);}};
#define NS_CONSUMED_CLAIM ObjectClaim original{(__bridge id)value};
#endif
static int ns_consume_sync(IntegratedRef __attribute__((ns_consumed)) value) noexcept
    __attribute__((nothrow,objc_executor(main)));
static int ns_consume_sync(IntegratedRef __attribute__((ns_consumed)) value) noexcept {
  enter();NS_CONSUMED_CLAIM int result=number(value);complete();return result;
}
static int ns_consume_strong_frame(IntegratedRef __attribute__((ns_consumed)) value) noexcept __objc3_async_frame
    __attribute__((nothrow,objc_executor(main)));
static int ns_consume_strong_frame(IntegratedRef __attribute__((ns_consumed)) value) noexcept __objc3_async_frame {
  enter();NS_CONSUMED_CLAIM __objc3_await pause_frame();complete();return number(value);
}
static int ns_consume_strong_task(IntegratedRef __attribute__((ns_consumed)) value) noexcept __objc3_async
    __attribute__((nothrow,objc_executor(main)));
static int ns_consume_strong_task(IntegratedRef __attribute__((ns_consumed)) value) noexcept __objc3_async {
  enter();NS_CONSUMED_CLAIM __objc3_await pause_frame();complete();return number(value);
}
static int ns_consume_unsafe_frame(__unsafe_unretained IntegratedRef __attribute__((ns_consumed)) value) noexcept __objc3_async_frame
    __attribute__((nothrow,objc_executor(main)));
static int ns_consume_unsafe_frame(__unsafe_unretained IntegratedRef __attribute__((ns_consumed)) value) noexcept __objc3_async_frame {
  enter();NS_CONSUMED_CLAIM __objc3_await pause_frame();complete();return number(value);
}
static int ns_consume_unsafe_task(__unsafe_unretained IntegratedRef __attribute__((ns_consumed)) value) noexcept __objc3_async
    __attribute__((nothrow,objc_executor(main)));
static int ns_consume_unsafe_task(__unsafe_unretained IntegratedRef __attribute__((ns_consumed)) value) noexcept __objc3_async {
  enter();NS_CONSUMED_CLAIM __objc3_await pause_frame();complete();return number(value);
}
static int ns_consume_weak_frame(__weak IntegratedRef __attribute__((ns_consumed)) value) noexcept __objc3_async_frame
    __attribute__((nothrow,objc_executor(main)));
static int ns_consume_weak_frame(__weak IntegratedRef __attribute__((ns_consumed)) value) noexcept __objc3_async_frame {
  enter();NS_CONSUMED_CLAIM __objc3_await pause_frame();complete();return number(value);
}
static int ns_consume_weak_task(__weak IntegratedRef __attribute__((ns_consumed)) value) noexcept __objc3_async
    __attribute__((nothrow,objc_executor(main)));
static int ns_consume_weak_task(__weak IntegratedRef __attribute__((ns_consumed)) value) noexcept __objc3_async {
  enter();NS_CONSUMED_CLAIM __objc3_await pause_frame();complete();return number(value);
}
static int cf_consume_weak_frame(__weak IntegratedRef __attribute__((cf_consumed)) value) noexcept __objc3_async_frame
    __attribute__((nothrow,objc_executor(main)));
static int cf_consume_weak_frame(__weak IntegratedRef __attribute__((cf_consumed)) value) noexcept __objc3_async_frame {
  enter();Claim original{value};__objc3_await pause_frame();complete();return number(value);
}
static int cf_consume_weak_task(__weak IntegratedRef __attribute__((cf_consumed)) value) noexcept __objc3_async
    __attribute__((nothrow,objc_executor(main)));
static int cf_consume_weak_task(__weak IntegratedRef __attribute__((cf_consumed)) value) noexcept __objc3_async {
  enter();Claim original{value};__objc3_await pause_frame();complete();return number(value);
}
static int cf_consume_unsafe_frame(__unsafe_unretained IntegratedRef __attribute__((cf_consumed)) value) noexcept __objc3_async_frame
    __attribute__((nothrow,objc_executor(main)));
static int cf_consume_unsafe_frame(__unsafe_unretained IntegratedRef __attribute__((cf_consumed)) value) noexcept __objc3_async_frame {
  enter();Claim original{value};__objc3_await pause_frame();complete();return number(value);
}
static int cf_consume_unsafe_task(__unsafe_unretained IntegratedRef __attribute__((cf_consumed)) value) noexcept __objc3_async
    __attribute__((nothrow,objc_executor(main)));
static int cf_consume_unsafe_task(__unsafe_unretained IntegratedRef __attribute__((cf_consumed)) value) noexcept __objc3_async {
  enter();Claim original{value};__objc3_await pause_frame();complete();return number(value);
}
#define INTEGRATED_CALLER_BODY \
  if (kind == 5 || kind == 8 || kind == 10 || kind == 11 || kind == 14 || kind == 15) borrowed_owner = source(); \
  int result = 0; \
  switch (kind) { \
  case 0: result = __objc3_await consume_frame(source(), false); break; \
  case 1: result = __objc3_await consume_task(source(), false); break; \
  case 2: result = __objc3_await consume_frame(source(), true); break; \
  case 3: result = __objc3_await consume_task(source(), true); break; \
  case 4: result = number(__objc3_await result_frame(false, false)); break; \
  case 5: result = number(__objc3_await borrowed_frame()); break; \
  case 6: result = number(__objc3_await result_frame(false, true)); break; \
  case 7: result = number(__objc3_await result_task(false)); break; \
  case 8: result = number(__objc3_await borrowed_task()); break; \
  case 9: result = number(__objc3_await result_task(true)); break; \
  case 10: result = number(__objc3_await object_frame()); break; \
  case 11: result = number(__objc3_await object_task()); break; \
  case 12: result = __objc3_await consume_sync(source()); break; \
  case 13: result = number(__objc3_await owned_sync()); break; \
  case 14: result = number(__objc3_await borrowed_sync()); break; \
  case 15: result = number(__objc3_await object_sync()); break; \
  case 16: result = number(__objc3_await ns_owned_sync()); break; \
  case 17: result = number(__objc3_await ns_owned_frame()); break; \
  case 18: result = number(__objc3_await ns_owned_task()); break; \
  case 19: result = __objc3_await ns_consume_sync(ns_source_owned()); break; \
  case 20: result = __objc3_await ns_consume_strong_frame(ns_source_owned()); break; \
  case 21: result = __objc3_await ns_consume_strong_task(ns_source_owned()); break; \
  case 22: result = __objc3_await ns_consume_unsafe_frame(ns_source_owned()); break; \
  case 23: result = __objc3_await ns_consume_unsafe_task(ns_source_owned()); break; \
  case 24: result = __objc3_await ns_consume_weak_frame(ns_source_owned()); break; \
  case 25: result = __objc3_await ns_consume_weak_task(ns_source_owned()); break; \
  case 26: result = __objc3_await cf_consume_weak_frame(source()); break; \
  case 27: result = __objc3_await cf_consume_weak_task(source()); break; \
  case 28: result = __objc3_await cf_consume_unsafe_frame(source()); break; \
  case 29: result = __objc3_await cf_consume_unsafe_task(source()); break; \
  default: assert(0); \
  } \
  assert(objc3_executor_is_current(destination)); \
  return result;
extern "C" int hop_integrated_cf_roundtrip(int kind, objc3_executor_t *destination) __objc3_async_frame {
  INTEGRATED_CALLER_BODY
}
static int task_caller(int kind, objc3_executor_t *destination) __objc3_async {INTEGRATED_CALLER_BODY}
extern "C" int hop_integrated_cf_task_roundtrip(int kind, objc3_executor_t *destination) __objc3_async_frame {
  return __objc3_await task_caller(kind, destination);
}
static int unstarted_frame(IntegratedRef __attribute__((cf_consumed)) value) __objc3_async_frame {
  Claim claim{value};assert(!"unused CF frame entered");return 0;
}
static int unstarted_task(IntegratedRef __attribute__((cf_consumed)) value) __objc3_async {
  Claim claim{value};assert(!"unused CF task entered");return 0;
}
extern "C" objc3_async_frame hop_integrated_cf_unstarted(int task) {
  return task ? __objc3_async_to_frame<int>(unstarted_task(source())) : unstarted_frame(source());
}
#undef INTEGRATED_CALLER_BODY
