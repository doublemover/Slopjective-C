#ifndef OBJC3_DYNAMIC_FRAME_STORAGE_H
#define OBJC3_DYNAMIC_FRAME_STORAGE_H
#include <stdint.h>
#include <alloca.h>

#ifdef __cplusplus
extern "C" void storage_fail_next_allocation(void);
extern "C" void storage_observe_allocation(void *);
#else
extern void storage_fail_next_allocation(void);
extern void storage_observe_allocation(void *);
#endif

typedef struct FrameStorageState {
  unsigned entered, exited, arrays, after_await;
  objc3_task_t *child;
} FrameStorageState;

static int storage_leaf(void) __objc3_async_frame { return 42; }

static int dynamic_frame_storage(FrameStorageState *state, int count)
    __objc3_async_frame {
  ++state->entered;
  defer { ++state->exited; }
  unsigned char *saved[5];
  for (int i = 0; i != 5; ++i) {
    // Builtin alloca has invocation lifetime, including after this loop body.
    unsigned char *bytes;
    if (i == 0) bytes = (unsigned char *)alloca(count);
    else if (i == 1) bytes = (unsigned char *)__builtin_alloca(count);
    else if (i == 2) bytes = (unsigned char *)__builtin_alloca_uninitialized(count);
    else if (i == 3) bytes = (unsigned char *)__builtin_alloca_with_align(count, 1024);
    else bytes = (unsigned char *)__builtin_alloca_with_align_uninitialized(count, 1024);
    if (i >= 3) assert((uintptr_t)bytes % 128 == 0);
    for (int j = 0; j != count; ++j) bytes[j] = (unsigned char)(i + j);
    saved[i] = bytes;
    int dimension = count + i;
    int values[dimension][2] __attribute__((aligned(128)));
    int (*view)[2] = values; // A pointer-to-VLA does not allocate another owner.
    assert((uintptr_t)values % 128 == 0);
    for (int j = 0; j != dimension; ++j) {
      view[j][0] = j;
      view[j][1] = j + 42;
    }
    defer {
      assert(values[dimension - 1][1] == dimension + 41);
      ++state->arrays;
    }
    int value = __objc3_await storage_leaf();
    assert(value == 42 && sizeof(values) == (unsigned long)dimension * 2 * sizeof(int));
    for (int j = 0; j != dimension; ++j)
      assert(view[j][0] == j && view[j][1] == j + 42);
    ++state->after_await;
  }
  for (int i = 0; i != 5; ++i)
    for (int j = 0; j != count; ++j) assert(saved[i][j] == (unsigned char)(i + j));
  return 42;
}

static int dynamic_storage_overflow(FrameStorageState *state, int mode,
                                     __SIZE_TYPE__ huge)
    __objc3_async_frame {
  ++state->entered;
  defer { ++state->exited; }
  // Existing lexical and invocation storage must unwind on a later failure.
  int first[mode + 1];
  unsigned char *prior = (unsigned char *)__builtin_alloca(16);
  first[0] = 42; prior[0] = 43;
  defer { assert(first[0] == 42 && prior[0] == 43); ++state->arrays; }
  if (mode == 0) { int values[huge]; (void)values; }
  if (mode == 1) { char values[huge][2]; (void)values; }
  if (mode == 2) (void)__builtin_alloca(huge);
  if (mode == 3) (void)__builtin_alloca_with_align(huge - 64, 1024);
  if (mode == 4) {
    storage_fail_next_allocation();
    int values[mode + 1]; storage_observe_allocation(values);
  }
  if (mode == 5) {
    storage_fail_next_allocation();
    storage_observe_allocation(__builtin_alloca(16));
  }
  assert(0);
  return 0;
}

