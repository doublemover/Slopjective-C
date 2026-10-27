#include "provider.h"
#include <assert.h>
#include <sched.h>
#include <string.h>
#ifdef __cplusplus
#include "objc3_task.hpp"
#endif
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
#ifdef __cplusplus
static int task_actor_entry(HopCounter *counter,
                            objc3_executor_t *destination) __objc3_async {
  int value = __objc3_await [counter add:1];
  assert(objc3_executor_is_current(destination));
  return value;
}
static int task_packet_entry(HopCounter *counter,
                             objc3_executor_t *destination) __objc3_async {
  HopPacket source={40,{10,20,30}};
  HopPacket packet=__objc3_await hop_packet_on_main(source);
  assert(objc3_executor_is_current(destination));
  packet=__objc3_await [counter transform:packet];
  assert(objc3_executor_is_current(destination));
  assert(source.value==40 && source.weights[2]==30 && packet.weights[2]==32);
  return packet.value;
}
#endif
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

    frame = hop_packet_roundtrip(counter, caller);
    type = frame.result_type;
    task = start(&frame);
    assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
    result = 0;
    assert(objc3_task_copy_result(task, type, &int_ops, &result) == OBJC3_SERVICE_OK);
    assert(result == 42 && hop_packet_arguments() == 1 && hop_packet_entries() == 2);
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

    hop_configure(caller, main_destination, global, named,
                  objc3_actor_identity(actor));
    frame = hop_packet_roundtrip(counter, caller);
    task = start(&frame);
    assert(objc3_executor_pump(caller, 1, &count) == OBJC3_SERVICE_OK && count == 1);
    assert(hop_packet_arguments() == 1);
    objc3_task_request_cancel(task);
    assert(finish(task).kind == OBJC3_COMPLETION_CANCELLED);
    assert(hop_packet_arguments() == 1 && hop_packet_entries() == 0);
    objc3_task_release(task);

#ifdef __cplusplus
    // Exercise a C++ task caller entering the C provider's actor through a
    // generated native frame, then returning to the original destination.
    {
      auto parent = task_actor_entry(counter, caller).start(caller);
      assert(finish(parent.native_handle()).kind == OBJC3_COMPLETION_SUCCESS);
      assert(parent.result() == 43);
    }
    {
      auto parent = task_packet_entry(counter, caller).start(caller);
      assert(finish(parent.native_handle()).kind == OBJC3_COMPLETION_SUCCESS);
      assert(parent.result() == 42);
    }
