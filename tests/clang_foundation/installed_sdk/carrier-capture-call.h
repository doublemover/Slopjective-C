#pragma once
#include "carrier-nested-call.h"
typedef int (^CapturedState)(void);
typedef NestedStrong (^CapturedValue)(void);
typedef NestedStrong (^CapturedThrowing)(int, int) __objc3_throws(id<Error>);
CapturedState capture_strong(int key);
CapturedState capture_weak(id object);
CapturedState capture_block_payload(int key);
CapturedState capture_binding(int key);
CapturedState capture_nested_block(int key);
CapturedValue capture_value(int key);
CapturedThrowing capture_throwing(int key);
int capture_noescape(int key);
#ifdef __cplusplus
extern "C" {
#endif
void capture_native_throw(int key);
id capture_native_object(int key) __attribute__((ns_returns_retained));
void capture_unwind_bridge(int key);
int capture_native_catch(int key);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
CapturedState capture_partial(int key, int mode, int objective_c);
#endif
