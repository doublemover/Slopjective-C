#include "api.h"

#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

extern void *objc_retain(void *);
extern void objc_release(void *);
extern void *_Block_copy(const void *);
extern void _Block_release(const void *);

enum { WorkerCount = 256 };
static unsigned calls[6];
static void *retained_object;
static CompletionBlock retained_block;
static objc3_async_c_error retained_error;
static objc3_async_c_error retained_cancellation;
static void *expected_error;
static pthread_mutex_t workers_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t workers_finished = PTHREAD_COND_INITIALIZER;
static unsigned workers;

static void value_done(void *context, objc_async_completion_status_t status,
                       CompletionRecord result, objc3_async_c_error error) {
  assert(context == &calls[0] && ++calls[0] == 1);
  assert(status == OBJC_ASYNC_COMPLETION_SUCCESS && !error);
  assert(result.values[0] == 32 && result.values[1] == 42 &&
         result.values[2] == 73 && result.values[3] == 91);
}

static void void_done(void *context, objc_async_completion_status_t status,
                      objc3_async_c_error error) {
  assert(context == NULL && ++calls[1] == 1);
  assert(status == OBJC_ASYNC_COMPLETION_SUCCESS && !error);
}

static void object_done(void *context, objc_async_completion_status_t status,
                        void *result, objc3_async_c_error error) {
  assert(context == &calls[2] && ++calls[2] <= 2);
  assert(status == OBJC_ASYNC_COMPLETION_SUCCESS && !error);
  if (calls[2] == 1) {
    assert(result && completion_object_count() == 2);
    retained_object = objc_retain(result);
  } else {
    assert(result == NULL);
  }
}

static void block_done(void *context, objc_async_completion_status_t status,
                       CompletionBlock result, objc3_async_c_error error) {
  assert(context == &calls[3] && ++calls[3] == 1);
  assert(status == OBJC_ASYNC_COMPLETION_SUCCESS && !error && result(2) == 42);
  retained_block = (CompletionBlock)_Block_copy((const void *)result);
}

static void error_done(void *context, objc_async_completion_status_t status,
                       int ignored, objc3_async_c_error error) {
  (void)ignored;
  assert(context == &calls[4] && ++calls[4] == 1);
  assert(status == OBJC_ASYNC_COMPLETION_ERROR && error == expected_error);
  assert(completion_error_conforms(error) && !objc3_async_c_error_is_cancellation(error));
  retained_error = objc_retain(error);
}

static void cancel_done(void *context, objc_async_completion_status_t status,
                        int ignored, objc3_async_c_error error) {
  (void)ignored;
  assert(context == &calls[5] && ++calls[5] <= 2);
  assert(status == OBJC_ASYNC_COMPLETION_CANCELLED && error);
  assert(objc3_async_c_error_is_cancellation(error));
  assert(completion_error_conforms(error));
  if (retained_cancellation) assert(error == retained_cancellation);
  else retained_cancellation = objc_retain(error);
}

static void worker_done(void *context, objc_async_completion_status_t status,
                        int result, objc3_async_c_error error) {
  int *expected = context;
  assert(status == OBJC_ASYNC_COMPLETION_SUCCESS && !error && result == *expected);
  free(expected);
  assert(!pthread_mutex_lock(&workers_mutex));
  assert(++workers <= WorkerCount);
  assert(!pthread_cond_signal(&workers_finished));
  assert(!pthread_mutex_unlock(&workers_mutex));
}

enum CancellationPoint { Child, SuspendedParent, BeforeAwait };

