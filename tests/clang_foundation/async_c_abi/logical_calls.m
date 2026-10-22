#ifndef ASYNC_LOGICAL_CALLS_H
#include "logical_calls.h"
#endif
#include <assert.h>
#include <string.h>

#define TYPE_ID(T) __builtin_objc3_type_identity((T *)0)
#define RESULT_TYPE(T, K)                                                       \
  {sizeof(objc3_async_result_type), OBJC3_ASYNC_ABI_VERSION, sizeof(T),          \
   __alignof__(T), K, 0, TYPE_ID(T), sizeof(TYPE_ID(T)) - 1, 0}
#define VALUE_OPS(NAME, T)                                                      \
  static objc3_service_status NAME##_copy(void *d, const void *s) {             \
    memcpy(d, s, sizeof(T));                                                    \
    return OBJC3_SERVICE_OK;                                                    \
  }                                                                            \
  static objc3_service_status NAME##_move(void *d, void *s) {                   \
    return NAME##_copy(d, s);                                                   \
  }                                                                            \
  static void NAME##_destroy(void *p) { (void)p; }                              \
  static const objc3_value_ops NAME##_ops = {                                    \
      sizeof(objc3_value_ops), OBJC3_VALUE_OPS_VERSION, sizeof(T),               \
      __alignof__(T), NAME##_copy, NAME##_move, NAME##_destroy}

static const objc3_async_result_type int_type =
    RESULT_TYPE(int, OBJC3_ASYNC_VALUE);
static const objc3_async_result_type pair_type =
    RESULT_TYPE(struct bridge_pair, OBJC3_ASYNC_VALUE);
static const objc3_async_result_type wide_type =
    RESULT_TYPE(struct bridge_wide, OBJC3_ASYNC_VALUE);
static const objc3_async_result_type complex_type =
    RESULT_TYPE(bridge_complex, OBJC3_ASYNC_VALUE);
typedef __unsafe_unretained id object_abi;
typedef __unsafe_unretained bridge_block block_abi;
static const objc3_async_result_type object_type =
    RESULT_TYPE(object_abi, OBJC3_ASYNC_OBJECT);
static const objc3_async_result_type block_type =
    RESULT_TYPE(block_abi, OBJC3_ASYNC_BLOCK);
static const objc3_async_result_type void_type = {
    sizeof(objc3_async_result_type), OBJC3_ASYNC_ABI_VERSION, 0, 1,
    OBJC3_ASYNC_VOID, 0, TYPE_ID(void), sizeof(TYPE_ID(void)) - 1, 0};
VALUE_OPS(int, int);
VALUE_OPS(pair, struct bridge_pair);
VALUE_OPS(wide, struct bridge_wide);
VALUE_OPS(complex, bridge_complex);

static objc3_task_t *start(objc3_async_frame *frame, objc3_executor_t *executor,
                           objc3_actor_t *actor, int manual) {
  objc3_task_t *task = 0;
  assert(frame->context && frame->struct_size == sizeof(*frame));
  assert(objc3_async_frame_start(frame, actor ? 0 : executor, actor, &task) ==
         OBJC3_SERVICE_OK);
  assert(!frame->context && !frame->resume && !frame->destroy && !frame->result_type);
  if (manual) {
    uint32_t count = 0;
    assert(objc3_executor_pump(executor, 10000, &count) == OBJC3_SERVICE_OK);
    assert(count != 0);
  }
  assert(objc3_task_wait(task) == OBJC3_SERVICE_OK);
  // Terminal completion publishes the result before the worker destroys its
  // frame. Join a closed group before inspecting stack-owned destruction
  // counters or allowing those counters to leave scope.
  objc3_task_group_t *cleanup = 0;
  assert(objc3_task_group_create(0, &cleanup) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_add_task(cleanup, task) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_close(cleanup) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_join(cleanup) == OBJC3_SERVICE_OK);
  objc3_task_group_release(cleanup);
  return task;
}

static void check_int(objc3_async_frame *frame, objc3_executor_t *executor,
                       objc3_actor_t *actor, int manual, int expected,
                       objc3_service_status expected_status) {
  objc3_task_t *task = start(frame, executor, actor, manual);
  int value = -123;
  assert(objc3_task_copy_result(task, &int_type, &int_ops, &value) == expected_status);
  assert(value == (expected_status == OBJC3_SERVICE_OK ? expected : -123));
  const objc3_async_result_type *type = 0;
  assert(objc3_task_get_result_type(task, &type) == OBJC3_SERVICE_OK);
  assert(objc3_async_result_type_equal(type, &int_type));
  if (expected_status != OBJC3_SERVICE_OK) {
    objc3_completion completion = {0};
    assert(objc3_task_get_completion(task, &completion) == OBJC3_SERVICE_OK);
    assert(completion.kind == (expected_status == OBJC3_SERVICE_CANCELLED
                                  ? OBJC3_COMPLETION_CANCELLED
                                  : OBJC3_COMPLETION_ERROR));
  }
  objc3_task_release(task);
}

