#include "provider.h"
#include <assert.h>
#include <string.h>
#define TYPE_ID(T) __builtin_objc3_type_identity((T *)0)
#define RESULT(T, K) {sizeof(objc3_async_result_type), OBJC3_ASYNC_ABI_VERSION, \
  sizeof(T), __alignof__(T), K, 0, TYPE_ID(T), sizeof(TYPE_ID(T))-1, 0}
static objc3_service_status int_copy(void *d, const void *s) {
  memcpy(d, s, sizeof(int)); return OBJC3_SERVICE_OK;
}
static objc3_service_status int_move(void *d, void *s) { return int_copy(d, s); }
static void int_destroy(void *p) { (void)p; }
static const objc3_value_ops int_ops = {sizeof(objc3_value_ops), OBJC3_VALUE_OPS_VERSION,
  sizeof(int), __alignof__(int), int_copy, int_move, int_destroy};
static const objc3_async_result_type int_type = RESULT(int, OBJC3_ASYNC_VALUE);

static objc3_task_t *start(objc3_async_frame *frame, objc3_executor_t *executor,
                           objc3_actor_t *actor) {
  objc3_task_t *task = 0;
  assert(frame->context && frame->resume && frame->destroy && frame->result_type);
  assert(objc3_async_frame_start(frame, actor ? 0 : executor, actor, &task) == OBJC3_SERVICE_OK);
  assert(!frame->context && !frame->resume && !frame->destroy && !frame->result_type);
  return task;
}
static void finish(objc3_task_t *task, objc3_executor_t *executor) {
  uint32_t count = 0;
  assert(objc3_executor_pump(executor, 10000, &count) == OBJC3_SERVICE_OK);
  assert(objc3_task_wait(task) == OBJC3_SERVICE_OK);
  objc3_task_group_t *join = 0;
  assert(objc3_task_group_create(0, &join) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_add(join, task) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_close(join) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_join(join) == OBJC3_SERVICE_OK);
  objc3_task_group_release(join);
}
extern id class_createInstance(Class, __SIZE_TYPE__) __attribute__((ns_returns_retained));
extern id object_dispose(__unsafe_unretained id);
extern void *probe_retain(void *) __asm__("objc_retain");
extern void probe_release(void *) __asm__("objc_release");
static int destroyed_objects;
__attribute__((objc_root_class))
@interface NativeCProbe <NativeCError> { Class isa; }
+ (id)new;
- (int)number;
@end
@implementation NativeCProbe
+ (id)new { return class_createInstance(self, 0); }
- (int)number { return 42; }
- (void)dealloc {
  __atomic_fetch_add(&destroyed_objects, 1, __ATOMIC_RELEASE);
  object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
@end
static void *new_probe(void) {
#if __has_feature(objc_arc)
  return (__bridge_retained void *)[NativeCProbe new];
#else
  return (void *)[NativeCProbe new];
#endif
}
static void references(objc3_executor_t *executor) {
  int before = __atomic_load_n(&destroyed_objects, __ATOMIC_ACQUIRE);
  int cleanups = 0;
  @autoreleasepool {
    void *source = new_probe();
    objc3_async_frame frame = native_c_object((__bridge id)source, &cleanups);
#if __has_feature(objc_arc)
    // The lazy frame already owns its ARC parameter. MRC parameters remain
    // borrowed and their source owner is kept until execution finishes.
    probe_release(source);
#endif
    objc3_task_t *task = start(&frame, executor, 0);
    finish(task, executor);
#if !__has_feature(objc_arc)
    probe_release(source);
#endif
    typedef __unsafe_unretained id ObjectABI;
    const objc3_async_result_type object_type = RESULT(ObjectABI, OBJC3_ASYNC_OBJECT);
    const objc3_value_ops *ops = objc3_async_reference_ops(OBJC3_ASYNC_OBJECT);
    void *owned = 0;
    assert(objc3_task_copy_result(task, &object_type, ops, &owned) == OBJC3_SERVICE_OK);
    objc3_task_release(task);
    assert([(__bridge NativeCProbe *)owned number] == 42);
    assert(__atomic_load_n(&destroyed_objects, __ATOMIC_ACQUIRE) == before);
    ops->destroy(&owned);
    assert(cleanups == 1);

    frame = native_c_block_value(38, &cleanups);
    task = start(&frame, executor, 0);
    finish(task, executor);
    typedef __unsafe_unretained native_c_block BlockABI;
    const objc3_async_result_type block_type = RESULT(BlockABI, OBJC3_ASYNC_BLOCK);
    ops = objc3_async_reference_ops(OBJC3_ASYNC_BLOCK);
    owned = 0;
    assert(objc3_task_copy_result(task, &block_type, ops, &owned) == OBJC3_SERVICE_OK);
    objc3_task_release(task);
    assert(((__bridge native_c_block)owned)(2) == 42);
    ops->destroy(&owned);

    source = new_probe();
    frame = native_c_recover((__bridge id<NativeCError>)source, &cleanups);
    task = start(&frame, executor, 0);
    finish(task, executor);
    int value = 0;
    assert(objc3_task_copy_result(task, &int_type, &int_ops, &value) == OBJC3_SERVICE_OK);
    assert(value == 42);
    objc3_task_release(task);
    frame = native_c_propagate((__bridge id<NativeCError>)source);
    task = start(&frame, executor, 0);
    finish(task, executor);
    objc3_completion completion = {0};
    assert(objc3_task_get_completion(task, &completion) == OBJC3_SERVICE_OK);
    assert(completion.kind == OBJC3_COMPLETION_ERROR && completion.error);
    objc3_task_release(task);
    frame = native_c_foreign((__bridge id)source, &cleanups);
    task = start(&frame, executor, 0);
    finish(task, executor);
    assert(objc3_task_get_completion(task, &completion) == OBJC3_SERVICE_OK);
    assert(completion.kind == OBJC3_COMPLETION_CANCELLED && !completion.error);
    objc3_task_release(task);
    probe_release(source);
  }
  assert(__atomic_load_n(&destroyed_objects, __ATOMIC_ACQUIRE) == before + 2);
}

int main(void) {
  objc3_executor_t *executor = 0;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  for (int actor_case = 0; actor_case != 2; ++actor_case) {
    objc3_actor_t *actor = 0;
    if (actor_case) assert(objc3_actor_create(executor, &actor) == OBJC3_SERVICE_OK);
    int cleanups = 0;
    objc3_async_frame abandoned = native_c_chain(8, &cleanups);
    assert(objc3_async_frame_dispose(&abandoned) == OBJC3_SERVICE_OK);
    assert(!abandoned.context && cleanups == 0); // body is still lazy
    objc3_async_frame frame = native_c_chain(8, &cleanups);
    objc3_task_t *invalid = 0;
    assert(objc3_async_frame_start(&frame, 0, 0, &invalid) == OBJC3_SERVICE_INVALID_ARGUMENT);
    assert(frame.context && !invalid); // failed start preserves ownership
    objc3_task_t *task = start(&frame, executor, actor);
    finish(task, executor);
    int result = -1;
    assert(objc3_task_copy_result(task, &int_type, &int_ops, &result) == OBJC3_SERVICE_OK);
    assert(result == 28 && __atomic_load_n(&cleanups, __ATOMIC_ACQUIRE) == 9);
    objc3_task_release(task);

    cleanups = 0;
    frame = native_c_chain(8, &cleanups);
    task = start(&frame, executor, actor);
    uint32_t pumped = 0;
    assert(objc3_executor_pump(executor, 1, &pumped) == OBJC3_SERVICE_OK && pumped == 1);
    objc3_task_request_cancel(task); // parent suspended on its private child
    finish(task, executor);
    result = -1;
    assert(objc3_task_copy_result(task, &int_type, &int_ops, &result) == OBJC3_SERVICE_CANCELLED);
    assert(result == -1 && __atomic_load_n(&cleanups, __ATOMIC_ACQUIRE) == 1);
    objc3_task_release(task);
    objc3_actor_release(actor);
  }
  references(executor);
  objc3_async_frame abandoned_oom = native_c_force_oom();
  assert(objc3_async_frame_validate(&abandoned_oom) == OBJC3_SERVICE_OK);
  assert(objc3_async_frame_dispose(&abandoned_oom) == OBJC3_SERVICE_OK);
  for (int nested = 0; nested != 2; ++nested) {
    objc3_async_frame failed = nested ? native_c_oom_parent() : native_c_force_oom();
    assert(objc3_async_frame_validate(&failed) == OBJC3_SERVICE_OK);
    objc3_task_t *task = start(&failed, executor, 0);
    finish(task, executor);
    objc3_completion completion = {0};
    assert(objc3_task_get_completion(task, &completion) == OBJC3_SERVICE_OK);
    assert(completion.kind == OBJC3_COMPLETION_ERROR && completion.error);
    objc3_service_status original = OBJC3_SERVICE_OK;
    assert(objc3_async_error_get_service(completion.error, &original) == OBJC3_SERVICE_OK);
    assert(original == OBJC3_SERVICE_OUT_OF_MEMORY);
    objc3_task_release(task);
  }
  // Cancellation before the first resume remains cancellation, including for
  // an allocation-failure carrier; the source body must never execute.
  objc3_async_frame cancelled_oom = native_c_force_oom();
  objc3_task_t *cancelled = start(&cancelled_oom, executor, 0);
  objc3_task_request_cancel(cancelled);
  finish(cancelled, executor);
  int untouched = 13;
  assert(objc3_task_copy_result(cancelled, &int_type, &int_ops, &untouched) == OBJC3_SERVICE_CANCELLED);
  assert(untouched == 13);
  objc3_task_release(cancelled);
  objc3_executor_release(executor);
  return 0;
}
