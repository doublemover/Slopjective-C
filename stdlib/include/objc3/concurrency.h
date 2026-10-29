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

/* Native C-frame and C++ task functions/methods marked objc_task_group own a
 * fresh group
 * from first resume through implicit finalization. Obtain its borrowed handle
 * with objc3_task_group_scope_current(), and add only structured descendants
 * created by child spawn/start. Normal exit closes and joins; checked failure,
 * cancellation and lost continuations also cancel unfinished scope work.
 * Defers/body locals follow lexical exit; frame captures and staged results/
 * errors remain owned through the join. Exceptions-enabled definitions require
 * Objective-C exception handling for C frames. C++ task coroutines use the
 * reserved scope-aware promise protocol; pending results and native/checked
 * errors remain in the final-suspended frame until quiescence. Their completion
 * notification publishes without resuming the completed coroutine. C++ callers
 * may use task_group_handle::current_scope() for a retained group view. */

/* Native async callable lowering is currently admitted in the compatibility
 * profile. Other profiles can use the shared C service ABI above without
 * eagerly parsing convenience bodies whose source-call ABI is not admitted. */
#if defined(__OBJC__) && __OBJC3_PROFILE__ == 0
#ifdef __cplusplus
#include <objc3c/runtime/objc3_task.hpp>

namespace objc3 {
/* The callback returns a lazy task and receives an independently retained group
 * view. Keep its callable object in this frame through callback completion and
 * the final group join, including coroutine callbacks that borrow their closure.
 * Tasks added by the callback must be structured descendants. */
template <class T = void, class Work>
task<T> with_task_group(Work work) __attribute__((objc_task_group)) {
  auto scope = task_group_handle::current_scope();
  // User iteration/join must not include the callback itself. The private
  // scope joins both this child group and the callback's own frame cleanup.
  auto group = scope.child();
  // Awaiting completion alone does not join callback frame/callback cleanup.
  // Registration must precede dispatch, including failed-start paths.
  auto body = [&]() {
    if constexpr (std::is_same_v<decltype(work(group)),objc3_async_frame>) {
      auto frame=work(group);
      try {return task<T>::from_native(&frame);}
      catch (...) {(void)objc3_async_frame_dispose(&frame);throw;}
    } else return work(group);
  }();
  auto callback = std::move(body).__objc3_start_group_body(scope);
  if constexpr (std::is_void_v<T>) {
    co_await callback;
    co_return;
  } else {
    co_return co_await callback;
  }
}
} // namespace objc3

/* C++ callable spawn owns its closure until native frame teardown, not merely
 * until completion publication. Result, cancellation, executor and structured
 * parent contracts are the same as direct task/frame spawn. */
template<class Work> requires requires(Work &work) {work().release_native();}
inline auto objc3_task_spawn(Work work) __attribute__((objc_task_spawn)) {
  using Task=decltype(work());
  return std::move(objc3::task_detail::own_callable_as<Task>(std::move(work))).start_child();
}
template<class Work> requires requires(Work &work) {work().release_native();}
inline auto objc3_task_detached(Work work) __attribute__((objc_task_detached)) {
  using Task=decltype(work());
  return std::move(objc3::task_detail::own_callable_as<Task>(std::move(work))).detach();
}
template<class R,class Work> requires requires(Work &work) {
  requires std::is_same_v<decltype(work()),objc3_async_frame>;
}
inline objc3::task_handle<R> objc3_task_spawn(Work work) __attribute__((objc_task_spawn)) {
  return std::move(objc3::task_detail::own_callable_as<objc3::task<R>>(std::move(work))).start_child();
}
template<class R,class Work> requires requires(Work &work) {
  requires std::is_same_v<decltype(work()),objc3_async_frame>;
}
inline objc3::task_handle<R> objc3_task_detached(Work work) __attribute__((objc_task_detached)) {
  return std::move(objc3::task_detail::own_callable_as<objc3::task<R>>(std::move(work))).detach();
}

/* Register the frame in the selected group before dispatch, while its adapter
 * owns the callable through complete inner-frame teardown. */
