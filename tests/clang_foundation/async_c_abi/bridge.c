#include "bridge.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define TYPE_ID(T) __builtin_objc3_type_identity((T *)0)
#define RESULT_TYPE(T, K)                                                      \
  {sizeof(objc3_async_result_type),                                            \
   OBJC3_ASYNC_ABI_VERSION,                                                    \
   sizeof(T),                                                                  \
   __alignof__(T),                                                             \
   K,                                                                          \
   0,                                                                          \
   TYPE_ID(T),                                                                 \
   sizeof(TYPE_ID(T)) - 1,                                                     \
   0}

static const objc3_async_result_type int_type =
    RESULT_TYPE(int, OBJC3_ASYNC_VALUE);
static const objc3_async_result_type unsigned_type =
    RESULT_TYPE(unsigned, OBJC3_ASYNC_VALUE);
static const objc3_async_result_type pair_type =
    RESULT_TYPE(struct bridge_pair, OBJC3_ASYNC_VALUE);
static const objc3_async_result_type wide_type =
    RESULT_TYPE(struct bridge_wide, OBJC3_ASYNC_VALUE);
static const objc3_async_result_type complex_type =
    RESULT_TYPE(bridge_complex, OBJC3_ASYNC_VALUE);
static const objc3_async_result_type void_type = {
    sizeof(objc3_async_result_type),
    OBJC3_ASYNC_ABI_VERSION,
    0,
    1,
    OBJC3_ASYNC_VOID,
    0,
    TYPE_ID(void),
    sizeof(TYPE_ID(void)) - 1,
    0};

static int copies;
static int reject_copy;
static objc3_task_t *copy_observer;
#ifdef __OBJC__
static void consume_cpp_references(objc3_executor_t *executor);
#endif
#define VALUE_OPS(NAME, T)                                                     \
  static objc3_service_status NAME##_copy(void *d, const void *s) {            \
    if (reject_copy)                                                           \
      return OBJC3_SERVICE_OUT_OF_MEMORY;                                      \
    if (copy_observer) {                                                       \
      objc3_completion view = {0};                                             \
      assert(objc3_task_get_completion(copy_observer, &view) ==                \
             OBJC3_SERVICE_OK);                                                \
    }                                                                          \
    __atomic_fetch_add(&copies, 1, __ATOMIC_RELAXED);                          \
    memcpy(d, s, sizeof(T));                                                   \
    return OBJC3_SERVICE_OK;                                                   \
  }                                                                            \
  static objc3_service_status NAME##_move(void *d, void *s) {                  \
    return NAME##_copy(d, s);                                                  \
  }                                                                            \
  static void NAME##_destroy(void *p) { (void)p; }                             \
  static const objc3_value_ops NAME##_ops = {sizeof(objc3_value_ops),          \
                                             OBJC3_VALUE_OPS_VERSION,          \
                                             sizeof(T),                        \
                                             __alignof__(T),                   \
                                             NAME##_copy,                      \
                                             NAME##_move,                      \
                                             NAME##_destroy}
VALUE_OPS(int, int);
VALUE_OPS(pair, struct bridge_pair);
VALUE_OPS(wide, struct bridge_wide);
VALUE_OPS(complex, bridge_complex);

struct frame {
  int step, mode, value;
  int *destroyed;
  const objc3_async_result_type *type;
  const objc3_value_ops *ops;
  struct bridge_pair pair;
  bridge_complex complex_value;
  objc3_async_frame child;
  objc3_task_t *dependency;
  void *reference;
};

#ifdef __OBJC__
extern void *bridge_retain(void *) __asm__("objc_retain");
extern void bridge_release(void *) __asm__("objc_release");
extern void *bridge_block_copy(const void *) __asm__("_Block_copy");
extern void bridge_block_release(const void *) __asm__("_Block_release");
#endif

static void destroy_frame(void *context) {
  struct frame *frame = context;
  if (frame->dependency) {
    objc3_task_request_cancel(frame->dependency);
    objc3_task_release(frame->dependency);
  }
  assert(objc3_async_frame_dispose(&frame->child) == OBJC3_SERVICE_OK);
#ifdef __OBJC__
  if (frame->type->kind == OBJC3_ASYNC_OBJECT)
    bridge_release(frame->reference);
  if (frame->type->kind == OBJC3_ASYNC_BLOCK)
    bridge_block_release(frame->reference);
#endif
  if (frame->destroyed)
    __atomic_fetch_add(frame->destroyed, 1, __ATOMIC_RELEASE);
  free(frame);
}

