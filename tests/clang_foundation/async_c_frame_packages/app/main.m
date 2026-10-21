#include <assert.h>
#include <objc3c/runtime/objc3_runtime_services.h>
#include <string.h>
@import fixture.CFrame;

static objc3_service_status copy_int(void *destination, const void *source) {
  memcpy(destination, source, sizeof(int));
  return OBJC3_SERVICE_OK;
}
static void destroy_int(void *value) { (void)value; }
static objc3_service_status move_int(void *destination, void *source) {
  return copy_int(destination, source);
}

int main(void) {
  const objc3_async_result_type expected = {
      sizeof(objc3_async_result_type),
      OBJC3_ASYNC_ABI_VERSION,
      sizeof(int),
      _Alignof(int),
      OBJC3_ASYNC_VALUE,
      0,
      __builtin_objc3_type_identity((int *)0),
      sizeof(__builtin_objc3_type_identity((int *)0)) - 1,
      0};
  const objc3_value_ops consumer = {sizeof(objc3_value_ops),
                                    OBJC3_VALUE_OPS_VERSION,
                                    sizeof(int),
                                    _Alignof(int),
                                    copy_int,
                                    move_int,
                                    destroy_int};
  objc3_async_frame dropped = @fixture.CFrame.c_frame_factory(0);
  assert(objc3_async_frame_dispose(&dropped) == OBJC3_SERVICE_OK);
  assert(!dropped.context && @fixture.CFrame.c_frame_destroyed() == 1);
  objc3_executor_t *executor = 0;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  objc3_async_frame frame = @fixture.CFrame.c_frame_factory(42);
  objc3_task_t *task = 0;
  assert(objc3_async_frame_start(&frame, executor, 0, &task) ==
         OBJC3_SERVICE_OK);
  assert(!frame.context);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 100, &executed) == OBJC3_SERVICE_OK);
  assert(executed == 2);
  int value = 0;
  assert(objc3_task_copy_result(task, &expected, &consumer, &value) ==
         OBJC3_SERVICE_OK);
  assert(value == 42);
  objc3_task_release(task);
  assert(@fixture.CFrame.c_frame_destroyed() == 2);
  objc3_executor_release(executor);
  return 0;
}
