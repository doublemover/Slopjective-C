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
