#include <assert.h>
#include <objc3c/runtime/objc3_runtime_services.h>
#include <stdlib.h>
#include <string.h>

static const objc3_async_result_type int_type = {
    sizeof(objc3_async_result_type),
    OBJC3_ASYNC_ABI_VERSION,
    sizeof(int),
    _Alignof(int),
    OBJC3_ASYNC_VALUE,
    0,
    __builtin_objc3_type_identity((int *)0),
    sizeof(__builtin_objc3_type_identity((int *)0)) - 1,
    0};

static objc3_service_status copy_int(void *destination, const void *source) {
  memcpy(destination, source, sizeof(int));
  return OBJC3_SERVICE_OK;
}
static void destroy_int(void *value) { (void)value; }
static objc3_service_status move_int(void *destination, void *source) {
  return copy_int(destination, source);
}
static const objc3_value_ops int_ops = {sizeof(objc3_value_ops),
                                        OBJC3_VALUE_OPS_VERSION,
                                        sizeof(int),
                                        _Alignof(int),
                                        copy_int,
                                        move_int,
                                        destroy_int};

struct frame {
  int step, value;
};
static int destroyed;
int c_frame_destroyed(void) { return destroyed; }
static void destroy_frame(void *context) {
  ++destroyed;
  free(context);
}
static void resume_frame(objc3_task_t *task, void *context) {
  struct frame *frame = context;
  if (!frame->step++) {
    assert(objc3_task_schedule_resume(task) == OBJC3_SERVICE_OK);
    return;
  }
  objc3_completion completion = {0};
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = OBJC3_COMPLETION_SUCCESS;
  completion.has_payload = 1;
  completion.payload = &frame->value;
  completion.payload_ops = &int_ops;
  completion.result_type = &int_type;
  assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
}
objc3_async_frame c_frame_factory(int value) {
  struct frame *context = calloc(1, sizeof(*context));
  assert(context);
  context->value = value;
  return (objc3_async_frame){sizeof(objc3_async_frame),
                             OBJC3_ASYNC_ABI_VERSION,
                             context,
                             resume_frame,
                             destroy_frame,
                             &int_type};
}

static int scope_child(int value) __objc3_async_frame { return value + 1; }
int c_frame_scoped(int value) __objc3_async_frame __attribute__((objc_task_group)) {
  objc3_task_group_t *group = objc3_task_group_scope_current();
  assert(group);
  objc3_async_frame frame = scope_child(value);
  objc3_task_t *child = NULL;
  assert(objc3_async_frame_spawn(&frame, &child) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_add(group, child) == OBJC3_SERVICE_OK);
  objc3_task_release(child);
  return value;
}
