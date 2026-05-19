#pragma once

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

/*
 * Plain i32 dispatch that starts method lookup at an explicit class name.
 * Lowered super sends use this with the statically resolved superclass.
 */
int objc3_runtime_dispatch_i32_from_class(int receiver,
                                          const char *lookup_start_class_name,
                                          const char *selector, int a0,
                                          int a1, int a2, int a3);

#ifdef __cplusplus
}
#endif
