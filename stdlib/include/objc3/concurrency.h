#ifndef OBJC3_CONCURRENCY_H
#define OBJC3_CONCURRENCY_H

#include "core.h"
#include <objc3c/runtime/objc3_runtime_services.h>

#define OBJC3_CONCURRENCY_CAPABILITY_ID "objc3.cap.concurrency"

/* The shared C task/frame, group, executor and actor ABI is available in both
 * source languages. Child spawn requires an active task and inherits its
 * dynamic destination and cancellation tree. Detached spawn selects the
 * canonical global executor without inheriting parent cancellation. Callers
 * explicitly join owned tasks/groups; these functions do not establish an
 * implicit structured group scope. */

/* Native async callable lowering is currently admitted in the compatibility
 * profile. Other profiles can use the shared C service ABI above without
 * eagerly parsing convenience bodies whose source-call ABI is not admitted. */
#if defined(__OBJC__) && __OBJC3_PROFILE__ == 0
#ifdef __cplusplus
#include <objc3c/runtime/objc3_task.hpp>

/* Both native async entry representations preserve the source result type.
 * A Block factory owns its captures in the lazy frame before returning, in ARC
 * and MRC. Startup errors throw service_failure; the lazy owner reclaims the
 * frame on every failed start. The annotated source call checks the original
 * Block's captures before any library forwarding or ABI conversion. */
template <class R>
inline objc3::task_handle<R>
objc3_task_spawn(R (^work)(void) __objc3_async)
    __attribute__((objc_task_spawn)) {
  if (!work) throw objc3::service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
  return std::move(work()).start_child();
}
template <class R>
inline objc3::task_handle<R>
objc3_task_spawn(R (^work)(void) __objc3_async_frame)
    __attribute__((objc_task_spawn)) {
  if (!work) throw objc3::service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
  return std::move(__objc3_async_from_frame<R>(work())).start_child();
}
template <class R>
inline objc3::task_handle<R>
objc3_task_detached(R (^work)(void) __objc3_async)
    __attribute__((objc_task_detached)) {
  if (!work) throw objc3::service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
  return std::move(work()).detach();
}
template <class R>
inline objc3::task_handle<R>
objc3_task_detached(R (^work)(void) __objc3_async_frame)
    __attribute__((objc_task_detached)) {
  if (!work) throw objc3::service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
  return std::move(__objc3_async_from_frame<R>(work())).detach();
}
/* Checked effects remain on the Block's type. Invoking its factory creates a
 * lazy owned frame; checked failure is observed when joining the resulting
 * task. E is deduced from the native effect metadata, never erased. */
template <class R, class E>
inline objc3::task_handle<R>
objc3_task_spawn(R (^work)(void) __objc3_async __objc3_throws(E))
    __attribute__((objc_task_spawn)) {
  if (!work) throw objc3::service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
  return std::move(work()).start_child();
}
template <class R, class E>
inline objc3::task_handle<R>
objc3_task_spawn(R (^work)(void) __objc3_async_frame __objc3_throws(E))
    __attribute__((objc_task_spawn)) {
  if (!work) throw objc3::service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
  return std::move(__objc3_async_from_frame<R>(work())).start_child();
}
template <class R, class E>
inline objc3::task_handle<R>
objc3_task_detached(R (^work)(void) __objc3_async __objc3_throws(E))
    __attribute__((objc_task_detached)) {
  if (!work) throw objc3::service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
  return std::move(work()).detach();
}
template <class R, class E>
inline objc3::task_handle<R>
objc3_task_detached(R (^work)(void) __objc3_async_frame __objc3_throws(E))
    __attribute__((objc_task_detached)) {
  if (!work) throw objc3::service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
  return std::move(__objc3_async_from_frame<R>(work())).detach();
}
template <class R>
inline objc3::task_handle<R>
objc3_task_spawn(R (^work)(void) __objc3_async __objc3_throws)
    __attribute__((objc_task_spawn)) {
  if (!work) throw objc3::service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
  return std::move(work()).start_child();
}
template <class R>
inline objc3::task_handle<R>
objc3_task_spawn(R (^work)(void) __objc3_async_frame __objc3_throws)
    __attribute__((objc_task_spawn)) {
  if (!work) throw objc3::service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
  return std::move(__objc3_async_from_frame<R>(work())).start_child();
}
template <class R>
inline objc3::task_handle<R>
objc3_task_detached(R (^work)(void) __objc3_async __objc3_throws)
    __attribute__((objc_task_detached)) {
  if (!work) throw objc3::service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
  return std::move(work()).detach();
}
template <class R>
inline objc3::task_handle<R>
objc3_task_detached(R (^work)(void) __objc3_async_frame __objc3_throws)
    __attribute__((objc_task_detached)) {
  if (!work) throw objc3::service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
  return std::move(__objc3_async_from_frame<R>(work())).detach();
}
#else
/* The C convenience surface accepts void async Blocks and returns one owned
 * task handle, or null on invalid input/startup failure. Use the status-returning
 * frame operations directly when failure detail or a nonvoid C result is
 * required. Untyped checked-throw Blocks retain their distinct callable type;
 * typed C checked errors continue through the lower-level frame API. */
typedef void (^objc3_task_work)(void) __objc3_async_frame;
typedef void (^objc3_task_throwing_work)(void) __objc3_async_frame __objc3_throws;

static inline objc3_task_t *objc3_task_spawn(objc3_task_work work)
    __attribute__((overloadable, objc_task_spawn)) {
  if (!work) return NULL;
  objc3_async_frame frame = work();
  objc3_task_t *task = NULL;
  if (objc3_async_frame_spawn(&frame, &task) != OBJC3_SERVICE_OK)
    (void)objc3_async_frame_dispose(&frame);
  return task;
}
static inline objc3_task_t *objc3_task_detached(objc3_task_work work)
    __attribute__((overloadable, objc_task_detached)) {
  if (!work) return NULL;
  objc3_async_frame frame = work();
  objc3_task_t *task = NULL;
  if (objc3_async_frame_detach(&frame, &task) != OBJC3_SERVICE_OK)
    (void)objc3_async_frame_dispose(&frame);
  return task;
}
static inline objc3_task_t *objc3_task_spawn(objc3_task_throwing_work work)
    __attribute__((overloadable, objc_task_spawn)) {
  if (!work) return NULL;
  objc3_async_frame frame = work();
  objc3_task_t *task = NULL;
  if (objc3_async_frame_spawn(&frame, &task) != OBJC3_SERVICE_OK)
    (void)objc3_async_frame_dispose(&frame);
  return task;
}
static inline objc3_task_t *objc3_task_detached(objc3_task_throwing_work work)
    __attribute__((overloadable, objc_task_detached)) {
  if (!work) return NULL;
  objc3_async_frame frame = work();
  objc3_task_t *task = NULL;
  if (objc3_async_frame_detach(&frame, &task) != OBJC3_SERVICE_OK)
    (void)objc3_async_frame_dispose(&frame);
  return task;
}
#endif

/* Direct recognized calls preserve one evaluation, capture diagnostics and
 * child/detached metadata at the user's original macro argument site. */
#define OBJC_TASK_SPAWN(block_expr) objc3_task_spawn((block_expr))
#define OBJC_TASK_DETACHED(block_expr) objc3_task_detached((block_expr))
#endif

#endif