static int dynamic_storage_group(FrameStorageState *state)
    __objc3_async_frame __attribute__((objc_task_group)) {
  int *value = (int *)__builtin_alloca(sizeof(int));
  *value = 42;
  defer { ++state->exited; }
#ifdef __cplusplus
  auto child = objc3_task_spawn(
#else
  objc3_task_t *child = objc3_task_spawn(
#endif
      ^void(void) __objc3_async_frame {
    __objc3_await storage_leaf();
    assert(*value == 42);
    ++state->after_await;
  });
  assert(child);
#ifdef __cplusplus
  state->child = objc3_task_retain(child.native_handle());
#else
  state->child = child;
#endif
  assert(objc3_task_group_add(objc3_task_group_scope_current(), state->child) ==
         OBJC3_SERVICE_OK);
  return *value;
}

static int dynamic_storage_goto(FrameStorageState *state, int count)
    __objc3_async_frame {
  unsigned loops = 0;
again:
  {
    int values[count];
    values[0] = 42;
    defer { assert(values[0] == 42); ++state->arrays; }
    __objc3_await storage_leaf();
    if (++loops != 3) goto again;
    return values[0];
  }
}
#ifdef __cplusplus
static int dynamic_storage_exception(FrameStorageState *state, int count)
    __objc3_async_frame {
  int values[count];
  values[0] = 42;
  defer { assert(values[0] == 42); ++state->arrays; }
  __objc3_await storage_leaf();
  throw 37;
}
#endif

static objc3_task_t *storage_start(objc3_executor_t *source, objc3_async_frame frame) {
  objc3_task_t *task = NULL;
  assert(objc3_async_frame_start(&frame, source, NULL, &task) == OBJC3_SERVICE_OK);
  return task;
}
static void dynamic_frame_storage_contract(objc3_executor_t *source) {
  FrameStorageState untouched = {0};
  objc3_async_frame abandoned = dynamic_frame_storage(&untouched, 7);
  assert(objc3_async_frame_dispose(&abandoned) == OBJC3_SERVICE_OK);
  assert(!untouched.entered && !untouched.exited);
  for (int cancel = 0; cancel != 2; ++cancel) {
    FrameStorageState state = {0};
    objc3_task_t *task = storage_start(source, dynamic_frame_storage(&state, 7));
    uint32_t executed = 0;
    assert(objc3_executor_pump(source, 1, &executed) == OBJC3_SERVICE_OK);
    assert(state.entered == 1 && !state.exited && !state.arrays);
    if (cancel) objc3_task_request_cancel(task);
    assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
    objc3_completion done = {0};
    assert(objc3_task_get_completion(task, &done) == OBJC3_SERVICE_OK);
    assert(state.exited == 1 && state.arrays == (cancel ? 1u : 5u));
    assert(state.after_await == (cancel ? 0u : 5u));
    if (cancel) assert(done.kind == OBJC3_COMPLETION_CANCELLED);
    else assert(done.kind == OBJC3_COMPLETION_SUCCESS && *(const int *)done.payload == 42);
    objc3_task_release(task);
  }
  {
    FrameStorageState state = {0};
    objc3_task_t *task = storage_start(source, dynamic_storage_goto(&state, 7));
    uint32_t executed = 0;
    assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
    objc3_completion done = {0};
    assert(objc3_task_get_completion(task, &done) == OBJC3_SERVICE_OK);
    assert(done.kind == OBJC3_COMPLETION_SUCCESS && *(const int *)done.payload == 42);
    assert(state.arrays == 3);
    objc3_task_release(task);
  }
#ifdef __cplusplus
  {
    FrameStorageState state = {0};
    objc3_task_t *task = storage_start(source, dynamic_storage_exception(&state, 7));
    uint32_t executed = 0;
    assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
    objc3_completion done = {0};
    assert(objc3_task_get_completion(task, &done) == OBJC3_SERVICE_OK);
    assert(done.kind == OBJC3_COMPLETION_CANCELLED && state.arrays == 1);
    objc3_task_release(task);
  }
#endif
  {
    FrameStorageState state = {0};
    objc3_task_t *task = storage_start(source, dynamic_storage_group(&state));
    uint32_t executed = 0;
    assert(objc3_executor_pump(source, 1, &executed) == OBJC3_SERVICE_OK);
    objc3_completion done = {0};
    assert(objc3_task_get_completion(task, &done) == OBJC3_SERVICE_NOT_READY);
    assert(state.exited == 1 && state.after_await == 0);
    assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
    assert(objc3_task_get_completion(task, &done) == OBJC3_SERVICE_OK);
    assert(done.kind == OBJC3_COMPLETION_SUCCESS && *(const int *)done.payload == 42);
    assert(state.after_await == 1);
    objc3_task_release(state.child);
    objc3_task_release(task);
  }
  for (int mode = 0; mode != 6; ++mode) {
    FrameStorageState state = {0};
    objc3_task_t *task = storage_start(source, dynamic_storage_overflow(&state, mode, SIZE_MAX));
    uint32_t executed = 0;
    assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
    objc3_completion done = {0};
    assert(objc3_task_get_completion(task, &done) == OBJC3_SERVICE_OK);
    assert(done.kind == OBJC3_COMPLETION_ERROR);
    objc3_service_status status = OBJC3_SERVICE_OK;
    assert(objc3_async_error_get_service(done.error, &status) == OBJC3_SERVICE_OK);
    assert(status == OBJC3_SERVICE_OUT_OF_MEMORY);
    assert(state.entered == 1 && state.exited == 1 && state.arrays == 1);
    objc3_task_release(task);
  }
}
#endif