static void check_cancelled_await(objc3_executor_t *executor,
                                  objc3_async_frame (*make)(CompletionCancellation *),
                                  int optional, int private_task,
                                  enum CancellationPoint point) {
  CompletionCancellation state = {0};
  objc3_async_frame frame = make(&state);
  objc3_task_t *task = NULL;
  assert(objc3_async_frame_start(&frame, executor, NULL, &task) == OBJC3_SERVICE_OK);
  unsigned executed = 0;
  if (point == BeforeAwait) state.cancel_before_await = task;
  if (point == SuspendedParent) {
    assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
    assert(executed == 1 && state.entered == 1 && state.resumed == 0);
    objc3_task_request_cancel(task);
  }
  assert(objc3_executor_pump(executor, 1000, &executed) == OBJC3_SERVICE_OK);
  objc3_completion result = {0};
  assert(objc3_task_get_completion(task, &result) == OBJC3_SERVICE_OK);
  if (optional) {
    assert(result.kind == OBJC3_COMPLETION_SUCCESS && result.has_payload);
    assert(*(const int *)result.payload == 42);
  } else {
    assert(result.kind == OBJC3_COMPLETION_CANCELLED && !result.has_payload);
  }
  objc3_task_release(task);
  assert(state.entered == 1 && state.resumed == optional && state.finished == 1);
  assert(state.child_destroyed == (point != BeforeAwait || private_task));
}

int main(void) {
  objc3_executor_t *manual = NULL;
  assert(objc3_executor_get_main(&manual) == OBJC3_SERVICE_OK);
  objc3_actor_t *actor = NULL;
  assert(objc3_actor_create(manual, &actor) == OBJC3_SERVICE_OK);
  void *object = completion_make_object();
  expected_error = completion_make_object();
  assert(object && expected_error && completion_object_count() == 2);
  completion_value_async_c(32, 41, &calls[0], value_done);
  completion_void_async_c(NULL, void_done);
  completion_object_async_c(object, &calls[2], object_done);
  completion_object_async_c(NULL, &calls[2], object_done);
  completion_block_async_c(39, &calls[3], block_done);
  completion_error_async_c(expected_error, &calls[4], error_done);
  completion_cancel_async_c(actor, &calls[5], cancel_done);
  for (unsigned index = 0; index < 6; ++index) assert(!calls[index]);
  assert(objc3_actor_cancel(actor) == OBJC3_SERVICE_OK);
  unsigned executed = 0;
  assert(objc3_executor_pump(manual, 1000, &executed) == OBJC3_SERVICE_OK);
  assert(calls[0] == 1 && calls[1] == 1 && calls[2] == 2 && calls[3] == 1 &&
         calls[4] == 1 && calls[5] == 1);
  // The unconsumed MRC parameters borrow their inputs through the call.
  objc_release(object);
  objc_release(expected_error);
  assert(retained_object && retained_error && retained_block && retained_cancellation);
  assert(completion_object_count() == 2 && retained_block(2) == 42);
  assert(completion_error_conforms(retained_error));
  completion_cancel_async_c(actor, &calls[5], cancel_done);
  assert(calls[5] == 2 && completion_cancel_entries() == 0);
  assert(objc3_async_c_error_is_cancellation(retained_cancellation));
  assert(!objc3_async_c_error_is_cancellation(NULL));
  objc_release(retained_object);
  objc_release(retained_error);
  objc_release(retained_cancellation);
  _Block_release((const void *)retained_block);
  assert(completion_object_count() == 0);
  objc3_actor_release(actor);
  for (int point = Child; point <= BeforeAwait; ++point) {
    check_cancelled_await(manual, completion_await_frame, 0, 0, point);
    check_cancelled_await(manual, completion_optional_frame, 1, 0, point);
#ifdef OBJC3_COMPLETION_PRIVATE_TASK
    check_cancelled_await(manual, completion_await_task, 0, 1, point);
    check_cancelled_await(manual, completion_optional_task, 1, 1, point);
#endif
  }
  objc3_executor_release(manual);

  for (int index = 0; index < WorkerCount; ++index) {
    int *expected = malloc(sizeof(*expected));
    assert(expected);
    *expected = index + 1;
    completion_worker_async_c(index, expected, worker_done);
  }
  struct timespec deadline;
  assert(timespec_get(&deadline, TIME_UTC) == TIME_UTC);
  deadline.tv_sec += 10;
  assert(!pthread_mutex_lock(&workers_mutex));
  while (workers != WorkerCount)
    assert(!pthread_cond_timedwait(&workers_finished, &workers_mutex, &deadline));
  assert(!pthread_mutex_unlock(&workers_mutex));
}
