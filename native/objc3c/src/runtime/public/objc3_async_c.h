#ifndef OBJC3_ASYNC_C_H
#define OBJC3_ASYNC_C_H

#include "objc3_runtime_services.h"

#ifdef __OBJC__
@protocol Error;
typedef id<Error> objc3_async_c_error;
#else
typedef void *objc3_async_c_error;
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum objc_async_completion_status {
  OBJC_ASYNC_COMPLETION_SUCCESS = 0,
  OBJC_ASYNC_COMPLETION_ERROR = 1,
  OBJC_ASYNC_COMPLETION_CANCELLED = 2
} objc_async_completion_status_t;

/* Recognizes the process-wide Objective-C cancellation error. Accepts null.
 * The error and object/Block results supplied to C callbacks are borrowed
 * until callback return. Retain or copy them to keep them past that point. */
int objc3_async_c_error_is_cancellation(objc3_async_c_error error);

/* Return the borrowed cancellation error for the active task, or null. */
objc3_async_c_error objc3_task_current_cancellation_error(void);

/* Start a frame without exporting a task handle. Two null destinations select
 * the global executor. Success clears the carrier before queuing work and the
 * task remains owned until its one completion callback returns. The callback
 * may run before submit returns. Failure preserves the carrier and never calls
 * completion. The opaque context belongs to the caller. */
objc3_service_status objc3_async_frame_submit(
    objc3_async_frame *frame, objc3_executor_t *executor, objc3_actor_t *actor,
    objc3_completion_callback completion, void *context);

#if defined(__OBJC__) && defined(__BLOCKS__)
/* Backing operations for the typed export declarations. */
typedef void (^__objc3_async_c_delivery)(const objc3_completion *);
void __objc3_async_c_submit(objc3_async_frame *, objc3_executor_t *,
                           objc3_actor_t *, int checked,
                           __objc3_async_c_delivery);
objc3_async_c_error __objc3_async_c_completion_error(const objc3_completion *);
void __objc3_async_c_check_result(const objc3_async_frame *,
                                 objc3_async_result_kind, const char *,
                                 size_t, size_t);
#endif

#ifdef __cplusplus
}
#define __OBJC3_ASYNC_C_LINKAGE extern "C"
#else
#define __OBJC3_ASYNC_C_LINKAGE extern
#endif

#if defined(__OBJC__) && defined(__OBJC3_LANGUAGE_REVISION__)
/* Throw at this explicit cooperative checkpoint if cancellation is requested. */
static inline void objc3_task_check_cancellation(void)
    __objc3_throws(id<Error>) {
  id<Error> error = objc3_task_current_cancellation_error();
  if (error) __objc3_throw(error);
}

/* Retain, close and await group quiescence, then propagate the first checked
 * failure or explicit group cancellation. An interrupted wait throws
 * cancellation without cancelling independently owned member tasks. */
__OBJC3_ASYNC_C_LINKAGE void objc3_task_group_join_checked(objc3_task_group_t *)
    __objc3_async_frame __objc3_throws(id<Error>)
    __asm__("__objc3_task_group_join_checked_frame");
#endif

/* Declare the same typed native entry in both provider and C client headers.
 * Extra arguments are the original parameter declarations, in source order.
 * Keep Objective-C object/Block ABI typedefs usable from C as opaque pointers
 * (or native Blocks when the C compiler has Blocks enabled). */
#define OBJC3_ASYNC_C_DECLARE(Name, Result, ...)                                \
  typedef void (*Name##_completion_t)(void * _Nullable,                        \
      objc_async_completion_status_t, Result, objc3_async_c_error _Nullable);  \
  __OBJC3_ASYNC_C_LINKAGE void Name##_async_c(                                  \
      __VA_OPT__(__VA_ARGS__,) void * _Nullable, Name##_completion_t _Nonnull)

#define OBJC3_ASYNC_C_DECLARE_VOID(Name, ...)                                   \
  typedef void (*Name##_completion_t)(void * _Nullable,                        \
      objc_async_completion_status_t, objc3_async_c_error _Nullable);           \
  __OBJC3_ASYNC_C_LINKAGE void Name##_async_c(                                  \
      __VA_OPT__(__VA_ARGS__,) void * _Nullable, Name##_completion_t _Nonnull)

#endif
