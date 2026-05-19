#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Narrow plain i32 dispatch entrypoint used by lowered runnable sends. Callers
 * that need status/diagnostic ownership should use
 * objc3_runtime_dispatch_i32_checked().
 */
int objc3_runtime_dispatch_i32(int receiver, const char *selector, int a0,
                               int a1, int a2, int a3);

#ifdef __cplusplus
}
#endif
