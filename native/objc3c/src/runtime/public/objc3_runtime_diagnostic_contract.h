#pragma once

#include "runtime/public/objc3_runtime_dispatch_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Strict checked i32 dispatch. Non-OK results carry canonical diagnostic fields
 * and a zero value; OK results carry the returned i32 value.
 */
objc3_runtime_dispatch_i32_result objc3_runtime_dispatch_i32_checked(
    int receiver,
    const char *selector,
    int a0,
    int a1,
    int a2,
    int a3);

#ifdef __cplusplus
}
#endif
