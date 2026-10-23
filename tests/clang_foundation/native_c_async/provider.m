#ifndef NATIVE_C_ASYNC_PROVIDER_H
#include "provider.h"
#endif
static void cleaned(int *count) {
  if (count) __atomic_fetch_add(count, 1, __ATOMIC_RELEASE);
}
int native_c_leaf(int value, int *cleanups) __objc3_async {
  __objc3_defer { cleaned(cleanups); }
  return value;
}
int native_c_roundtrip(int value) __objc3_async {
  return __objc3_await native_c_external_leaf(value);
}
int native_c_chain(int count, int *cleanups) __objc3_async {
  int accumulated = 0;
  __objc3_defer { cleaned(cleanups); }
  for (int i = 0; i < count; ++i) {
    int before = accumulated;
    accumulated = before + __objc3_await native_c_leaf(i, cleanups);
  }
  return accumulated;
}
struct native_c_pair native_c_pair_value(struct native_c_pair value) __objc3_async {
  value.left += __objc3_await native_c_leaf(2, 0);
  return value;
}
struct native_c_wide native_c_wide_value(struct native_c_wide value) __objc3_async {
  value.bytes[255] += __objc3_await native_c_leaf(2, 0);
  return value;
}
native_c_complex native_c_complex_value(native_c_complex value) __objc3_async {
  __real__ value += __objc3_await native_c_leaf(2, 0);
  return value;
}
void native_c_empty(int *cleanups) __objc3_async {
  __objc3_defer { cleaned(cleanups); }
  __objc3_await native_c_leaf(0, 0);
}
id native_c_object(id object, int *cleanups) __objc3_async {
  id live = object;
  __objc3_defer { cleaned(cleanups); }
  __objc3_await native_c_leaf(0, 0);
  return live;
}
native_c_block native_c_block_value(int value, int *cleanups) __objc3_async {
  __objc3_defer { cleaned(cleanups); }
  int delta = __objc3_await native_c_leaf(2, 0);
  return ^(int extra) { return value + delta + extra; };
}
int native_c_throw(id<NativeCError> error) __objc3_async __objc3_throws(id<NativeCError>) {
  __objc3_await native_c_leaf(0, 0);
  __objc3_throw(error);
}
int native_c_propagate(id<NativeCError> error) __objc3_async __objc3_throws(id<NativeCError>) {
  return __objc3_try(__objc3_await native_c_throw(error));
}
int native_c_recover(id<NativeCError> error, int *cleanups) __objc3_async {
  __objc3_defer { cleaned(cleanups); }
  __objc3_do {
    return __objc3_try(__objc3_await native_c_propagate(error));
  } __objc3_catch(id<NativeCError> caught) {
    return caught == error ? __objc3_await native_c_leaf(42, 0) : -1;
  }
}
int native_c_foreign(id exception, int *cleanups) __objc3_async {
  __objc3_defer { cleaned(cleanups); }
  __objc3_await native_c_leaf(0, 0);
  @throw exception;
}

// Native-C methods and Blocks share the same carrier boundary as functions.
extern id class_createInstance(Class, __SIZE_TYPE__) __attribute__((ns_returns_retained));
extern id object_dispose(__unsafe_unretained id);
extern void release_source(void *) __asm__("objc_release");
extern const char *sel_getName(SEL);
@implementation NativeCAsyncWorker
+ (id)new { return class_createInstance(self, 0); }
+ (int)classValue __objc3_async { return __objc3_await native_c_leaf(42, 0); }
- (int)value:(int)delta __objc3_async {
  int saved = number;
  SEL selector = _cmd;
  __objc3_await native_c_leaf(0, 0);
  return selector == _cmd && self ? saved + delta : -1;
}
- (int)directValue:(int)delta __objc3_async __attribute__((objc_direct)) {
  SEL selector = _cmd;
  __objc3_await native_c_leaf(0, 0);
  return selector == _cmd && self ? number + delta + 1 : -1;
}
- (void)dealloc {
  if (destroyed) __atomic_fetch_add(destroyed, 1, __ATOMIC_RELEASE);
  object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
@end
@interface NativeCAsyncDerived : NativeCAsyncWorker @end
@implementation NativeCAsyncDerived
- (int)value:(int)delta __objc3_async {
  return 1 + __objc3_await [super value:delta];
}
@end
objc3_async_frame native_c_make_method_frame(int mode, int *destroyed) {
  if (mode == 4) return [NativeCAsyncWorker classValue];
  NativeCAsyncWorker *worker = mode == 3 ? [NativeCAsyncDerived new]
                                       : [NativeCAsyncWorker new];
  worker->number = 40;
  worker->destroyed = destroyed;
  objc3_async_frame frame;
  if (mode == 1) frame = [worker directValue:2];
  else if (mode == 2) {
    id<NativeCAsyncMethods> protocol = worker;
    frame = [protocol value:2];
  } else frame = [worker value:2];
#if !__has_feature(objc_arc)
  release_source((__bridge void *)worker);
#endif
  return frame;
}
objc3_async_frame native_c_make_block_frame(int value, int *destroyed) {
  NativeCAsyncWorker *worker = [NativeCAsyncWorker new];
  worker->number = value;
  worker->destroyed = destroyed;
  __block int calls = 0;
  NativeCAsyncBlock __unsafe_unretained block = ^int(int delta) __objc3_async {
    ++calls;
    int saved = worker->number;
    __objc3_await native_c_leaf(0, 0);
    return saved + delta + calls;
  };
  objc3_async_frame frame = block(2);
#if !__has_feature(objc_arc)
  release_source((__bridge void *)worker);
#endif
  return frame;
}
objc3_async_frame native_c_make_nil_frame(void) {
  NativeCAsyncWorker *worker = 0;
  return [worker value:42];
}
static int native_c_nil_await(void) __objc3_async {
  NativeCAsyncWorker *worker = 0;
  return __objc3_await [worker value:42];
}
objc3_async_frame native_c_make_nil_await_frame(void) { return native_c_nil_await(); }
