#include "provider.h"
#include "objc3_task.hpp"
static objc3::task<int> foreign_leaf(int value) {
  co_await objc3::task_yield;
  if (value < 0) throw 42;
  co_return value;
}
// A plain C frame factory is consumed by a logical native-C async declaration.
// Its C++ promise and exception_ptr layout never appear in the C provider.
extern "C" objc3_async_frame native_c_external_leaf(int value) {
  return foreign_leaf(value).release_native();
}