template<class Work> requires requires(Work &work) {work().release_native();}
inline auto objc3_task_spawn_in_group(objc3::task_group_handle group,Work work)
    __attribute__((objc_task_spawn)) {
  using Task=decltype(work());
  return std::move(objc3::task_detail::own_callable_as<Task>(std::move(work))).start_child(group);
}
template<class R,class Work> requires requires(Work &work) {
  requires std::is_same_v<decltype(work()),objc3_async_frame>;
}
inline objc3::task_handle<R> objc3_task_spawn_in_group(objc3::task_group_handle group,Work work)
    __attribute__((objc_task_spawn)) {
  return std::move(objc3::task_detail::own_callable_as<objc3::task<R>>(std::move(work))).start_child(group);
}

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

#ifndef __cplusplus
/* Declare a typed native await entry for an existing C task handle. The result
 * is validated against the task's canonical descriptor before copy-out; a
 * mismatched declaration reports TYPE_MISMATCH. Typedefs support records,
 * objects and Blocks without erasing their native ownership/result kind.
 * Checked task errors retain their source domain on the explicit variant.
 * These declarations use the shared frame ABI, not a source callback body. */
#define OBJC_TASK_JOIN_DECLARE(name, result_type) \
  extern result_type name(objc3_task_t *) __objc3_async_frame \
      __asm__("__objc3_task_join_frame")
#define OBJC_TASK_JOIN_DECLARE_THROWING(name, result_type, error_type) \
  extern result_type name(objc3_task_t *) __objc3_async_frame \
      __objc3_throws(error_type) __asm__("__objc3_task_join_frame")

/* Declare completion-order native iteration returning Optional(T). Typedefs
 * support record, object and Block element types. The compiler supplies exact
 * Optional layout and ownership operations. Construction retains the group
 * immediately; first resume closes its tree. A present nil remains Some(nil).
 * A typed mismatch leaves the selected task available for another consumer.
 * The physical local shim has the ordinary shared-frame C ABI; callers see
 * only the logical async declaration, including its checked error domain. */
extern objc3_async_frame __objc3_task_group_next_frame(
    objc3_task_group_t *, const objc3_async_group_next_plan *);
#define __OBJC3_GROUP_NEXT_DECLARE(name, element_type, effects) \
  static objc3_async_frame name##_objc3_physical(objc3_task_group_t *) \
      __asm__("__objc3_group_next_" #name); \
  static Optional(element_type) name(objc3_task_group_t *) \
      __objc3_async_frame effects \
      __attribute__((alias("__objc3_group_next_" #name), unused)); \
  __attribute__((used)) static objc3_async_frame \
  name##_objc3_physical(objc3_task_group_t *group) { \
    return __objc3_task_group_next_frame(group, \
        (const objc3_async_group_next_plan *) \
        __builtin_objc3_async_group_next_plan((Optional(element_type) *)0)); \
  }
#define OBJC_TASK_GROUP_NEXT_DECLARE(name, element_type) \
  __OBJC3_GROUP_NEXT_DECLARE(name, element_type, )
#define OBJC_TASK_GROUP_NEXT_DECLARE_THROWING(name, element_type, error_type) \
  __OBJC3_GROUP_NEXT_DECLARE(name, element_type, __objc3_throws(error_type))

/* Close and nonblockingly join an existing group tree. The group is retained
 * by the lazy frame, independently of the caller's handle. Ordinary caller
 * cancellation remains observable; this is an explicit join, not a scope. */
extern void objc3_task_group_join_async(objc3_task_group_t *)
    __objc3_async_frame __asm__("__objc3_task_group_join_frame");
#endif

/* Direct recognized calls preserve one evaluation, capture diagnostics and
 * child/detached metadata at the user's original macro argument site. */
#define OBJC_TASK_SPAWN(block_expr) objc3_task_spawn((block_expr))
#define OBJC_TASK_DETACHED(block_expr) objc3_task_detached((block_expr))
#endif

#endif
