#include "provider.h"
#include <assert.h>
#include <sched.h>
#include <string.h>
extern void release_object(void *) __asm__("objc_release");
static objc3_executor_t *caller, *main_destination, *global, *named;
static objc3_service_status copy_int(void *to, const void *from) {
  memcpy(to, from, sizeof(int));
  return OBJC3_SERVICE_OK;
}
static objc3_service_status move_int(void *to, void *from) { return copy_int(to, from); }
static void destroy_int(void *p) { (void)p; }
static const objc3_value_ops int_ops = {sizeof(objc3_value_ops),
                                        OBJC3_VALUE_OPS_VERSION,
                                        sizeof(int),
                                        _Alignof(int),
                                        copy_int,
                                        move_int,
                                        destroy_int};
static objc3_task_t *start(objc3_async_frame *frame) {
  objc3_task_t *task = 0;
  assert(objc3_async_frame_start(frame, caller, 0, &task) == OBJC3_SERVICE_OK);
  return task;
}
static objc3_completion finish(objc3_task_t *task) {
  objc3_completion completion = {sizeof(completion), OBJC3_SERVICE_ABI_VERSION};
  for (unsigned step = 0; step != 1000000; ++step) {
    uint32_t count;
    assert(objc3_executor_pump(main_destination, 32, &count) ==
           OBJC3_SERVICE_OK);
    assert(objc3_executor_pump(named, 32, &count) == OBJC3_SERVICE_OK);
    assert(objc3_executor_pump(caller, 32, &count) == OBJC3_SERVICE_OK);
    objc3_service_status status = objc3_task_get_completion(task, &completion);
    if (status == OBJC3_SERVICE_OK)
      return completion;
    assert(status == OBJC3_SERVICE_NOT_READY);
    sched_yield();
  }
  assert(!"native isolation task did not progress");
  return completion;
}
int main(void) {
  @autoreleasepool {
    assert(objc3_executor_create(1, &caller) == OBJC3_SERVICE_OK);
    assert(objc3_executor_create(1, &named) == OBJC3_SERVICE_OK);
    assert(objc3_executor_get_main(&main_destination) == OBJC3_SERVICE_OK);
    assert(objc3_executor_get_global(&global) == OBJC3_SERVICE_OK);
    assert(objc3_executor_register_named("native.isolation", named) ==
           OBJC3_SERVICE_OK);
    objc3_actor_t *actor = 0;
    assert(objc3_actor_create(named, &actor) == OBJC3_SERVICE_OK);
    void *object = hop_new_counter();
    assert(objc3_actor_bind_object(object, actor) == OBJC3_SERVICE_OK);
    release_object(object); // The explicit association pins the receiver.
    __unsafe_unretained HopCounter *counter = (__bridge HopCounter *)object;
    hop_configure(caller, main_destination, global, named,
                  objc3_actor_identity(actor));
    objc3_async_frame frame = hop_roundtrip(counter, caller);
    const objc3_async_result_type *type = frame.result_type;
    objc3_task_t *task = start(&frame);
    assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
    int result = 0;
    assert(objc3_task_copy_result(task, type, &int_ops, &result) ==
           OBJC3_SERVICE_OK);
    assert(result == 42 && hop_arguments() == 1 && hop_entries() == 3 &&
           hop_cleanups() == 1);
    objc3_task_release(task);

    frame = hop_nil_roundtrip();
    type = frame.result_type;
    task = start(&frame);
    assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
    result = -1;
    assert(objc3_task_copy_result(task, type, &int_ops, &result) ==
           OBJC3_SERVICE_OK);
    assert(result == 0);
    objc3_task_release(task);

    // Cancel after caller-side argument evaluation but before the child enters
    // its target executor. Neither target invocation nor an argument replay is
    // allowed; the parent's lexical cleanup executes once.
    hop_configure(caller, main_destination, global, named,
                  objc3_actor_identity(actor));
    frame = hop_roundtrip(counter, caller);
    task = start(&frame);
    uint32_t count;
    assert(objc3_executor_pump(caller, 1, &count) == OBJC3_SERVICE_OK &&
           count == 1);
    assert(hop_arguments() == 1);
    objc3_task_request_cancel(task);
    assert(finish(task).kind == OBJC3_COMPLETION_CANCELLED);
    assert(hop_arguments() == 1 && hop_entries() == 0 && hop_cleanups() == 1);
    objc3_task_release(task);

    int before = hop_destroyed();
    @autoreleasepool {
      frame = hop_owned_roundtrip();
      type = frame.result_type;
      task = start(&frame);
      assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
      void *owned = 0;
      const objc3_value_ops *ops =
          objc3_async_reference_ops(OBJC3_ASYNC_OBJECT);
      assert(objc3_task_copy_result(task, type, ops, &owned) ==
             OBJC3_SERVICE_OK);
      objc3_task_release(task);
      assert([(__bridge HopObject *)owned number] == 42);
      ops->destroy(&owned);
    }
    assert(hop_destroyed() == before + 1);
    assert(objc3_actor_close(actor) == OBJC3_SERVICE_OK);
    assert(objc3_actor_join(actor) == OBJC3_SERVICE_OK);
    assert(objc3_actor_unbind_object(object) == OBJC3_SERVICE_OK);
    objc3_actor_release(actor);
    assert(objc3_executor_unregister_named("native.isolation") ==
           OBJC3_SERVICE_OK);
    objc3_executor_release(named);
    objc3_executor_release(global);
    objc3_executor_release(main_destination);
    objc3_executor_release(caller);
  }
}