static void resume_frame(objc3_task_t *task, void *context) {
  struct frame *frame = context;
  objc3_completion completion = {0};
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  if (objc3_task_is_cancel_requested(task) || frame->mode == 2) {
    completion.kind = OBJC3_COMPLETION_CANCELLED;
    assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
    return;
  }
  if (frame->step++ == 0) {
    if (frame->child.context) {
      assert(objc3_async_frame_start_child(
                 &frame->child, task, &frame->dependency) == OBJC3_SERVICE_OK);
      assert(objc3_task_await(task, frame->dependency) == OBJC3_SERVICE_OK);
    } else {
      assert(objc3_task_schedule_resume(task) == OBJC3_SERVICE_OK);
    }
    return;
  }
  if (frame->dependency) {
    assert(objc3_task_copy_result(frame->dependency, &int_type, &int_ops,
                                  &frame->value) == OBJC3_SERVICE_OK);
    frame->value += 1;
  }
  if (frame->mode == 3) {
    completion.kind = OBJC3_COMPLETION_ERROR;
    assert(objc3_error_create(&frame->value, &int_ops, &completion.error) ==
           OBJC3_SERVICE_OK);
    assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
    objc3_error_release(completion.error);
    return;
  }
  completion.kind = OBJC3_COMPLETION_SUCCESS;
  completion.result_type = frame->type;
  completion.has_payload = frame->type->kind != OBJC3_ASYNC_VOID;
  completion.payload_ops = frame->ops;
  if (frame->type == &int_type)
    completion.payload = &frame->value;
  else if (frame->type == &pair_type)
    completion.payload = &frame->pair;
  else if (frame->type == &complex_type)
    completion.payload = &frame->complex_value;
  else if (frame->type->kind == OBJC3_ASYNC_OBJECT ||
           frame->type->kind == OBJC3_ASYNC_BLOCK)
    completion.payload = &frame->reference;
  struct bridge_wide wide = {{0}};
  if (frame->type == &wide_type) {
    wide.bytes[255] = 42;
    completion.payload = &wide;
  }
  if (frame->mode == 4) {
    completion.result_type = &unsigned_type;
    int before = __atomic_load_n(&copies, __ATOMIC_RELAXED);
    assert(objc3_task_complete(task, &completion) ==
           OBJC3_SERVICE_TYPE_MISMATCH);
    assert(__atomic_load_n(&copies, __ATOMIC_RELAXED) == before);
    completion.result_type = frame->type;
  }
  assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
}

static objc3_async_frame make_frame(const objc3_async_result_type *type,
                                    const objc3_value_ops *ops,
                                    int *destroyed) {
  struct frame *frame = calloc(1, sizeof(*frame));
  assert(frame);
  frame->type = type;
  frame->ops = ops;
  frame->destroyed = destroyed;
  objc3_async_frame result = {sizeof(result), OBJC3_ASYNC_ABI_VERSION, frame,
                              resume_frame,   destroy_frame,           type};
  return result;
}

objc3_async_frame bridge_c_int(int value, int mode, int *destroyed) {
  objc3_async_frame result = make_frame(&int_type, &int_ops, destroyed);
  ((struct frame *)result.context)->value = value;
  ((struct frame *)result.context)->mode = mode;
  return result;
}
objc3_async_frame bridge_c_pair(struct bridge_pair pair, int *destroyed) {
  objc3_async_frame result = make_frame(&pair_type, &pair_ops, destroyed);
  ((struct frame *)result.context)->pair = pair;
  return result;
}
objc3_async_frame bridge_c_wide(int *destroyed) {
  return make_frame(&wide_type, &wide_ops, destroyed);
}
objc3_async_frame bridge_c_complex(bridge_complex value, int *destroyed) {
  objc3_async_frame result = make_frame(&complex_type, &complex_ops, destroyed);
  ((struct frame *)result.context)->complex_value = value;
  return result;
}
objc3_async_frame bridge_c_void(int *destroyed) {
  return make_frame(&void_type, 0, destroyed);
}
objc3_async_frame bridge_c_chain(objc3_async_frame *child, int *destroyed) {
  objc3_async_frame result = bridge_c_int(0, 0, destroyed);
  ((struct frame *)result.context)->child = *child;
  memset(child, 0, sizeof(*child));
  return result;
}
int bridge_c_copy_count(void) {
  return __atomic_load_n(&copies, __ATOMIC_RELAXED);
}

