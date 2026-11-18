#include "api.h"
#include "objc3_async_c_export.h"
#ifdef OBJC3_COMPLETION_PRIVATE_TASK
#include "objc3_task.hpp"
#endif

@protocol Error
@end

#ifdef __cplusplus
extern "C" {
#endif
extern id class_createInstance(Class, __SIZE_TYPE__) __attribute__((ns_returns_retained));
extern id object_dispose(__unsafe_unretained id);
extern Class object_getClass(id);
extern unsigned char class_conformsToProtocol(Class, Protocol *);
#ifdef __cplusplus
}
#endif

static int live_objects;
static int cancel_entries;

__attribute__((objc_root_class))
@interface CompletionProbe <Error> {
  Class isa;
}
+ (id)new;
- (void)dealloc;
@end

@implementation CompletionProbe
+ (id)new {
  id object = class_createInstance(self, 0);
  if (object) __atomic_add_fetch(&live_objects, 1, __ATOMIC_RELAXED);
  return object;
}
- (void)dealloc {
  __atomic_sub_fetch(&live_objects, 1, __ATOMIC_RELAXED);
  object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
@end

CompletionObject completion_make_object(void) { return [CompletionProbe new]; }
int completion_object_count(void) { return __atomic_load_n(&live_objects, __ATOMIC_RELAXED); }
int completion_cancel_entries(void) { return cancel_entries; }
int completion_error_conforms(id error) {
  Class type = objc3_async_c_error_is_cancellation((id<Error>)error)
      ? (Class)error : object_getClass(error);
  return class_conformsToProtocol(type, @protocol(Error));
}

static int next(int value) async { return value + 1; }
static CompletionRecord value_result(long first, int second) async {
  int adjusted = await next(second);
  CompletionRecord result = {{first, adjusted, 73, 91}};
  return result;
}
static void void_result(void) async { await next(1); }
static id object_result(id object) async { await next(1); return object; }
static CompletionBlock block_result(int value) async {
  int adjusted = await next(value);
  return ^(int extra) { return adjusted + extra; };
}
static int error_result(id<Error> error) async throws(id<Error>) {
  await next(1);
  __objc3_throw(error);
}
static int cancel_result(objc3_actor_t *actor) async { ++cancel_entries; return actor != 0; }

int completion_checked_error(const objc3_completion *result,
                             objc3_async_c_error expected) {
  return result->kind == OBJC3_COMPLETION_ERROR && !result->has_payload &&
         __objc3_async_c_completion_error(result) == expected;
}
int completion_checked_cancellation(const objc3_completion *result,
                                    objc3_async_c_error expected) {
  return objc3_async_c_error_is_cancellation(expected) &&
         completion_checked_error(result, expected);
}
objc3_async_frame completion_member_failure(CompletionObject error) {
  return error_result((id<Error>)error);
}

@protocol NarrowCompletionError <Error>
- (int)completionErrorCode;
@end

static void cancel_child_resume(objc3_task_t *task, void *context) {
  (void)context;
  objc3_completion result = {0};
  result.struct_size = sizeof(result);
  result.version = OBJC3_SERVICE_ABI_VERSION;
  result.kind = OBJC3_COMPLETION_CANCELLED;
  if (objc3_task_complete(task, &result) != OBJC3_SERVICE_OK) __builtin_trap();
}
static void cancel_child_destroy(void *context) {
  ++((CompletionCancellation *)context)->child_destroyed;
}
#ifdef __cplusplus
extern "C" {
#endif
int cancel_child(CompletionCancellation *state) async throws(id<Error>)
    __asm__("completion_cancel_child");
int cancel_narrow_child(CompletionCancellation *state)
    async throws(id<NarrowCompletionError>) __asm__("completion_cancel_child");
int join_cancelled_child(objc3_task_t *child) async throws(id<Error>)
    __asm__("__objc3_task_join_frame");
objc3_async_frame completion_cancel_child(CompletionCancellation *state) {
  static const char identity[] = __builtin_objc3_type_identity((int *)0);
  static const objc3_async_result_type type = {sizeof(type), OBJC3_ASYNC_ABI_VERSION,
      sizeof(int), __alignof__(int), OBJC3_ASYNC_VALUE, 0,
      identity, sizeof(identity) - 1, 0};
  objc3_async_frame frame = {sizeof(frame), OBJC3_ASYNC_ABI_VERSION,
      state, cancel_child_resume, cancel_child_destroy, &type};
  return frame;
}
#ifdef __cplusplus
}
#endif
static int optional_parent(CompletionCancellation *state) async {
  ++state->entered;
  __objc3_defer { ++state->finished; }
  if (state->cancel_before_await) objc3_task_request_cancel(state->cancel_before_await);
  __objc3_optional(int) result = try? await cancel_child(state);
  ++state->resumed;
  return __objc3_has_value(result) ? -1 : 42;
}
static int await_parent(CompletionCancellation *state) async throws(id<Error>) {
  ++state->entered;
  __objc3_defer { ++state->finished; }
  if (state->cancel_before_await) objc3_task_request_cancel(state->cancel_before_await);
  int result = try await cancel_child(state);
  ++state->resumed;
  return result;
}
static int narrow_parent(CompletionCancellation *state)
    async throws(id<NarrowCompletionError>) {
  ++state->entered;
  __objc3_defer { ++state->finished; }
  if (state->cancel_before_await) objc3_task_request_cancel(state->cancel_before_await);
  int result = try await cancel_narrow_child(state);
  ++state->resumed;
  return result;
}
static int join_parent(objc3_task_t *child) async throws(id<Error>) {
  return try await join_cancelled_child(child);
}
objc3_async_frame completion_join_frame(objc3_task_t *child) {
  return join_parent(child);
}
static void group_join_parent(objc3_task_group_t *group) async throws(id<Error>) {
  try await objc3_task_group_join_checked(group);
}
objc3_async_frame completion_group_join_frame(objc3_task_group_t *group) {
  return group_join_parent(group);
}
static int checkpoint_parent(CompletionCancellation *state) async {
  ++state->entered;
  __objc3_defer { ++state->finished; }
  if (objc3_task_current_cancel_requested() ||
      objc3_task_current_cancellation_error()) return -1;
  if (state->cancel_before_await) objc3_task_request_cancel(state->cancel_before_await);
  __objc3_do {
    try objc3_task_check_cancellation();
    return state->cancel_before_await ? -2 : 42;
  } __objc3_catch(id<Error> error) {
    if (!state->cancel_before_await || !objc3_task_current_cancel_requested() ||
        !objc3_async_c_error_is_cancellation(error)) return -3;
    return 42;
  }
}
objc3_async_frame completion_checkpoint_frame(CompletionCancellation *state) {
  return checkpoint_parent(state);
}
objc3_async_frame completion_narrow_frame(CompletionCancellation *state) {
  return narrow_parent(state);
}
objc3_async_frame completion_optional_frame(CompletionCancellation *state) {
  return optional_parent(state);
}
objc3_async_frame completion_await_frame(CompletionCancellation *state) {
  return await_parent(state);
}
#ifdef OBJC3_COMPLETION_PRIVATE_TASK
static int optional_task(CompletionCancellation *state) __objc3_async {
  ++state->entered;
  __objc3_defer { ++state->finished; }
  if (state->cancel_before_await) objc3_task_request_cancel(state->cancel_before_await);
  __objc3_optional(int) result = try? await cancel_child(state);
  ++state->resumed;
  return __objc3_has_value(result) ? -1 : 42;
}
static int await_task(CompletionCancellation *state) __objc3_async throws(id<Error>) {
  ++state->entered;
  __objc3_defer { ++state->finished; }
  if (state->cancel_before_await) objc3_task_request_cancel(state->cancel_before_await);
  int result = try await cancel_child(state);
  ++state->resumed;
  return result;
}
static int narrow_task(CompletionCancellation *state)
    __objc3_async throws(id<NarrowCompletionError>) {
  ++state->entered;
  __objc3_defer { ++state->finished; }
  if (state->cancel_before_await) objc3_task_request_cancel(state->cancel_before_await);
  int result = try await cancel_narrow_child(state);
  ++state->resumed;
  return result;
}
objc3_async_frame completion_narrow_task(CompletionCancellation *state) {
  return __objc3_async_to_frame<int>(narrow_task(state));
}
objc3_async_frame completion_optional_task(CompletionCancellation *state) {
  return __objc3_async_to_frame<int>(optional_task(state));
}
objc3_async_frame completion_await_task(CompletionCancellation *state) {
  return __objc3_async_to_frame<int>(await_task(state));
}
#endif

static objc3_executor_t *main_executor(void) {
  // The shared process-wide destination outlives every submitted callback.
  objc3_executor_t *executor = 0;
  if (objc3_executor_get_main(&executor) != OBJC3_SERVICE_OK) __builtin_trap();
  objc3_executor_release(executor);
  return executor;
}

OBJC3_ASYNC_C_DEFINE_VALUE(completion_value, CompletionRecord,
    value_result(first, second), 0, main_executor(), 0, long first, int second)
OBJC3_ASYNC_C_DEFINE_VOID(completion_void, void_result(), 0, main_executor(), 0)
OBJC3_ASYNC_C_DEFINE_OBJECT(completion_object, CompletionObject,
    object_result(object), 0, main_executor(), 0, CompletionObject _Nullable object)
OBJC3_ASYNC_C_DEFINE_BLOCK(completion_block, CompletionBlock,
    block_result(value), 0, main_executor(), 0, int value)
OBJC3_ASYNC_C_DEFINE_VALUE(completion_error, int,
    error_result((id<Error>)error), 1, main_executor(), 0, CompletionObject _Nonnull error)
OBJC3_ASYNC_C_DEFINE_VALUE(completion_checked_cancel, int,
    await_parent(state), 1, main_executor(), 0, CompletionCancellation * _Nonnull state)
OBJC3_ASYNC_C_DEFINE_VALUE(completion_cancel, int,
    cancel_result(actor), 0, 0, actor, objc3_actor_t * _Nonnull actor)
OBJC3_ASYNC_C_DEFINE_VALUE(completion_worker, int, next(value), 0, 0, 0, int value)