#endif
    assert(hop_cxx_packet_live() == 0);
    frame = hop_cxx_packet_roundtrip(caller);
    type = frame.result_type;
    task = start(&frame);
    assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
    result = 0;
    assert(objc3_task_copy_result(task, type, &int_ops, &result) == OBJC3_SERVICE_OK);
    assert(result == 42 && hop_cxx_packet_entries() == 1);
    objc3_task_release(task);
    for (unsigned step = 0; step != 1000000 && hop_cxx_packet_live() != 0; ++step)
      sched_yield();
    assert(hop_cxx_packet_live() == 0);
    frame = hop_cxx_packet_roundtrip(caller);
    task = start(&frame);
    assert(objc3_executor_pump(caller, 1, &count) == OBJC3_SERVICE_OK && count == 1);
    objc3_task_request_cancel(task);
    assert(finish(task).kind == OBJC3_COMPLETION_CANCELLED);
    objc3_task_release(task);
    for (unsigned step = 0; step != 1000000 && hop_cxx_packet_live() != 0; ++step)
      sched_yield();
    assert(hop_cxx_packet_entries() == 1 && hop_cxx_packet_live() == 0);
    for (int failed_move = 1; failed_move <= 3; ++failed_move) {
      hop_cxx_packet_fail_move(failed_move);
      frame = hop_cxx_packet_roundtrip(caller);
      task = start(&frame);
      assert(finish(task).kind == OBJC3_COMPLETION_CANCELLED);
      objc3_task_release(task);
      for (unsigned step = 0; step != 1000000 && hop_cxx_packet_live() != 0; ++step)
        sched_yield();
      assert(hop_cxx_packet_entries() == 1 && hop_cxx_packet_live() == 0);
    }
    hop_cxx_packet_fail_move(0);

    assert(hop_cxx_value_live() == 0);
    frame = hop_cxx_value_roundtrip(caller);
    type = frame.result_type;
    task = start(&frame);
    assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
    result = 0;
    assert(objc3_task_copy_result(task, type, &int_ops, &result) == OBJC3_SERVICE_OK);
    assert(result == 42);
    objc3_task_release(task);
    for (unsigned step = 0; step != 1000000 && hop_cxx_value_live() != 0; ++step)
      sched_yield();
    assert(hop_cxx_value_live() == 0);

    frame = hop_cxx_actor_roundtrip(counter, caller);
    type = frame.result_type;
    task = start(&frame);
    assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
    result = 0;
    assert(objc3_task_copy_result(task, type, &int_ops, &result) == OBJC3_SERVICE_OK);
    assert(result == 42);
    objc3_task_release(task);
    for (unsigned step = 0; step != 1000000 &&
         (hop_cxx_value_live() || hop_cxx_packet_live()); ++step)
      sched_yield();
    assert(hop_cxx_value_live() == 0 && hop_cxx_packet_live() == 0);

    for (int kind = 0; kind != 10; ++kind) {
      int expected_objects = (kind == 4 || kind == 8) ? 2 : 1;
      int destroyed_before = hop_destroyed();
      hop_consumed_reset(0);
      frame = hop_consumed_roundtrip(kind, caller);
      type = frame.result_type;
      task = start(&frame);
      assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
      result = 0;
      assert(objc3_task_copy_result(task, type, &int_ops, &result) == OBJC3_SERVICE_OK);
      assert(result == 42 && hop_consumed_arguments() == expected_objects &&
             hop_consumed_entries() == 1);
      objc3_task_release(task);
      for (unsigned step = 0; step != 1000000 &&
           hop_destroyed() != destroyed_before + expected_objects; ++step)
        sched_yield();
      assert(hop_destroyed() == destroyed_before + expected_objects);

      destroyed_before = hop_destroyed();
      hop_consumed_reset(0);
      frame = hop_consumed_roundtrip(kind, caller);
      task = start(&frame);
      assert(objc3_executor_pump(caller, 1, &count) == OBJC3_SERVICE_OK && count == 1);
      assert(hop_consumed_arguments() == expected_objects);
      objc3_task_request_cancel(task);
      assert(finish(task).kind == OBJC3_COMPLETION_CANCELLED);
      assert(hop_consumed_entries() == 0);
      objc3_task_release(task);
      for (unsigned step = 0; step != 1000000 &&
           hop_destroyed() != destroyed_before + expected_objects; ++step)
        sched_yield();
      assert(hop_destroyed() == destroyed_before + expected_objects);

      destroyed_before = hop_destroyed();
      hop_consumed_reset(1);
      frame = hop_consumed_roundtrip(kind, caller);
      task = start(&frame);
      objc3_completion failure = finish(task);
      assert(failure.kind == OBJC3_COMPLETION_ERROR);
      objc3_service_status service = OBJC3_SERVICE_OK;
      assert(objc3_async_error_get_service(failure.error, &service) == OBJC3_SERVICE_OK &&
             service == OBJC3_SERVICE_OUT_OF_MEMORY);
      assert(hop_consumed_entries() == 0 && hop_consumed_arguments() == expected_objects);
      objc3_task_release(task);
      for (unsigned step = 0; step != 1000000 &&
           hop_destroyed() != destroyed_before + expected_objects; ++step)
        sched_yield();
      assert(hop_destroyed() == destroyed_before + expected_objects);
    }
    int snapshot_destroyed_before = hop_destroyed();
    hop_consumed_reset(0);
    frame = hop_consumed_roundtrip(10, caller);
    task = start(&frame);
    assert(finish(task).kind == OBJC3_COMPLETION_CANCELLED);
    assert(hop_consumed_arguments() == 1 && hop_consumed_entries() == 0);
    objc3_task_release(task);
    for (unsigned step = 0; step != 1000000 &&
         hop_destroyed() != snapshot_destroyed_before + 1; ++step)
      sched_yield();
    assert(hop_destroyed() == snapshot_destroyed_before + 1);
    hop_consumed_reset(0);

    int before = hop_destroyed();
    frame = hop_owned_packet_roundtrip(caller);
    type = frame.result_type;
    task = start(&frame);
    assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
    result = 0;
    assert(objc3_task_copy_result(task, type, &int_ops, &result) == OBJC3_SERVICE_OK);
    assert(result == 42);
    objc3_task_release(task);
    for (unsigned step = 0; step != 1000000 && hop_destroyed() == before; ++step)
      sched_yield();
    assert(hop_destroyed() == before + 1);
    before = hop_destroyed();
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
    // Completion publication can precede the global worker's frame/pool
    // cleanup. Observe final result-owner destruction before checking its count.
    for (unsigned step = 0; step != 1000000 && hop_destroyed() == before;
         ++step)
      sched_yield();
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
