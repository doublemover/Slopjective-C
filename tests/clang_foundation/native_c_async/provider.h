#ifndef NATIVE_C_ASYNC_PROVIDER_H
#define NATIVE_C_ASYNC_PROVIDER_H
#include "objc3_runtime_services.h"
struct native_c_pair { int left; double right; };
struct __attribute__((aligned(256))) native_c_wide { unsigned char bytes[256]; };
typedef __complex__ double native_c_complex;
#ifdef __cplusplus
extern "C" {
objc3_async_frame native_c_leaf(int value, int *cleanups);
objc3_async_frame native_c_chain(int count, int *cleanups);
objc3_async_frame native_c_pair_value(struct native_c_pair value);
objc3_async_frame native_c_wide_value(struct native_c_wide value);
objc3_async_frame native_c_complex_value(native_c_complex value);
objc3_async_frame native_c_empty(int *cleanups);
objc3_async_frame native_c_external_leaf(int value);
objc3_async_frame native_c_roundtrip(int value);
#else
int native_c_leaf(int value, int *cleanups) __objc3_async;
int native_c_chain(int count, int *cleanups) __objc3_async;
struct native_c_pair native_c_pair_value(struct native_c_pair value) __objc3_async;
struct native_c_wide native_c_wide_value(struct native_c_wide value) __objc3_async;
native_c_complex native_c_complex_value(native_c_complex value) __objc3_async;
void native_c_empty(int *cleanups) __objc3_async;
int native_c_external_leaf(int value) __objc3_async;
int native_c_roundtrip(int value) __objc3_async;
#endif
objc3_async_frame native_c_make_method_frame(int mode, int *destroyed);
objc3_async_frame native_c_make_block_frame(int value, int *destroyed);
objc3_async_frame native_c_make_nil_frame(void);
objc3_async_frame native_c_make_nil_await_frame(void);
#ifdef __OBJC__
@protocol Error @end
@protocol NativeCError <Error> @end
typedef int (^native_c_block)(int);
#ifdef __cplusplus
objc3_async_frame native_c_object(id object, int *cleanups);
objc3_async_frame native_c_block_value(int value, int *cleanups);
objc3_async_frame native_c_throw(id<NativeCError> error);
objc3_async_frame native_c_propagate(id<NativeCError> error);
objc3_async_frame native_c_recover(id<NativeCError> error, int *cleanups);
objc3_async_frame native_c_foreign(id exception, int *cleanups);
#else
id native_c_object(id object, int *cleanups) __objc3_async;
native_c_block native_c_block_value(int value, int *cleanups) __objc3_async;
int native_c_throw(id<NativeCError> error) __objc3_async __objc3_throws(id<NativeCError>);
int native_c_propagate(id<NativeCError> error) __objc3_async __objc3_throws(id<NativeCError>);
int native_c_recover(id<NativeCError> error, int *cleanups) __objc3_async;
int native_c_foreign(id exception, int *cleanups) __objc3_async;
#endif
#endif
#ifdef __cplusplus
}
#endif
#endif
