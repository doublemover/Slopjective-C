#pragma once

#include "runtime/public/objc3_runtime_dispatch_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Strict checked i32 dispatch. Non-OK results carry canonical diagnostic fields
 * and a zero value; OK results carry the returned i32 value. OK non-i32
 * results fail closed with UNSUPPORTED_RETURN_TYPE instead of projecting to i32.
 */
objc3_runtime_dispatch_i32_result objc3_runtime_dispatch_i32_checked(
    int receiver,
    const char *selector,
    int a0,
    int a1,
    int a2,
    int a3);

/*
 * Strict checked i32 dispatch with an explicit class-name lookup start. Lowered
 * super sends use this after semantic analysis resolves the enclosing
 * superclass.
 */
objc3_runtime_dispatch_i32_result objc3_runtime_dispatch_i32_from_class_checked(
    int receiver,
    const char *lookup_start_class_name,
    const char *selector,
    int a0,
    int a1,
    int a2,
    int a3);

/*
 * Strict checked typed dispatch. OK results carry exactly one typed value field
 * according to return_kind, except void which is represented by return_kind
 * alone. Non-OK results carry canonical diagnostic fields and zero values.
 */
objc3_runtime_dispatch_typed_result objc3_runtime_dispatch_typed_checked(
    int receiver,
    const char *selector,
    int a0,
    int a1,
    int a2,
    int a3);

/*
 * Strict checked typed dispatch with an explicit class-name lookup start.
 */
objc3_runtime_dispatch_typed_result
objc3_runtime_dispatch_typed_from_class_checked(
    int receiver,
    const char *lookup_start_class_name,
    const char *selector,
    int a0,
    int a1,
    int a2,
    int a3);

/*
 * Abort the current process with the canonical strict-dispatch diagnostic for
 * status_code. Lowered unchecked value entrypoints use this after a checked
 * dispatch helper returns a strict-error status envelope.
 */
void objc3_runtime_abort_dispatch_status_i32(int status_code);

#ifdef __cplusplus
}
#endif