static void pump(objc3_executor_t *executor) {
  uint32_t count = 0;
  assert(objc3_executor_pump(executor, 1000, &count) == OBJC3_SERVICE_OK);
}

int bridge_c_consume_cpp(objc3_executor_t *executor) {
  objc3_async_frame frame = bridge_cpp_int(42);
  objc3_task_t *task = 0;
  objc3_task_start_options stale = {0};
  stale.struct_size = sizeof(stale);
  stale.version = 3;
  stale.executor = executor;
  stale.frame = frame.context;
  stale.resume = frame.resume;
  stale.destroy = frame.destroy;
  stale.result_type = frame.result_type;
  assert(objc3_task_start(&stale, &task) == OBJC3_SERVICE_UNSUPPORTED_VERSION);
  assert(!task && frame.context == stale.frame);
  objc3_async_result_type local_type = *frame.result_type;
  char local_identity[OBJC3_ASYNC_IDENTITY_LIMIT + 1];
  memcpy(local_identity, local_type.identity, local_type.identity_size + 1);
  local_type.identity = local_identity;
  frame.result_type = &local_type;
  assert(objc3_async_frame_start(&frame, executor, 0, &task) ==
         OBJC3_SERVICE_OK);
  assert(!frame.context);
  memset(local_identity, 'x', local_type.identity_size);
  local_type.identity = 0;
  const objc3_async_result_type *type = 0;
  assert(objc3_task_get_result_type(task, &type) == OBJC3_SERVICE_OK);
  assert(objc3_async_result_type_equal(type, &int_type));
  int value = -1;
  assert(objc3_task_copy_result(task, &int_type, &int_ops, &value) ==
         OBJC3_SERVICE_NOT_READY);
  assert(value == -1);
  pump(executor);
  int before = bridge_c_copy_count();
  assert(objc3_task_copy_result(task, &unsigned_type, &int_ops, &value) ==
         OBJC3_SERVICE_TYPE_MISMATCH);
  assert(value == -1 && bridge_c_copy_count() == before);
  reject_copy = 1;
  assert(objc3_task_copy_result(task, &int_type, &int_ops, &value) ==
         OBJC3_SERVICE_OUT_OF_MEMORY);
  assert(value == -1 && bridge_c_copy_count() == before);
  reject_copy = 0;
  copy_observer = task;
  assert(objc3_task_copy_result(task, &int_type, &int_ops, &value) ==
         OBJC3_SERVICE_OK);
  copy_observer = 0;
  assert(value == 42);
  objc3_task_release(task);
  struct bridge_pair pair = {40, 2.0};
  frame = bridge_cpp_pair(pair);
  assert(objc3_async_frame_start(&frame, executor, 0, &task) ==
         OBJC3_SERVICE_OK);
  pump(executor);
  struct bridge_pair copied = {0, 0};
  assert(objc3_task_copy_result(task, &pair_type, &pair_ops, &copied) ==
         OBJC3_SERVICE_OK);
  assert(copied.left == 40 && copied.right == 2.0);
  objc3_task_release(task);
  frame = bridge_cpp_void();
  assert(objc3_async_frame_start(&frame, executor, 0, &task) ==
         OBJC3_SERVICE_OK);
  pump(executor);
  assert(objc3_task_copy_result(task, &void_type, 0, 0) == OBJC3_SERVICE_OK);
  objc3_task_release(task);
  frame = bridge_cpp_int(-1);
  assert(objc3_async_frame_start(&frame, executor, 0, &task) ==
         OBJC3_SERVICE_OK);
  pump(executor);
  value = -123;
  assert(objc3_task_copy_result(task, &int_type, &int_ops, &value) ==
         OBJC3_SERVICE_FAILED);
  assert(value == -123);
  objc3_completion failure = {0};
  assert(objc3_task_get_completion(task, &failure) == OBJC3_SERVICE_OK);
  assert(failure.kind == OBJC3_COMPLETION_ERROR && failure.error &&
         !failure.result_type);
  assert(objc3_task_get_result_type(task, &type) == OBJC3_SERVICE_OK);
  assert(objc3_async_result_type_equal(type, &int_type));
  objc3_task_release(task);
#ifdef __OBJC__
  consume_cpp_references(executor);
#endif
  return 1;
}