static void values(objc3_executor_t *executor, objc3_actor_t *actor, int manual) {
  int destroyed = 0;
  objc3_async_frame abandoned = logical_c_int(42, 0, &destroyed);
  assert(destroyed == 0);
  assert(objc3_async_frame_dispose(&abandoned) == OBJC3_SERVICE_OK);
  assert(!abandoned.context && destroyed == 1);
  logical_c_int_pointer c_pointer = logical_c_int;
  logical_cpp_int_pointer cpp_pointer = logical_cpp_int;
  objc3_async_frame frame = c_pointer(0, 0, &destroyed);
  objc3_task_t *failed = 0;
  assert(objc3_async_frame_start(&frame, 0, 0, &failed) ==
         OBJC3_SERVICE_INVALID_ARGUMENT);
  assert(frame.context && !failed);
  check_int(&frame, executor, actor, manual, 0, OBJC3_SERVICE_OK);
  frame = logical_c_int(42, 0, &destroyed);
  check_int(&frame, executor, actor, manual, 42, OBJC3_SERVICE_OK);
  frame = logical_c_int(42, 2, &destroyed);
  check_int(&frame, executor, actor, manual, 0, OBJC3_SERVICE_CANCELLED);
  frame = logical_c_int(42, 3, &destroyed);
  check_int(&frame, executor, actor, manual, 0, OBJC3_SERVICE_FAILED);
  frame = cpp_pointer(42);
  check_int(&frame, executor, actor, manual, 42, OBJC3_SERVICE_OK);
  frame = logical_cpp_int(-1);
  check_int(&frame, executor, actor, manual, 0, OBJC3_SERVICE_FAILED);

  for (int cpp = 0; cpp != 2; ++cpp) {
    struct bridge_pair original = {40, 2.0};
    frame = cpp ? logical_cpp_pair(original) : logical_c_pair(original, &destroyed);
    objc3_task_t *task = start(&frame, executor, actor, manual);
    struct bridge_pair value = {0};
    assert(objc3_task_copy_result(task, &pair_type, &pair_ops, &value) == OBJC3_SERVICE_OK);
    assert(value.left == 40 && value.right == 2.0);
    objc3_task_release(task);
    logical_void_pointer empty = logical_cpp_void;
    frame = cpp ? empty() : logical_c_void(&destroyed);
    task = start(&frame, executor, actor, manual);
    assert(objc3_task_copy_result(task, &void_type, 0, 0) == OBJC3_SERVICE_OK);
    objc3_task_release(task);
  }
  bridge_complex original = 0;
  __real__ original = 40;
  __imag__ original = 2;
  frame = logical_c_complex(original, &destroyed);
  objc3_task_t *task = start(&frame, executor, actor, manual);
  bridge_complex complex_value = 0;
  assert(objc3_task_copy_result(task, &complex_type, &complex_ops, &complex_value) == OBJC3_SERVICE_OK);
  assert(__real__ complex_value == 40 && __imag__ complex_value == 2);
  objc3_task_release(task);
  frame = logical_c_wide(&destroyed);
  task = start(&frame, executor, actor, manual);
  struct bridge_wide wide = {{0}};
  assert(objc3_task_copy_result(task, &wide_type, &wide_ops, &wide) == OBJC3_SERVICE_OK);
  assert(wide.bytes[0] == 0 && wide.bytes[255] == 42);
  objc3_task_release(task);
  assert(__atomic_load_n(&destroyed, __ATOMIC_ACQUIRE) == 9);
}

static void references(objc3_executor_t *executor, objc3_actor_t *actor, int manual) {
  for (int cpp = 0; cpp != 2; ++cpp) {
    for (int block = 0; block != 2; ++block) {
      for (int nil_value = 0; nil_value != (cpp ? 1 : 2); ++nil_value) {
        int destroyed = 0;
        objc3_async_frame frame;
        if (block)
          frame = cpp ? logical_cpp_block(40)
                      : logical_c_block(40, nil_value, &destroyed);
        else
          frame = cpp ? logical_cpp_object(42)
                      : logical_c_object(42, nil_value, &destroyed);
        objc3_task_t *task = start(&frame, executor, actor, manual);
        void *value = 0;
        const objc3_value_ops *ops = objc3_async_reference_ops(
            block ? OBJC3_ASYNC_BLOCK : OBJC3_ASYNC_OBJECT);
        assert(objc3_task_copy_result(task, block ? &block_type : &object_type,
                                       ops, &value) == OBJC3_SERVICE_OK);
        objc3_task_release(task);
        assert((value == 0) == nil_value);
        if (value) {
          if (block) assert(((__bridge bridge_block)value)(2) == 42);
          else assert(bridge_object_number(value) == 42);
        }
        ops->destroy(&value);
        if (!cpp) assert(__atomic_load_n(&destroyed, __ATOMIC_ACQUIRE) == 1);
      }
    }
  }
}

int bridge_logical_c_calls(objc3_executor_t *executor) {
  for (int actor_destination = 0; actor_destination != 2; ++actor_destination) {
    objc3_actor_t *actor = 0;
    if (actor_destination)
      assert(objc3_actor_create(executor, &actor) == OBJC3_SERVICE_OK);
    values(executor, actor, 1);
    references(executor, actor, 1);
    objc3_actor_release(actor);
  }
  objc3_executor_t *worker = 0;
  assert(objc3_executor_create(0, &worker) == OBJC3_SERVICE_OK);
  values(worker, 0, 0);
  references(worker, 0, 0);
  objc3_executor_release(worker);
  return 1;
}
