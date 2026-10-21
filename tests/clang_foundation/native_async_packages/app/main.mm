#include <objc3c/runtime/objc3_task.hpp>
@import fixture.NativeAsync;

int main() {
  objc3_executor_t *executor = nullptr;
  if (objc3_executor_create(1, &executor) != OBJC3_SERVICE_OK)
    return 1;
  auto pending = native_module_value(41).start(executor);
  uint32_t executed = 0;
  if (objc3_executor_pump(executor, 2, &executed) != OBJC3_SERVICE_OK ||
      executed != 2 || pending.result() != 42)
    return 2;
  pending = {};
  objc3_executor_release(executor);
}
