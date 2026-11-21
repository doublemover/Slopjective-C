#include "cf-contract.h"
#include <assert.h>
#if __has_feature(objc_arc)
#define CFResults CFARCResults
#define cfOwnedFrame cfARCOwnedFrame
#define cfBorrowedFrame cfARCBorrowedFrame
#define hop_cf_result_roundtrip hop_cf_arc_result_roundtrip
#define hop_cf_result_reset hop_cf_arc_result_reset
#define hop_cf_result_completed hop_cf_arc_result_completed
#define hop_cf_result_release_owner hop_cf_arc_result_release_owner
#endif
static HopCFRef borrowed_owner;
static int completed;
static _Thread_local int allocation_failure;
void hop_cf_result_release_owner(void) {
  HopCFRef owner = borrowed_owner;
  borrowed_owner = 0;
  if (owner) CFRelease(owner);
}
void hop_cf_result_reset(int failure) {
  assert(!borrowed_owner);
  __atomic_store_n(&completed, 0, __ATOMIC_RELEASE);
  allocation_failure = failure;
}
int hop_cf_result_completed(void) {return __atomic_load_n(&completed, __ATOMIC_ACQUIRE);}
static HopCFRef result_source(void) __attribute__((nothrow, cf_returns_retained));
static HopCFRef result_source(void) {
  objc3_executor_t *main = 0;
  assert(objc3_executor_get_main(&main) == OBJC3_SERVICE_OK);
  HopCFRef value = hop_cf_source(main);
  objc3_executor_release(main);
  return value;
}
static HopCFRef cf_owned(void) __attribute__((nothrow, cf_returns_retained, objc_executor(main)));
static HopCFRef cf_owned(void) {
  hop_cf_enter();
  HopCFRef value = result_source();
  __atomic_fetch_add(&completed, 1, __ATOMIC_RELEASE);
  return value;
}
static HopCFRef cf_borrowed(void) __attribute__((nothrow, cf_returns_not_retained, objc_executor(main)));
static HopCFRef cf_borrowed(void) {
  hop_cf_enter();
  __atomic_fetch_add(&completed, 1, __ATOMIC_RELEASE);
  return borrowed_owner;
}
static void pause_frame(void) __objc3_async_frame {return;}
static HopCFRef cf_owned_frame(void) __objc3_async_frame
    __attribute__((nothrow, cf_returns_retained, objc_executor(main)));
static HopCFRef cf_owned_frame(void) __objc3_async_frame {
  hop_cf_enter();
  __objc3_await pause_frame();
  HopCFRef value = result_source();
  __atomic_fetch_add(&completed, 1, __ATOMIC_RELEASE);
  return value;
}
static HopCFRef cf_borrowed_frame(void) __objc3_async_frame
    __attribute__((nothrow, cf_returns_not_retained, objc_executor(main)));
static HopCFRef cf_borrowed_frame(void) __objc3_async_frame {
  hop_cf_enter();
  __objc3_await pause_frame();
  __atomic_fetch_add(&completed, 1, __ATOMIC_RELEASE);
  return borrowed_owner;
}
static HopCFRef cf_null_frame(void) __objc3_async_frame
    __attribute__((nothrow, cf_returns_retained, objc_executor(main)));
static HopCFRef cf_null_frame(void) __objc3_async_frame {
  hop_cf_enter();
  __objc3_await pause_frame();
  __atomic_fetch_add(&completed, 1, __ATOMIC_RELEASE);
  return 0;
}
@interface HopObject (CFResults)
+ (HopCFRef)cfOwnedFrame __objc3_async_frame
    __attribute__((nothrow, cf_returns_retained, objc_executor(main)));
+ (HopCFRef)cfBorrowedFrame __objc3_async_frame
    __attribute__((nothrow, cf_returns_not_retained, objc_executor(main)));
@end
@implementation HopObject (CFResults)
+ (HopCFRef)cfOwnedFrame __objc3_async_frame {
  hop_cf_enter();
  __objc3_await pause_frame();
  HopCFRef value = result_source();
  __atomic_fetch_add(&completed, 1, __ATOMIC_RELEASE);
  return value;
}
+ (HopCFRef)cfBorrowedFrame __objc3_async_frame {
  hop_cf_enter();
  __objc3_await pause_frame();
  __atomic_fetch_add(&completed, 1, __ATOMIC_RELEASE);
  return borrowed_owner;
}
@end
static HopCFRef claim_result(HopCFRef value) __attribute__((nothrow, cf_returns_retained));
static HopCFRef claim_result(HopCFRef value) {
  return value ? (HopCFRef)CFRetain(value) : 0;
}
int hop_cf_result_roundtrip(int kind, objc3_executor_t *caller) __objc3_async_frame {
  if (kind == 1 || kind == 3 || kind == 5) borrowed_owner = hop_cf_source(caller);
  if (allocation_failure == 1) hop_fail_next_frame_allocation();
  if (allocation_failure == 2) hop_fail_second_frame_allocation();
  allocation_failure = 0;
  HopCFRef value;
  switch (kind) {
  case 0: value = claim_result(__objc3_await cf_owned()); break;
  case 1: value = claim_result(__objc3_await cf_borrowed()); break;
  case 2: value = claim_result(__objc3_await cf_owned_frame()); break;
  case 3: value = claim_result(__objc3_await cf_borrowed_frame()); break;
  case 4: value = claim_result(__objc3_await [HopObject cfOwnedFrame]); break;
  case 5: value = claim_result(__objc3_await [HopObject cfBorrowedFrame]); break;
  case 6: value = claim_result(__objc3_await cf_null_frame()); break;
  default: assert(0); value = 0;
  }
  assert(objc3_executor_is_current(caller));
  int result = hop_cf_value(value);
  if (value) CFRelease(value);
  return result;
}
