#ifndef NATIVE_C_ASYNC_PROVIDER_H
#define NATIVE_C_ASYNC_PROVIDER_H
#include "objc3_runtime_services.h"
struct native_c_pair { int left; double right; };
struct __attribute__((aligned(256))) native_c_wide { unsigned char bytes[256]; };
typedef __complex__ double native_c_complex;
#ifdef __cplusplus
extern "C" {
#endif
#if defined(__cplusplus) && !defined(__OBJC__)
objc3_async_frame native_c_leaf(int value, int *cleanups);
objc3_async_frame native_c_chain(int count, int *cleanups);
objc3_async_frame native_c_pair_value(struct native_c_pair value);
objc3_async_frame native_c_wide_value(struct native_c_wide value);
objc3_async_frame native_c_complex_value(native_c_complex value);
objc3_async_frame native_c_empty(int *cleanups);
objc3_async_frame native_c_external_leaf(int value);
objc3_async_frame native_c_roundtrip(int value);
objc3_async_frame native_c_oom_parent(void);
#else
int native_c_leaf(int value, int *cleanups) __objc3_async_frame;
int native_c_chain(int count, int *cleanups) __objc3_async_frame;
struct native_c_pair native_c_pair_value(struct native_c_pair value) __objc3_async_frame;
struct native_c_wide native_c_wide_value(struct native_c_wide value) __objc3_async_frame;
native_c_complex native_c_complex_value(native_c_complex value) __objc3_async_frame;
void native_c_empty(int *cleanups) __objc3_async_frame;
int native_c_external_leaf(int value) __objc3_async_frame;
int native_c_roundtrip(int value) __objc3_async_frame;
int native_c_oom_parent(void) __objc3_async_frame;
#endif
objc3_async_frame native_c_force_oom(void);
objc3_async_frame native_c_make_method_frame(int mode, int *destroyed);
objc3_async_frame native_c_make_block_frame(int value, int *destroyed);
objc3_async_frame native_c_make_nil_frame(void);
objc3_async_frame native_c_make_nil_await_frame(void);
#ifdef __OBJC__
@protocol Error @end
@protocol NativeCError <Error> @end
typedef int (^native_c_block)(int);
id native_c_object(id object, int *cleanups) __objc3_async_frame;
native_c_block native_c_block_value(int value, int *cleanups) __objc3_async_frame;
int native_c_throw(id<NativeCError> error) __objc3_async_frame __objc3_throws(id<NativeCError>);
int native_c_propagate(id<NativeCError> error) __objc3_async_frame __objc3_throws(id<NativeCError>);
int native_c_recover(id<NativeCError> error, int *cleanups) __objc3_async_frame;
int native_c_foreign(id exception, int *cleanups) __objc3_async_frame;
@protocol NativeCAsyncMethods
- (int)value:(int)delta __objc3_async_frame;
@end
__attribute__((objc_root_class))
@interface NativeCAsyncWorker <NativeCAsyncMethods> {
  Class isa;
@public
  int number;
  int *destroyed;
}
+ (id)new;
+ (int)classValue __objc3_async_frame;
- (int)value:(int)delta __objc3_async_frame;
- (int)directValue:(int)delta __objc3_async_frame __attribute__((objc_direct));
@end
typedef int (^NativeCAsyncBlock)(int) __objc3_async_frame;
#endif
#ifdef __cplusplus
}
#endif
#endif
