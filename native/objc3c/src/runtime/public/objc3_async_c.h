#ifndef OBJC3_ASYNC_C_H
#define OBJC3_ASYNC_C_H

#include "objc3_runtime_services.h"

#ifdef __clang__
#pragma clang diagnostic push
// C17 and C++ clients use Clang's nullability types for Swift import.
#pragma clang diagnostic ignored "-Wnullability-extension"
#endif

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
int objc3_async_c_error_is_cancellation(objc3_async_c_error _Nullable error);

/* Explicit ownership for C and foreign clients that preserve a +0 native
 * error past its callback or autorelease scope. Both functions accept null.
 * The retained pointer carries the original Objective-C object identity. */
void * _Nullable objc3_async_c_error_retain(void * _Nullable error);
void objc3_async_c_error_release(void * _Nullable error);

/* Pair these calls on one thread around a synchronous foreign entry. */
void * _Nullable objc3_c_autorelease_pool_push(void);
void objc3_c_autorelease_pool_pop(void * _Nullable token);

/* Return the borrowed cancellation error for the active task, or null. */
objc3_async_c_error _Nullable objc3_task_current_cancellation_error(void);

/* Start a frame without exporting a task handle. Two null destinations select
 * the global executor. Success clears the carrier before queuing work and the
 * task remains owned until its one completion callback returns. The callback
 * may run before submit returns. Failure preserves the carrier and never calls
 * completion. The opaque context belongs to the caller. */
objc3_service_status objc3_async_frame_submit(
    objc3_async_frame * _Nullable frame, objc3_executor_t * _Nullable executor,
    objc3_actor_t * _Nullable actor, objc3_completion_callback _Nullable completion,
    void * _Nullable context);

/* Submit with the same completion ownership and return a separate retained
 * task. The caller can request cancellation and must release the handle.
 * Releasing it leaves the completion's lease active. The caller keeps the
 * output slot valid until submission returns. Failure stores null in it. */
objc3_service_status objc3_async_frame_submit_task(
    objc3_async_frame * _Nullable frame, objc3_executor_t * _Nullable executor,
    objc3_actor_t * _Nullable actor, objc3_completion_callback _Nullable completion,
    void * _Nullable context, objc3_task_t * _Nullable * _Nullable out_task);

#if defined(__OBJC__) && defined(__BLOCKS__)
/* Backing operations for the typed export declarations. */
typedef void (^__objc3_async_c_delivery)(const objc3_completion * _Nonnull);
void __objc3_async_c_submit_with_task(objc3_async_frame * _Nonnull,
                                    objc3_executor_t * _Nullable, objc3_actor_t * _Nullable,
                                    int checked, __objc3_async_c_delivery _Nonnull,
                                    objc3_task_t * _Nullable * _Nullable);
objc3_async_c_error _Nullable __objc3_async_c_completion_error(const objc3_completion * _Nonnull);
void __objc3_async_c_check_result(const objc3_async_frame * _Nonnull,
                                 objc3_async_result_kind, const char * _Nonnull,
                                 size_t, size_t);
#endif

#ifdef __OBJC__
/* Backing operation for a generated MRC export's retainable +1 handoff. */
void * _Nullable __objc3_c_export_retain(void * _Nullable value, int block);
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
__OBJC3_ASYNC_C_LINKAGE void objc3_task_group_join_checked(objc3_task_group_t * _Nullable)
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

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif
