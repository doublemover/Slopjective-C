#include "cf-contract.h"
#include <assert.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <stdint.h>
#include <objc/runtime.h>
#include <objc/objc-arc.h>
#define HOP_CF_MAGIC UINT64_C(0x4f3343465f4d414e)
struct HopCFObject { uintptr_t magic; _Atomic unsigned claims; int value; };
static int is_plain_cf(const void *opaque) {
  uintptr_t tag;
  __builtin_memcpy(&tag, opaque, sizeof(tag));
  return tag == HOP_CF_MAGIC;
}
static _Atomic int arguments, entries, destroyed, live;
static _Thread_local int fail_allocation;
extern void hop_fail_second_frame_allocation(void);
void hop_cf_reset(int fail) {
  assert(atomic_load(&live) == 0);
  atomic_store(&arguments, 0);
  atomic_store(&entries, 0);
  fail_allocation = fail;
}
int hop_cf_arguments(void) {return atomic_load(&arguments);}
int hop_cf_entries(void) {return atomic_load(&entries);}
int hop_cf_destroyed(void) {return atomic_load(&destroyed);}
int hop_cf_live(void) {return atomic_load(&live);}
HopCFRef hop_cf_source(objc3_executor_t *caller) {
  assert(objc3_executor_is_current(caller));
  struct HopCFObject *value = calloc(1, sizeof(*value));
  assert(value);
  value->magic = HOP_CF_MAGIC;
  atomic_init(&value->claims, 1);
  value->value = 42;
  atomic_fetch_add(&arguments, 1);
  atomic_fetch_add(&live, 1);
  if (fail_allocation) {
    int fail = fail_allocation;
    fail_allocation = 0;
    if (fail == 2) hop_fail_second_frame_allocation();
    else hop_fail_next_frame_allocation();
  }
  return value;
}
const void *CFRetain(const void *opaque) {
  assert(opaque); // Compiler-owned optional slots must skip null.
  if (!is_plain_cf(opaque)) return (__bridge const void *)objc_retain((__bridge id)opaque);
  struct HopCFObject *value = (struct HopCFObject *)opaque;
  unsigned before = atomic_fetch_add(&value->claims, 1);
  assert(before > 0);
  return opaque;
}
void CFRelease(const void *opaque) {
  assert(opaque);
  if (!is_plain_cf(opaque)) {objc_release((__bridge id)opaque);return;}
  struct HopCFObject *value = (struct HopCFObject *)opaque;
  unsigned before = atomic_fetch_sub(&value->claims, 1);
  assert(before > 0);
  if (before == 1) {
    atomic_fetch_add(&destroyed, 1);
    atomic_fetch_sub(&live, 1);
    free(value);
  }
}
int hop_cf_value(HopCFRef value) {
  if (!value) return 0;
  assert(atomic_load(&value->claims) > 0);
  return value->value;
}
void hop_cf_enter(void) {
  objc3_executor_t *main = 0;
  assert(objc3_executor_get_main(&main) == OBJC3_SERVICE_OK);
  hop_cf_enter_on(main);
  objc3_executor_release(main);
}
void hop_cf_enter_on(objc3_executor_t *destination) {
  assert(objc3_executor_is_current(destination));
  atomic_fetch_add(&entries, 1);
}
