#pragma once

#include "runtime/public/objc3_runtime_dispatch_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Narrow plain i32 dispatch entrypoint used by lowered runnable sends. Callers
 * that need status/diagnostic ownership should use
 * objc3_runtime_dispatch_i32_checked(); callers that need non-i32 return
 * shapes should use objc3_runtime_dispatch_typed_checked().
 */
int objc3_runtime_dispatch_i32(int receiver, const char *selector, int a0,
                               int a1, int a2, int a3);

int objc3_runtime_dispatch_i32_error_out(
    int receiver, const char *selector, int a0, int a1, int a2, int a3,
    int *throws_error_out);

/*
 * Plain i32 dispatch that starts method lookup at an explicit class name.
 * Lowered super sends use this with the statically resolved superclass.
 */
int objc3_runtime_dispatch_i32_from_class(int receiver,
                                          const char *lookup_start_class_name,
                                          const char *selector, int a0,
                                          int a1, int a2, int a3);

int objc3_runtime_dispatch_i32_from_class_error_out(
    int receiver, const char *lookup_start_class_name, const char *selector,
    int a0, int a1, int a2, int a3, int *throws_error_out);

/*
 * Plain typed-value dispatch used by source lowering when semantic analysis
 * knows that the selected Objective-C method returns a non-i32 runtime value
 * shape. It executes the checked typed ABI, verifies the observed return kind,
 * and projects the matching value field back into the compiler's i32 internal
 * value lane. Mismatched, unsupported, or strict-error results abort with the
 * same runtime-owned diagnostics as plain i32 dispatch.
 */
int objc3_runtime_dispatch_typed_value(
    objc3_runtime_dispatch_return_kind_code expected_return_kind,
    int receiver, const char *selector, int a0, int a1, int a2, int a3);

int objc3_runtime_dispatch_typed_value_error_out(
    objc3_runtime_dispatch_return_kind_code expected_return_kind,
    int receiver, const char *selector, int a0, int a1, int a2, int a3,
    int *throws_error_out);

/*
 * Typed-value dispatch with an explicit class-name lookup start for lowered
 * super-send paths that need a statically resolved superclass.
 */
int objc3_runtime_dispatch_typed_value_from_class(
    objc3_runtime_dispatch_return_kind_code expected_return_kind, int receiver,
    const char *lookup_start_class_name, const char *selector, int a0, int a1,
    int a2, int a3);

int objc3_runtime_dispatch_typed_value_from_class_error_out(
    objc3_runtime_dispatch_return_kind_code expected_return_kind, int receiver,
    const char *lookup_start_class_name, const char *selector, int a0, int a1,
    int a2, int a3, int *throws_error_out);

#ifdef __cplusplus
}
#endif
