#ifndef ASYNC_C_COMPLETION_API_H
#define ASYNC_C_COMPLETION_API_H

#include "objc3_async_c.h"

#ifdef __OBJC__
typedef id CompletionObject;
#else
typedef void *CompletionObject;
#endif
typedef int (^CompletionBlock)(int);
typedef struct CompletionRecord {
  long values[4];
} CompletionRecord;
typedef struct CompletionCancellation {
  objc3_task_t *cancel_before_await;
  int entered;
  int resumed;
  int finished;
  int child_destroyed;
} CompletionCancellation;

OBJC3_ASYNC_C_DECLARE(completion_value, CompletionRecord, long first, int second);
OBJC3_ASYNC_C_DECLARE_VOID(completion_void);
OBJC3_ASYNC_C_DECLARE(completion_object, CompletionObject _Nullable,
                      CompletionObject _Nullable object);
OBJC3_ASYNC_C_DECLARE(completion_block, CompletionBlock _Nullable, int value);
OBJC3_ASYNC_C_DECLARE(completion_error, int, CompletionObject _Nonnull error);
OBJC3_ASYNC_C_DECLARE(completion_cancel, int, objc3_actor_t * _Nonnull actor);
OBJC3_ASYNC_C_DECLARE(completion_checked_cancel, int,
                      CompletionCancellation * _Nonnull state);
OBJC3_ASYNC_C_DECLARE(completion_worker, int, int value);

#ifdef __cplusplus
extern "C" {
#endif
CompletionObject _Nonnull completion_make_object(void)
#ifdef __OBJC__
    __attribute__((ns_returns_retained))
#endif
    ;
int completion_object_count(void);
int completion_cancel_entries(void);
int completion_error_conforms(CompletionObject _Nonnull error);
int completion_checked_cancellation(const objc3_completion * _Nonnull result,
                                    objc3_async_c_error _Nonnull expected);
int completion_checked_error(const objc3_completion * _Nonnull result,
                             objc3_async_c_error _Nonnull expected);
objc3_async_frame completion_cancel_child(CompletionCancellation * _Nonnull state);
objc3_async_frame completion_member_failure(CompletionObject _Nonnull error);
objc3_async_frame completion_join_frame(objc3_task_t * _Nonnull child);
objc3_async_frame completion_group_join_frame(objc3_task_group_t * _Nonnull group);
objc3_async_frame completion_checkpoint_frame(CompletionCancellation * _Nonnull state);
objc3_async_frame completion_narrow_frame(CompletionCancellation * _Nonnull state);
objc3_async_frame completion_optional_frame(CompletionCancellation * _Nonnull state);
objc3_async_frame completion_await_frame(CompletionCancellation * _Nonnull state);
#ifdef OBJC3_COMPLETION_PRIVATE_TASK
objc3_async_frame completion_optional_task(CompletionCancellation * _Nonnull state);
objc3_async_frame completion_await_task(CompletionCancellation * _Nonnull state);
objc3_async_frame completion_narrow_task(CompletionCancellation * _Nonnull state);
#endif
#ifdef __cplusplus
}
#endif

#endif
