#include <objc3c/runtime/objc3_task.hpp>

int native_module_value(int value) __objc3_async {
  __objc3_await objc3::task_yield;
  return value + 1;
}
