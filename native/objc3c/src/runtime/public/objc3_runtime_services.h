#ifndef OBJC3_RUNTIME_SERVICES_H
#define OBJC3_RUNTIME_SERVICES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OBJC3_SERVICE_ABI_VERSION 3u
#define OBJC3_VALUE_OPS_VERSION 1u

typedef struct objc3_task objc3_task_t;
typedef struct objc3_executor objc3_executor_t;
typedef struct objc3_error objc3_error_t;
typedef struct objc3_task_group objc3_task_group_t;
typedef struct objc3_actor objc3_actor_t;

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

/* start transfers frame ownership only on success. Select exactly one of
 * executor or actor. An actor task resumes through its serial mailbox, including
 * after dependencies and yields; suspension permits other mailbox jobs to run.
 * A task keeps its selected destination
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
  objc3_actor_t *actor;
} objc3_task_start_options;

objc3_service_status objc3_task_start(const objc3_task_start_options *options,
                                     objc3_task_t **out_task);
/* Start a private child from the parent's active resume, inheriting its current
 * destination. Both destination fields in options must be null. Ownership
 * transfers only on success, as for start. No parent-child ownership is implied. */
objc3_service_status objc3_task_start_child(
    objc3_task_t *parent, const objc3_task_start_options *options,
    objc3_task_t **out_task);
objc3_task_t *objc3_task_retain(objc3_task_t *task);
void objc3_task_release(objc3_task_t *task);
void objc3_task_request_cancel(objc3_task_t *task);
int objc3_task_is_cancel_requested(const objc3_task_t *task);
objc3_service_status objc3_task_schedule_resume(objc3_task_t *task);
/* Change destination from this task's active resume and request a queued
 * continuation. Select exactly one of executor or actor. Always suspend after
 * success, including a same-destination hop. Later yields/dependencies retain
 * this destination until another hop. Resumption cannot overlap the active
 * callback. A failed deferred dispatch terminalizes as cancellation. */
objc3_service_status objc3_task_hop(objc3_task_t *task,
                                   objc3_executor_t *executor,
                                   objc3_actor_t *actor);
/* complete copies a successful payload and retains an error; the caller keeps
 * ownership of its supplied view. It rejects invalid states and duplicate
 * completion. Runtime locks must never enclose user callbacks/destruction. */
objc3_service_status objc3_task_complete(objc3_task_t *task,
                                        const objc3_completion *completion);
objc3_service_status objc3_task_add_waiter(objc3_task_t *task,
                                          objc3_completion_callback callback,
                                          void *context);
/* Register one dependency from the parent's active resume. Always suspend
 * after success, even for a completed dependency: continuation runs on the
 * parent's executor. Cancellation wakes a suspended parent without cancelling
 * a shared dependency. Self-await and overlapping awaits are rejected. Keep
 * the dependency alive while its completion is needed. Dispatch failure
 * cancels the parent, rather than leaving an unreachable suspended frame. */
objc3_service_status objc3_task_await(objc3_task_t *parent,
                                     objc3_task_t *dependency);
/* Nonblocking terminal view, borrowed until the retained task is released.
 * NOT_READY and invalid arguments leave out_completion unchanged. */
objc3_service_status objc3_task_get_completion(
    objc3_task_t *task, objc3_completion *out_completion);
/* wait blocks until terminal completion; it does not pump a manual executor.
 * A pending wait from the task itself or any runtime executor callback returns
 * NOT_READY so queued task work can continue. Completed waits remain valid. */
objc3_service_status objc3_task_wait(objc3_task_t *task);

/* A serial executor runs one callback at a time. Manual executors advance only
 * through pump, allowing explicit main-thread ownership. Worker executors own
 * a bounded scheduling thread. enqueue transfers no ownership of context.
 * When the Objective-C runtime is linked, callbacks have an autorelease pool
 * that drains after they return. Retain values needed after callback scope. */
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

/* A group owns its registered tasks and child groups. The parent association
 * is optional. Close recursively prevents new registrations; cancel requests
 * cooperative cancellation without closing. The first failed task cancels its
 * siblings and propagates through the parent tree before publishing completion.
 * Close and join before releasing the last caller-owned reference. */
objc3_service_status objc3_task_group_create(objc3_task_group_t *parent,
                                            objc3_task_group_t **out_group);
objc3_task_group_t *objc3_task_group_retain(objc3_task_group_t *group);
void objc3_task_group_release(objc3_task_group_t *group);
objc3_service_status objc3_task_group_add(objc3_task_group_t *group,
                                         objc3_task_t *task);
objc3_service_status objc3_task_group_close(objc3_task_group_t *group);
objc3_service_status objc3_task_group_cancel(objc3_task_group_t *group);
/* next never blocks or pumps. It returns one retained direct child task in
 * completion order, NOT_READY while none is ready, or CLOSED after a closed
 * group drains. Nested groups are joined recursively, not flattened here. */
objc3_service_status objc3_task_group_next(objc3_task_group_t *group,
                                          objc3_task_t **out_task);
/* join waits for closed groups, terminal tasks, callbacks and frame cleanup.
 * A wait that would block a runtime executor callback or a manual task returns
 * NOT_READY. The caller can pump a manual executor and retry. */
objc3_service_status objc3_task_group_join(objc3_task_group_t *group);

#define OBJC3_ACTOR_SERVICE_VERSION 1u
typedef void (*objc3_actor_job)(void *context);
typedef void (*objc3_actor_context_destroy)(void *context);

/* These are logical actor/executor identities, independent of physical thread
 * identity. A different actor on the same executor still requires an actor hop.
 * Initialize struct_size and version before calling either facts API. */
typedef struct objc3_actor_hop_facts {
  uint32_t struct_size;
  uint32_t version;
  uint64_t actor_identity;
  uint64_t current_actor_identity;
  uint64_t target_executor_identity;
  uint64_t current_executor_identity;
  uint32_t actor_hop;
  uint32_t executor_hop;
} objc3_actor_hop_facts;

typedef struct objc3_current_actor_facts {
  uint32_t struct_size;
  uint32_t version;
  uint64_t actor_identity;
  uint64_t executor_identity;
} objc3_current_actor_facts;

objc3_service_status objc3_actor_create(objc3_executor_t *executor,
                                       objc3_actor_t **out_actor);
objc3_actor_t *objc3_actor_retain(objc3_actor_t *actor);
void objc3_actor_release(objc3_actor_t *actor);
uint64_t objc3_actor_identity(const objc3_actor_t *actor);
uint64_t objc3_actor_executor_identity(const objc3_actor_t *actor);
/* On success ownership of context transfers to the actor. destroy runs once
 * after job returns, or after cancellation discards the queued job. On failure
 * the caller keeps ownership and out_facts is unchanged. Jobs are serialized;
 * enqueue records the hop and always queues, including same-actor reentry. */
objc3_service_status objc3_actor_enqueue(objc3_actor_t *actor,
                                        objc3_actor_job job, void *context,
                                        objc3_actor_context_destroy destroy,
                                        objc3_actor_hop_facts *out_facts);
objc3_service_status objc3_actor_close(objc3_actor_t *actor);
objc3_service_status objc3_actor_cancel(objc3_actor_t *actor);
/* join includes context cleanup. Active jobs finish cooperatively. Manual or
 * current-executor pending waits return NOT_READY; a callback cannot join its
 * own actor. Last caller release cancels unexecuted mailbox jobs. */
objc3_service_status objc3_actor_join(objc3_actor_t *actor);
objc3_service_status objc3_actor_current_facts(objc3_current_actor_facts *out_facts);

#ifdef __cplusplus
}
#endif
#endif
