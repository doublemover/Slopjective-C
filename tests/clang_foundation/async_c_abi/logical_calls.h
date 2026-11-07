#ifndef ASYNC_LOGICAL_CALLS_H
#define ASYNC_LOGICAL_CALLS_H

#include "bridge.h"

#if !defined(__OBJC__) || defined(__cplusplus)
#error This witness requires Objective-C 3 with the C17 base language
#endif

// Each asm symbol is an actual C frame factory, never a C++ task<T> entry.
// Canonical types retain the logical result; evaluated calls produce a frame.
int logical_c_int(int, int, int *) __objc3_async __asm__("bridge_c_int");
struct bridge_pair logical_c_pair(struct bridge_pair, int *)
    __objc3_async __asm__("bridge_c_pair");
struct bridge_wide logical_c_wide(int *) __objc3_async __asm__("bridge_c_wide");
bridge_complex logical_c_complex(bridge_complex, int *)
    __objc3_async __asm__("bridge_c_complex");
void logical_c_void(int *) __objc3_async __asm__("bridge_c_void");
id logical_c_object(int, int, int *) __objc3_async __asm__("bridge_c_object");
bridge_block logical_c_block(int, int, int *)
    __objc3_async __asm__("bridge_c_block");
int logical_cpp_int(int) __objc3_async __asm__("bridge_cpp_int");
struct bridge_pair logical_cpp_pair(struct bridge_pair)
    __objc3_async __asm__("bridge_cpp_pair");
void logical_cpp_void(void) __objc3_async __asm__("bridge_cpp_void");
id logical_cpp_object(int) __objc3_async __asm__("bridge_cpp_object");
bridge_block logical_cpp_block(int) __objc3_async __asm__("bridge_cpp_block");

typedef int (*logical_c_int_pointer)(int, int, int *) __objc3_async;
typedef int (*logical_cpp_int_pointer)(int) __objc3_async;
typedef void (*logical_void_pointer)(void) __objc3_async;

_Static_assert(__builtin_types_compatible_p(__typeof__(&logical_c_int),
                                           logical_c_int_pointer),
               "logical async pointer types survive a PCH round trip");
_Static_assert(sizeof(logical_c_int(0, 0, 0)) == sizeof(objc3_async_frame),
               "a native C async call physically returns a lazy frame");
_Static_assert(sizeof(logical_c_object(0, 1, 0)) == sizeof(objc3_async_frame),
               "logical object results do not change the frame entry ABI");
#endif
