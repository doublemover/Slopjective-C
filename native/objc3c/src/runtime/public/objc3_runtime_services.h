#ifndef OBJC3_RUNTIME_SERVICES_H
#define OBJC3_RUNTIME_SERVICES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OBJC3_SERVICE_ABI_VERSION 2u
#define OBJC3_VALUE_OPS_VERSION 1u

typedef struct objc3_task objc3_task_t;
typedef struct objc3_executor objc3_executor_t;
typedef struct objc3_error objc3_error_t;

typedef enum objc3_service_status {
  OBJC3_SERVICE_OK = 0,
  OBJC3_SERVICE_INVALID_ARGUMENT = 1,
  OBJC3_SERVICE_UNSUPPORTED_VERSION = 2,
  OBJC3_SERVICE_OUT_OF_MEMORY = 3,
  OBJC3_SERVICE_ALREADY_COMPLETED = 4,
  OBJC3_SERVICE_NOT_READY = 5,
  OBJC3_SERVICE_CLOSED = 6
} objc3_service_status;

/* Callbacks do not throw across this C ABI. Successful initialization creates
 * one owned value; failed initialization leaves destination uninitialized.
 * move_init leaves source uninitialized only on success. Descriptor callbacks
 * and their code must outlive every value using them. */
typedef struct objc3_value_ops {
  uint32_t struct_size;
  uint32_t version;
  uint64_t payload_size;
  uint64_t payload_alignment;
  objc3_service_status (*copy_init)(void *destination, const void *source);
  objc3_service_status (*move_init)(void *destination, void *source);
  void (*destroy)(void *value);
} objc3_value_ops;

objc3_service_status objc3_value_ops_validate(const objc3_value_ops *ops);

/* An error owns an independent copy of its payload, including a zero payload.
 * No native Objective-C reference or C++ exception is implicitly converted. */
objc3_service_status objc3_error_create(const void *payload,
                                      const objc3_value_ops *ops,
                                      objc3_error_t **out_error);
objc3_error_t *objc3_error_retain(objc3_error_t *error);
void objc3_error_release(objc3_error_t *error);
const void *objc3_error_payload(const objc3_error_t *error);
const objc3_value_ops *objc3_error_payload_ops(const objc3_error_t *error);

typedef enum objc3_completion_kind {
  OBJC3_COMPLETION_SUCCESS = 0,
  OBJC3_COMPLETION_ERROR = 1,
  OBJC3_COMPLETION_CANCELLED = 2
} objc3_completion_kind;

/* has_payload, not the payload bits, encodes presence. Void success has
 * has_payload=0. Success with zero/false has has_payload=1. Completion views
 * are borrowed for callback scope; retaining the error or copying the payload
 * is required to keep them. Cancelled completion carries neither. */
typedef struct objc3_completion {
  uint32_t struct_size;
  uint32_t version;
  objc3_completion_kind kind;
  uint32_t has_payload;
  const void *payload;
  const objc3_value_ops *payload_ops;
  objc3_error_t *error;
} objc3_completion;

typedef void (*objc3_completion_callback)(void *context,
                                        const objc3_completion *completion);
typedef void (*objc3_task_resume)(objc3_task_t *task, void *frame);
typedef void (*objc3_frame_destroy)(void *frame);
typedef void (*objc3_executor_callback)(void *context);

/* start transfers frame ownership only on success. A task keeps its executor
 * alive, owns its stored outcome until final release, invokes each registered
 * completion callback once, and destroys its frame exactly once after terminal
 * completion and after active resume returns. Synchronous completion is valid.
 * Merely returning from resume without completing is suspension. */
typedef struct objc3_task_start_options {
  uint32_t struct_size;
  uint32_t version;
  objc3_executor_t *executor;
  void *frame;
  objc3_task_resume resume;
  objc3_frame_destroy destroy;
  objc3_completion_callback completion;
  void *completion_context;
} objc3_task_start_options;

objc3_service_status objc3_task_start(const objc3_task_start_options *options,
                                     objc3_task_t **out_task);
objc3_task_t *objc3_task_retain(objc3_task_t *task);
void objc3_task_release(objc3_task_t *task);
void objc3_task_request_cancel(objc3_task_t *task);
int objc3_task_is_cancel_requested(const objc3_task_t *task);
objc3_service_status objc3_task_schedule_resume(objc3_task_t *task);
/* complete copies a successful payload and retains an error; the caller keeps
 * ownership of its supplied view. It rejects invalid states and duplicate
 * completion. Runtime locks must never enclose user callbacks/destruction. */
objc3_service_status objc3_task_complete(objc3_task_t *task,
                                        const objc3_completion *completion);
objc3_service_status objc3_task_add_waiter(objc3_task_t *task,
                                          objc3_completion_callback callback,
                                          void *context);
/* wait blocks until terminal completion; it does not pump a manual executor.
 * A pending wait from the task itself or its current serial executor returns
 * NOT_READY so queued task work can continue. Completed waits remain valid. */
objc3_service_status objc3_task_wait(objc3_task_t *task);

/* A serial executor runs one callback at a time. Manual executors advance only
 * through pump, allowing explicit main-thread ownership. Worker executors own
 * a bounded scheduling thread. enqueue transfers no ownership of context. */
objc3_service_status objc3_executor_create(uint32_t manual,
                                         objc3_executor_t **out_executor);
objc3_executor_t *objc3_executor_retain(objc3_executor_t *executor);
void objc3_executor_release(objc3_executor_t *executor);
objc3_service_status objc3_executor_enqueue(objc3_executor_t *executor,
                                            objc3_executor_callback callback,
                                            void *context);
objc3_service_status objc3_executor_pump(objc3_executor_t *executor,
                                         uint32_t limit,
                                         uint32_t *out_executed);

#ifdef __cplusplus
}
#endif
#endif