#ifdef __OBJC__
extern id class_createInstance(Class, __SIZE_TYPE__)
    __attribute__((ns_returns_retained));
extern id object_dispose(__unsafe_unretained id);
static int objects_destroyed;
__attribute__((objc_root_class))
@interface BridgeResult {
  Class isa;
@public
  int number;
}
+ (id)new;
- (int)number;
@end
@implementation BridgeResult
+ (id)new {
  return class_createInstance(self, 0);
}
- (int)number {
  return number;
}
- (void)dealloc {
  __atomic_fetch_add(&objects_destroyed, 1, __ATOMIC_RELEASE);
  object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {
}
#endif
@end

typedef __unsafe_unretained id bridge_object_abi;
typedef __unsafe_unretained bridge_block bridge_block_abi;
static const objc3_async_result_type object_type =
    RESULT_TYPE(bridge_object_abi, OBJC3_ASYNC_OBJECT);
static const objc3_async_result_type block_type =
    RESULT_TYPE(bridge_block_abi, OBJC3_ASYNC_BLOCK);

static objc3_async_frame make_reference(int value, int nil_value,
                                        int *destroyed,
                                        const objc3_async_result_type *type) {
  objc3_async_frame result =
      make_frame(type, objc3_async_reference_ops(type->kind), destroyed);
  if (!nil_value) {
    BridgeResult *object = [BridgeResult new];
    object->number = value;
    if (type->kind == OBJC3_ASYNC_OBJECT) {
      ((struct frame *)result.context)->reference =
          bridge_retain((__bridge void *)object);
    } else {
      bridge_block block = ^(int delta) {
        return [object number] + delta;
      };
      ((struct frame *)result.context)->reference =
          bridge_block_copy((__bridge void *)block);
    }
#if !__has_feature(objc_arc)
    bridge_release((__bridge void *)object);
#endif
  }
  return result;
}
objc3_async_frame bridge_c_object(int value, int nil_value, int *destroyed) {
  return make_reference(value, nil_value, destroyed, &object_type);
}
objc3_async_frame bridge_c_block(int value, int nil_value, int *destroyed) {
  return make_reference(value, nil_value, destroyed, &block_type);
}
int bridge_object_number(void *value) { return [(__bridge id)value number]; }
int bridge_object_destructions(void) {
  return __atomic_load_n(&objects_destroyed, __ATOMIC_ACQUIRE);
}

static void consume_cpp_references(objc3_executor_t *executor) {
  objc3_async_frame frame = bridge_cpp_object(42);
  objc3_task_t *task = 0;
  assert(objc3_async_frame_start(&frame, executor, 0, &task) ==
         OBJC3_SERVICE_OK);
  pump(executor);
  void *object = 0;
  const objc3_value_ops *ops = objc3_async_reference_ops(OBJC3_ASYNC_OBJECT);
  assert(objc3_task_copy_result(task, &object_type, ops, &object) ==
         OBJC3_SERVICE_OK);
  objc3_task_release(task);
  assert(bridge_object_number(object) == 42);
  ops->destroy(&object);
  frame = bridge_cpp_block(40);
  assert(objc3_async_frame_start(&frame, executor, 0, &task) ==
         OBJC3_SERVICE_OK);
  pump(executor);
  void *block = 0;
  ops = objc3_async_reference_ops(OBJC3_ASYNC_BLOCK);
  assert(objc3_task_copy_result(task, &block_type, ops, &block) ==
         OBJC3_SERVICE_OK);
  objc3_task_release(task);
  assert(((__bridge bridge_block)block)(2) == 42);
  ops->destroy(&block);
}
#endif
