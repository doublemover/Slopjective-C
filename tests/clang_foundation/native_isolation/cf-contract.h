#ifndef NATIVE_ISOLATION_CF_CONTRACT_H
#define NATIVE_ISOLATION_CF_CONTRACT_H
#include "provider.h"
typedef const struct HopCFObject *HopCFRef;
#ifdef __cplusplus
extern "C" {
#endif
const void *CFRetain(const void *) __attribute__((nothrow, cf_returns_retained));
void CFRelease(const void *) __attribute__((nothrow));
HopCFRef hop_cf_source(objc3_executor_t *) __attribute__((nothrow, cf_returns_retained));
int hop_cf_value(HopCFRef) __attribute__((nothrow));
void hop_cf_enter(void) __attribute__((nothrow));
void hop_cf_reset(int);
int hop_cf_arguments(void);
int hop_cf_entries(void);
int hop_cf_destroyed(void);
int hop_cf_live(void);
#ifdef __cplusplus
}
#endif
#endif
