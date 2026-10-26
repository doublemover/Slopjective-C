#include "service_internal.h"
#include "service_autorelease_pool.h"
#include "service_actor.h"
#include "service_group.h"

#include <cstdlib>
#include <cstring>
#include <new>
#include <utility>

extern "C" objc3_service_status
objc3_async_result_type_validate(const objc3_async_result_type *type) {
  if (!type || type->struct_size < sizeof(*type))
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (type->version != OBJC3_ASYNC_ABI_VERSION)
    return OBJC3_SERVICE_UNSUPPORTED_VERSION;
  if (type->reserved || !type->identity || !type->identity_size ||
      type->identity_size > OBJC3_ASYNC_IDENTITY_LIMIT ||
      !type->storage_alignment ||
      (type->storage_alignment & (type->storage_alignment - 1)) ||
      type->storage_size > SIZE_MAX || type->storage_alignment > SIZE_MAX ||
      std::memchr(type->identity, 0, type->identity_size) ||
      type->identity[type->identity_size] != 0)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  constexpr char local_prefix[] = "objc3.async.local.v1:";
  constexpr char external_prefix[] = "objc3.async.v1:";
  const bool local =
      type->identity_size > sizeof(local_prefix) - 1 &&
      std::memcmp(type->identity, local_prefix, sizeof(local_prefix) - 1) == 0;
  const bool external = type->identity_size > sizeof(external_prefix) - 1 &&
                        std::memcmp(type->identity, external_prefix,
                                    sizeof(external_prefix) - 1) == 0;
  if ((!local && !external) || local != (type->identity_domain != nullptr))
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  switch (type->kind) {
  case OBJC3_ASYNC_VOID:
    return type->storage_size == 0 && type->storage_alignment == 1
               ? OBJC3_SERVICE_OK
               : OBJC3_SERVICE_INVALID_ARGUMENT;
  case OBJC3_ASYNC_OBJECT:
  case OBJC3_ASYNC_BLOCK:
    return type->storage_size == sizeof(void *) &&
                   type->storage_alignment == alignof(void *)
               ? OBJC3_SERVICE_OK
               : OBJC3_SERVICE_INVALID_ARGUMENT;
  case OBJC3_ASYNC_VALUE:
    return type->storage_size ? OBJC3_SERVICE_OK
                              : OBJC3_SERVICE_INVALID_ARGUMENT;
  }
  return OBJC3_SERVICE_INVALID_ARGUMENT;
}

extern "C" int
objc3_async_result_type_equal(const objc3_async_result_type *left,
                              const objc3_async_result_type *right) {
  return objc3_async_result_type_validate(left) == OBJC3_SERVICE_OK &&
         objc3_async_result_type_validate(right) == OBJC3_SERVICE_OK &&
         left->kind == right->kind &&
         left->storage_size == right->storage_size &&
         left->storage_alignment == right->storage_alignment &&
         left->identity_domain == right->identity_domain &&
         left->identity_size == right->identity_size &&
         std::memcmp(left->identity, right->identity, left->identity_size) == 0;
}

namespace {

thread_local objc3_task_t *current_service_task = nullptr;

struct CurrentTaskScope {
  objc3_task_t *previous;
  explicit CurrentTaskScope(objc3_task_t *task)
      : previous(std::exchange(current_service_task, task)) {}
  ~CurrentTaskScope() { current_service_task = previous; }
};

struct AllocationHeader { void *base; };

struct TaskReleaseGuard {
  objc3_task_t *task;
  ~TaskReleaseGuard() { objc3_task_internal_release(task); }
};

struct ResumeDispatch {
  objc3_task_t *task;
  bool executed{false};
};

void cancel_task(objc3_task_t *task) noexcept {
  if (objc3::services::task_group_scope_abort(task)) return;
  // Forced loss of a continuation is cancellation of the task's structured
  // work too. Publish descendant flags/wakes before reporting this outcome.
  objc3_task_request_cancel(task);
  {
    std::lock_guard<std::mutex> lock(task->mutex);
    if (task->awaiting && task->await_cancellation_shielded) {
      // A broken callback or lost external owner cannot reclaim a structured
      // scope's native frame before its already-registered cleanup finishes.
      task->cancel_after_cleanup = true;
      return;
    }
  }
  objc3_completion cancelled{};
  cancelled.struct_size = sizeof(cancelled);
  cancelled.version = OBJC3_SERVICE_ABI_VERSION;
  cancelled.kind = OBJC3_COMPLETION_CANCELLED;
  (void)objc3_task_complete(task, &cancelled);
}

void run_dispatch(void *context) {
  auto *dispatch = static_cast<ResumeDispatch *>(context);
  dispatch->executed = true;
  objc3_service_run_task_resume(dispatch->task);
}

void destroy_dispatch(void *context) {
  std::unique_ptr<ResumeDispatch> dispatch(
      static_cast<ResumeDispatch *>(context));
  if (!dispatch->executed) {
    {
      std::lock_guard<std::mutex> lock(dispatch->task->mutex);
      dispatch->task->resume_queued = false;
    }
    // Actor cancellation discards queued jobs. Such a continuation must not
    // leave a suspended task and its native frame unreachable.
    cancel_task(dispatch->task);
  }
  objc3_task_internal_release(dispatch->task);
}

void run_executor_dispatch(void *context) {
  run_dispatch(context);
  destroy_dispatch(context);
}

// The task mutex covers both reservation and publication. These enqueue APIs
// always queue and never invoke user code inline. A destination worker cannot
// enter the task until this lock is released, and another wake cannot overtake
// the publication of a deferred resume. Failed dispatch references are released
// after unlocking, since final release can destroy a frame or call observers.
objc3_service_status queue_resume_locked(objc3_task_t *task,
                                         ResumeDispatch *&rejected) {
  if (task->completion_claimed) {
    task->resume_after_completion_claim = true;
    return OBJC3_SERVICE_OK;
  }
  auto *dispatch = new (std::nothrow) ResumeDispatch{task};
  if (!dispatch) return OBJC3_SERVICE_OUT_OF_MEMORY;
  if (!objc3_task_internal_retain(task)) {
    delete dispatch;
    return OBJC3_SERVICE_NOT_READY;
  }
  task->resume_queued = true;
  objc3_service_status status;
  if (task->actor) {
    objc3_actor_hop_facts facts{};
    facts.struct_size = sizeof(facts);
    facts.version = OBJC3_ACTOR_SERVICE_VERSION;
    status = objc3_actor_enqueue(task->actor, run_dispatch, dispatch,
                                 destroy_dispatch, &facts);
  } else {
    status = objc3_executor_enqueue(task->executor, run_executor_dispatch,
                                     dispatch);
  }
  if (status != OBJC3_SERVICE_OK) {
    task->resume_queued = false;
    rejected = dispatch;
  }
  return status;
}

void release_rejected_dispatch(ResumeDispatch *dispatch) {
  if (!dispatch) return;
  auto *task = dispatch->task;
  delete dispatch;
  objc3_task_internal_release(task);
}

void wake_awaiting_task(objc3_task_t *task) noexcept {
  const auto status = objc3_task_schedule_resume(task);
  if (status == OBJC3_SERVICE_OK || status == OBJC3_SERVICE_ALREADY_COMPLETED)
    return;
  cancel_task(task);
}

void dependency_completed(void *context, const objc3_completion *) {
  std::unique_ptr<ObjC3TaskAwaitContinuation> continuation(
      static_cast<ObjC3TaskAwaitContinuation *>(context));
  objc3_task_finish_await(*continuation);
}

void *allocate_payload(uint64_t size, uint64_t alignment) noexcept {
  if (!alignment || (alignment & (alignment - 1)) != 0 ||
      alignment > static_cast<uint64_t>(SIZE_MAX - sizeof(AllocationHeader)))
    return nullptr;
  const size_t bytes = static_cast<size_t>(size ? size : 1);
  const size_t align = static_cast<size_t>(alignment < alignof(void *)
                                              ? alignof(void *) : alignment);
  if (bytes > SIZE_MAX - sizeof(AllocationHeader) - align) return nullptr;
  void *base = std::malloc(bytes + sizeof(AllocationHeader) + align - 1);
  if (!base) return nullptr;
  uintptr_t start = reinterpret_cast<uintptr_t>(base) + sizeof(AllocationHeader);
  uintptr_t aligned = (start + align - 1) & ~(static_cast<uintptr_t>(align) - 1);
  reinterpret_cast<AllocationHeader *>(aligned)[-1].base = base;
  return reinterpret_cast<void *>(aligned);
}

void free_payload_storage(void *payload) noexcept {
  if (payload) std::free(reinterpret_cast<AllocationHeader *>(payload)[-1].base);
}

objc3_service_status validate_ops(const objc3_value_ops *ops) noexcept {
  if (!ops) return OBJC3_SERVICE_INVALID_ARGUMENT;
  const objc3_service_status status = objc3_value_ops_validate(ops);
  if (status != OBJC3_SERVICE_OK) return status;
  if (ops->struct_size < sizeof(objc3_value_ops) ||
      ops->version != OBJC3_VALUE_OPS_VERSION || !ops->copy_init ||
      !ops->destroy)
    return ops->version != OBJC3_VALUE_OPS_VERSION
               ? OBJC3_SERVICE_UNSUPPORTED_VERSION
               : OBJC3_SERVICE_INVALID_ARGUMENT;
  return OBJC3_SERVICE_OK;
}

objc3_service_status validate_completion(const objc3_completion *value) noexcept {
  if (!value || value->struct_size < sizeof(objc3_completion) ||
      value->version != OBJC3_SERVICE_ABI_VERSION)
    return value && value->version != OBJC3_SERVICE_ABI_VERSION
               ? OBJC3_SERVICE_UNSUPPORTED_VERSION
               : OBJC3_SERVICE_INVALID_ARGUMENT;
  switch (value->kind) {
  case OBJC3_COMPLETION_SUCCESS: {
    if (value->error) return OBJC3_SERVICE_INVALID_ARGUMENT;
    if (value->result_type) {
      const auto status = objc3_async_result_type_validate(value->result_type);
      if (status != OBJC3_SERVICE_OK)
        return status;
      if ((value->result_type->kind == OBJC3_ASYNC_VOID) != !value->has_payload)
        return OBJC3_SERVICE_INVALID_ARGUMENT;
    }
    if (!value->has_payload)
      return value->payload == nullptr && value->payload_ops == nullptr
                 ? OBJC3_SERVICE_OK : OBJC3_SERVICE_INVALID_ARGUMENT;
    if (value->has_payload != 1) return OBJC3_SERVICE_INVALID_ARGUMENT;
    const objc3_service_status ops_status = validate_ops(value->payload_ops);
    if (ops_status != OBJC3_SERVICE_OK) return ops_status;
    if (value->result_type &&
        (value->result_type->storage_size != value->payload_ops->payload_size ||
         value->result_type->storage_alignment !=
             value->payload_ops->payload_alignment))
      return OBJC3_SERVICE_TYPE_MISMATCH;
    return value->payload || value->payload_ops->payload_size == 0
               ? OBJC3_SERVICE_OK : OBJC3_SERVICE_INVALID_ARGUMENT;
  }
  case OBJC3_COMPLETION_ERROR:
    return !value->result_type && !value->has_payload && !value->payload &&
                   !value->payload_ops && value->error
               ? OBJC3_SERVICE_OK
               : OBJC3_SERVICE_INVALID_ARGUMENT;
  case OBJC3_COMPLETION_CANCELLED:
    return !value->result_type && !value->has_payload && !value->payload &&
                   !value->payload_ops && !value->error
               ? OBJC3_SERVICE_OK
               : OBJC3_SERVICE_INVALID_ARGUMENT;
  }
  return OBJC3_SERVICE_INVALID_ARGUMENT;
}

objc3_completion task_view(const objc3_task_t *task) noexcept {
  objc3_completion value{};
  value.struct_size = sizeof(value);
  value.version = OBJC3_SERVICE_ABI_VERSION;
  value.kind = task->kind;
  value.has_payload = task->has_payload ? 1u : 0u;
  value.payload = task->payload;
  value.payload_ops = task->payload_ops_set ? &task->payload_ops : nullptr;
  value.error = task->error;
  value.result_type = task->result_type_set ? &task->result_type : nullptr;
  return value;
}

bool task_quiescent_locked(const objc3_task_t *task) noexcept {
  return task->terminal && task->frame_destroyed && !task->frame_destroying &&
         !task->resume_active && task->callbacks_active == 0 &&
         task->continuation_owners_active == 0;
}

void notify_quiescence(objc3_task_t *task) noexcept {
  std::shared_ptr<ObjC3TaskQuiescenceObserver> observer;
  {
    std::lock_guard<std::mutex> lock(task->mutex);
    if (!task_quiescent_locked(task)) return;
    observer = task->quiescence_observers;
  }
  // These are scheduler notifications, not completion callbacks. Counting them
  // as active completion callbacks would make a notified group temporarily
  // unjoinable on a different worker. The caller retains the task throughout.
  for (; observer; observer = observer->next) {
    CurrentTaskScope current(task);
    try { observer->notify(observer->context.get()); } catch (...) {}
  }
}

void invoke_waiters(objc3_task_t *task,
                    const std::vector<objc3_task::Waiter> &waiters,
                    const objc3_completion &value) noexcept {
  for (const auto &waiter : waiters) {
    {
      CurrentTaskScope current(task);
      try { waiter.callback(waiter.context, &value); } catch (...) {}
    }
    {
      std::lock_guard<std::mutex> lock(task->mutex);
      --task->callbacks_active;
    }
    task->completed.notify_all();
  }
  notify_quiescence(task);
}

void finish_frame_destruction(objc3_task_t *task, void *frame,
                              objc3_frame_destroy destroy) noexcept {
  if (frame) {
    CurrentTaskScope current(task);
    objc3_task_destroy_frame(frame, destroy);
  }
  std::shared_ptr<void> scope_owner;
  {
    std::lock_guard<std::mutex> lock(task->mutex);
    scope_owner = std::move(task->group_scope_owner);
  }
  {
    CurrentTaskScope current(task);
    scope_owner.reset();
  }
  {
    std::lock_guard<std::mutex> lock(task->mutex);
    task->frame_destroying = false;
    task->frame_destroyed = true;
  }
  task->completed.notify_all();
  notify_quiescence(task);
}

void finish_continuation_owner(objc3_task_t *task,
                               std::shared_ptr<void> &owner) noexcept {
  if (!owner) return;
  {
    // A payload destructor may reenter a blocking wait on this very task.
    // Preserve callback identity until all retained cleanup resources are gone.
    CurrentTaskScope current(task);
    owner.reset();
  }
  {
    std::lock_guard<std::mutex> lock(task->mutex);
    --task->continuation_owners_active;
  }
  task->completed.notify_all();
  notify_quiescence(task);
}

void release_payload(objc3_task_t *task) noexcept {
  if (!task->payload) return;
  if (task->payload_ops_set) {
    try { task->payload_ops.destroy(task->payload); } catch (...) {}
  }
  free_payload_storage(task->payload);
  task->payload = nullptr;
}

} // namespace

objc3_task::AwaitTarget::~AwaitTarget() {
  // Releasing the last descendant can reclaim an arbitrarily deep chain of
  // completed ancestor tokens. A nested destructor hands its one strong parent
  // to the outer loop rather than recursively dropping another shared_ptr.
  // The TLS pointer is trivial and owns no state at thread teardown.
  static thread_local std::shared_ptr<AwaitTarget> *deferred_parent = nullptr;
  if (deferred_parent) {
    *deferred_parent = std::move(structured_parent);
    return;
  }
  auto next = std::move(structured_parent);
  std::shared_ptr<AwaitTarget> pending;
  deferred_parent = &pending;
  while (next) {
    next.reset();
    next = std::move(pending);
  }
  deferred_parent = nullptr;
}

objc3_task_t::~objc3_task() {
  if (await_target) {
    std::lock_guard<std::mutex> lock(await_target->mutex);
    await_target->task = nullptr;
  }
  // One task can be registered in many independent groups. Drop the immutable
  // observation chain iteratively: shared_ptr's recursive next destruction
  // would otherwise make final task release consume unbounded native stack.
  while (quiescence_observers) {
    auto observer = std::move(quiescence_observers);
    quiescence_observers = std::move(observer->next);
  }
  // A last handle released while suspended becomes an implicit cancellation.
  // The release path normally terminalizes first; this is a defensive fallback.
  release_payload(this);
  if (error) objc3_error_release(error);
  if (frame && destroy) objc3_task_destroy_frame(frame, destroy);
  if (actor) objc3_actor_release(actor);
  if (executor) objc3_executor_release(executor);
}

void objc3_task_destroy_frame(void *frame, objc3_frame_destroy destroy) {
  if (!frame || !destroy) return;
  try { destroy(frame); } catch (...) {}
}

objc3_task_t *objc3_task_current_callback() noexcept {
  return current_service_task;
}

objc3_service_status objc3_task_wait_quiescent(objc3_task_t *task) {
  if (!task) return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (current_service_task == task) return OBJC3_SERVICE_NOT_READY;
  if (!objc3_task_internal_retain(task)) return OBJC3_SERVICE_NOT_READY;
  std::unique_lock<std::mutex> lock(task->mutex);
  auto quiescent = [task] {
    return task_quiescent_locked(task);
  };
  // A task can hop to any executor after this check. Never block an executor
  // callback on pending task work, even when its current destination differs.
  if (!quiescent() &&
      (objc3::services::executor_current_identity() != 0 ||
       (task->executor && task->executor->manual))) {
    lock.unlock();
    objc3_task_internal_release(task);
    return OBJC3_SERVICE_NOT_READY;
  }
  task->completed.wait(lock, quiescent);
  lock.unlock();
  objc3_task_internal_release(task);
  return OBJC3_SERVICE_OK;
}

bool objc3_task_internal_retain(objc3_task_t *task) {
  return task && objc3::services::try_retain(task->references);
}

namespace {
void release_task_reference(objc3_task_t *task, bool check_cleanup) {
  bool last, only_cleanup = false;
  {
    std::lock_guard<std::mutex> lock(task->reference_mutex);
    const auto previous = task->references.fetch_sub(1, std::memory_order_acq_rel);
    last = previous == 1;
    if (!last && check_cleanup && task->cleanup_references &&
        previous - 1 == task->cleanup_references) {
      // Own a temporary reference before unlocking: a notification can retire
      // the last cleanup lease concurrently. This hold is released below
      // without repeating the same orphan check.
      task->references.fetch_add(1, std::memory_order_relaxed);
      only_cleanup = true;
    }
  }
  if (only_cleanup) {
    // An explicit cleanup await without a compiler scope keeps its established
    // lease semantics. Compiler scopes instead cancel their unfinished work;
    // an earlier shielded dependency still finishes before their own cleanup.
    (void)objc3::services::task_group_scope_abort(task);
    release_task_reference(task, false);
  } else if (last) {
    bool needs_cancel;
    {
      std::lock_guard<std::mutex> lock(task->mutex);
      needs_cancel = !task->terminal;
    }
    if (needs_cancel) {
      // Keep the record alive across cancellation callbacks and their reentry.
      task->references.store(1, std::memory_order_release);
      cancel_task(task);
      release_task_reference(task, true);
    } else {
      delete task;
    }
  }
}
} // namespace

void objc3_task_internal_release(objc3_task_t *task) {
  release_task_reference(task, true);
}

bool objc3_task_cleanup_retain(objc3_task_t *task) {
  if (!task) return false;
  std::lock_guard<std::mutex> lock(task->reference_mutex);
  if (!objc3_task_internal_retain(task)) return false;
  ++task->cleanup_references;
  return true;
}

void objc3_task_cleanup_release(objc3_task_t *task) {
  if (!task) return;
  {
    std::lock_guard<std::mutex> lock(task->reference_mutex);
    --task->cleanup_references;
  }
  release_task_reference(task, true);
}

namespace {
objc3_service_status start_task(
    const objc3_task_start_options *options, objc3_task_t **out_task,
    objc3_task_t *parent = nullptr,
    objc3::services::TaskGroup *group = nullptr) {
  if (!out_task) return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_task = nullptr;
  if (!options || options->struct_size < sizeof(objc3_task_start_options) ||
      options->version != OBJC3_SERVICE_ABI_VERSION ||
      (bool(options->executor) == bool(options->actor)) ||
      !options->resume || (options->frame && !options->destroy))
    return options && options->version != OBJC3_SERVICE_ABI_VERSION
               ? OBJC3_SERVICE_UNSUPPORTED_VERSION
               : OBJC3_SERVICE_INVALID_ARGUMENT;
  auto *task = new (std::nothrow) objc3_task;
  if (!task) return OBJC3_SERVICE_OUT_OF_MEMORY;
  try {
    task->await_target = std::make_shared<objc3_task::AwaitTarget>(task);
  } catch (...) {
    delete task;
    return OBJC3_SERVICE_OUT_OF_MEMORY;
  }
  if (options->result_type) {
    const auto status = objc3_async_result_type_validate(options->result_type);
    if (status != OBJC3_SERVICE_OK) {
      delete task;
      return status;
    }
    try {
      task->declared_result_identity.assign(
          options->result_type->identity, options->result_type->identity_size);
    } catch (...) {
      delete task;
      return OBJC3_SERVICE_OUT_OF_MEMORY;
    }
    task->declared_result_type = *options->result_type;
    task->declared_result_type.identity =
        task->declared_result_identity.c_str();
    task->declared_result_type_set = true;
  }
  if (options->actor)
    task->actor = objc3_actor_retain(options->actor);
  auto *executor = options->actor
      ? objc3::services::actor_executor(
            reinterpret_cast<objc3::services::Actor *>(options->actor))
      : options->executor;
  task->executor = objc3_executor_retain(executor);
  if (!task->executor || (options->actor && !task->actor)) {
    task->frame = nullptr;
    delete task;
    return OBJC3_SERVICE_NOT_READY;
  }
  task->frame = options->frame;
  task->frame_destroyed = options->frame == nullptr;
  task->resume = options->resume;
  task->destroy = options->destroy;
  if (options->completion) {
    try {
      task->waiters.push_back({options->completion, options->completion_context});
    } catch (...) {
      task->frame = nullptr; // Ownership transfers only after start succeeds.
      delete task;
      return OBJC3_SERVICE_OUT_OF_MEMORY;
    }
  }
  if (parent) {
    try {
      // Register before the first resume can be published. Cancellation and
      // registration use the same token lock: the child is either traversed
      // by an ongoing request or inherits the already-published request.
      std::lock_guard<std::mutex> lock(parent->await_target->mutex);
      auto &children = parent->await_target->structured_children;
      // After cancellation, wake traversal uses stable vector positions while
      // temporarily dropping the token lock around callback-capable dispatch.
      if (!parent->await_target->cancel_requested) {
        size_t kept = 0;
        for (const auto &weak_child : children)
          if (auto child = weak_child.lock()) {
            child->structured_index = kept;
            children[kept++] = child;
          }
        children.resize(kept);
      }
      task->await_target->structured_index = children.size();
      children.push_back(task->await_target);
      task->await_target->structured_parent = parent->await_target;
      task->await_target->cancel_requested =
          parent->await_target->cancel_requested;
      task->cancel_requested.store(task->await_target->cancel_requested,
                                    std::memory_order_release);
    } catch (...) {
      task->frame = nullptr;
      delete task;
      return OBJC3_SERVICE_OUT_OF_MEMORY;
    }
  }
  if (group) {
    const auto registered = objc3::services::task_group_add_task(group, task);
    if (registered != OBJC3_SERVICE_OK) {
      // The caller still owns the unstarted frame. No body can observe its
      // borrowed captures before group registration succeeds.
      {
        std::lock_guard<std::mutex> lock(task->mutex);
        task->frame = nullptr;
        task->frame_destroyed = true;
        task->terminal = true;
      }
      objc3_task_internal_release(task);
      return registered;
    }
  }
  const objc3_service_status queued = objc3_task_schedule_resume(task);
  if (queued != OBJC3_SERVICE_OK) {
    {
      std::lock_guard<std::mutex> lock(task->mutex);
      task->frame = nullptr;
      task->frame_destroyed = true;
      if (!group) task->terminal = true;
    }
    if (group) {
      // Registration is already visible. Publish a cancelled, frame-less
      // member so joins remain quiescent even when queue publication fails.
      // Frame disposal stays with the caller, as on every failed start.
      objc3_completion cancelled{};
      cancelled.struct_size = sizeof(cancelled);
      cancelled.version = OBJC3_SERVICE_ABI_VERSION;
      cancelled.kind = OBJC3_COMPLETION_CANCELLED;
      (void)objc3_task_complete(task, &cancelled);
    }
    objc3_task_internal_release(task);
    return queued;
  }
  *out_task = task;
  return OBJC3_SERVICE_OK;
}
} // namespace

extern "C" objc3_service_status objc3_task_start(
    const objc3_task_start_options *options, objc3_task_t **out_task) {
  return start_task(options, out_task);
}

namespace {
objc3_service_status start_child(
    objc3_task_t *parent, const objc3_task_start_options *options,
    objc3_task_t **out_task, bool explicit_destination,
    objc3::services::TaskGroup *group = nullptr) {
  if (!out_task) return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_task = nullptr;
  if (!parent || current_service_task != parent || !options ||
      options->struct_size < sizeof(*options))
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (options->version != OBJC3_SERVICE_ABI_VERSION)
    return OBJC3_SERVICE_UNSUPPORTED_VERSION;
  if (explicit_destination
          ? bool(options->executor) == bool(options->actor)
          : options->executor || options->actor)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  auto inherited = *options;
  {
    std::lock_guard<std::mutex> lock(parent->mutex);
    if (parent->terminal || parent->completion_claimed)
      return OBJC3_SERVICE_ALREADY_COMPLETED;
    if (!parent->resume_active || parent->awaiting || parent->resume_requested)
      return OBJC3_SERVICE_INVALID_ARGUMENT;
    if (!explicit_destination) {
      if (parent->actor) inherited.actor = parent->actor;
      else inherited.executor = parent->executor;
    }
  }
  return start_task(&inherited, out_task, parent, group);
}
} // namespace

extern "C" objc3_service_status objc3_task_start_child(
    objc3_task_t *parent, const objc3_task_start_options *options,
    objc3_task_t **out_task) {
  return start_child(parent, options, out_task, false);
}

extern "C" objc3_service_status objc3_task_start_child_on(
    objc3_task_t *parent, const objc3_task_start_options *options,
    objc3_task_t **out_task) {
  return start_child(parent, options, out_task, true);
}

extern "C" objc3_task_t *objc3_task_retain(objc3_task_t *task) {
  return objc3_task_internal_retain(task) ? task : nullptr;
}

extern "C" void objc3_task_release(objc3_task_t *task) {
  if (task) objc3_task_internal_release(task);
}

extern "C" void objc3_task_request_cancel(objc3_task_t *task) {
  if (!task || !objc3_task_internal_retain(task)) return;
  TaskReleaseGuard guard{task};
  if (objc3_task_mark_cancel(task)) objc3_task_wake_cancelled(task);
}

namespace {
using CancellationTarget = std::shared_ptr<objc3_task::AwaitTarget>;

CancellationTarget next_cancellation_target(CancellationTarget current,
                                            const CancellationTarget &root) {
  {
    std::lock_guard<std::mutex> lock(current->mutex);
    for (const auto &weak_child : current->structured_children)
      if (auto child = weak_child.lock()) return child;
  }
  while (current != root) {
    auto parent = current->structured_parent;
    {
      std::lock_guard<std::mutex> lock(parent->mutex);
      // Cancellation freezes existing child positions. New registrations append
      // and inherit cancellation before their first queue publication.
      for (size_t index = current->structured_index + 1;
           index < parent->structured_children.size(); ++index)
        if (auto sibling = parent->structured_children[index].lock())
          return sibling;
    }
    current = std::move(parent);
  }
  return {};
}

bool mark_cancel_tree(const CancellationTarget &root) {
  bool wake = false;
  for (auto target = root; target; target = next_cancellation_target(target, root)) {
    std::lock_guard<std::mutex> lifetime(target->mutex);
    target->cancel_requested = true;
    if (auto *task = target->task) {
      // The token protects the raw record through its destruction boundary.
      // No retained task is released under a group/publication lock, so marking
      // cannot invoke completion observers or native frame cleanup.
      task->cancel_requested.store(true, std::memory_order_release);
      std::lock_guard<std::mutex> lock(task->mutex);
      if (!task->terminal && task->awaiting && !task->await_cancellation_shielded) {
        if (task->await_status) *task->await_status = OBJC3_SERVICE_CANCELLED;
        task->await_status = nullptr;
        task->await_result_task = nullptr;
        task->awaiting = false;
        task->await_cancellation_shielded = false;
        task->cancel_wake_pending = true;
        wake = true;
      }
    }
  }
  return wake;
}

void wake_cancelled_tree(const CancellationTarget &root) {
  for (auto target = root; target; target = next_cancellation_target(target, root)) {
    objc3_task_t *task = nullptr;
    {
      std::lock_guard<std::mutex> lifetime(target->mutex);
      if (objc3_task_internal_retain(target->task)) task = target->task;
    }
    if (task) {
      TaskReleaseGuard guard{task};
      bool wake;
      {
        std::lock_guard<std::mutex> lock(task->mutex);
        wake = std::exchange(task->cancel_wake_pending, false) && !task->terminal;
      }
      // No token or group lock spans dispatch, which may synchronously invoke
      // completion or frame cleanup if admission fails.
      if (wake) wake_awaiting_task(task);
    }
  }
}
} // namespace

bool objc3_task_mark_cancel(objc3_task_t *task) {
  return mark_cancel_tree(task->await_target);
}

void objc3_task_wake_cancelled(objc3_task_t *task) {
  wake_cancelled_tree(task->await_target);
}

extern "C" int objc3_task_is_cancel_requested(const objc3_task_t *task) {
  return task && task->cancel_requested.load(std::memory_order_acquire) ? 1 : 0;
}

extern "C" objc3_service_status objc3_task_schedule_resume(objc3_task_t *task) {
  if (!task) return OBJC3_SERVICE_INVALID_ARGUMENT;
  ResumeDispatch *rejected = nullptr;
  objc3_service_status status;
  {
    std::lock_guard<std::mutex> lock(task->mutex);
    if (task->terminal)
      return OBJC3_SERVICE_ALREADY_COMPLETED;
    if (task->awaiting && task->await_cancellation_shielded)
      return OBJC3_SERVICE_NOT_READY;
    if (task->completion_claimed) {
      // A user payload copy can fail. Preserve the wake until that claim
      // resolves instead of treating an in-progress copy as terminal success.
      task->resume_after_completion_claim = true;
      return OBJC3_SERVICE_OK;
    }
    if (task->resume_queued) return OBJC3_SERVICE_OK;
    if (task->resume_active) {
      // Defer publication until resume returns. A different destination may
      // execute immediately, so relying on executor seriality loses wakes or
      // overlaps native frame access during a hop.
      task->resume_requested = true;
      return OBJC3_SERVICE_OK;
    }
    status = queue_resume_locked(task, rejected);
  }
  release_rejected_dispatch(rejected);
  return status;
}

extern "C" objc3_service_status objc3_task_hop(
    objc3_task_t *task, objc3_executor_t *executor, objc3_actor_t *actor) {
  if (!task || current_service_task != task ||
      (bool(executor) == bool(actor)))
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  objc3_actor_t *retained_actor = actor ? objc3_actor_retain(actor) : nullptr;
  auto *target = actor ? objc3::services::actor_executor(
                            reinterpret_cast<objc3::services::Actor *>(actor))
                      : executor;
  auto *retained_executor = objc3_executor_retain(target);
  if (!retained_executor || (actor && !retained_actor)) {
    objc3_actor_release(retained_actor);
    objc3_executor_release(retained_executor);
    return OBJC3_SERVICE_NOT_READY;
  }
  objc3_service_status status = OBJC3_SERVICE_OK;
  {
    std::lock_guard<std::mutex> lock(task->mutex);
    if (task->terminal || task->completion_claimed)
      status = OBJC3_SERVICE_ALREADY_COMPLETED;
    else if (!task->resume_active || task->awaiting || task->resume_requested)
      status = OBJC3_SERVICE_INVALID_ARGUMENT;
    else {
      std::swap(task->actor, retained_actor);
      std::swap(task->executor, retained_executor);
      task->resume_requested = true;
    }
  }
  objc3_actor_release(retained_actor);
  objc3_executor_release(retained_executor);
  return status;
}

extern "C" objc3_service_status objc3_task_complete(
    objc3_task_t *task, const objc3_completion *completion) {
  if (!task) return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (!objc3_task_internal_retain(task)) return OBJC3_SERVICE_NOT_READY;
  TaskReleaseGuard task_guard{task};
  const objc3_service_status validation = validate_completion(completion);
  if (validation != OBJC3_SERVICE_OK) return validation;
  if (completion->kind == OBJC3_COMPLETION_SUCCESS &&
      task->declared_result_type_set &&
      !objc3_async_result_type_equal(&task->declared_result_type,
                                     completion->result_type))
    return OBJC3_SERVICE_TYPE_MISMATCH;
  {
    std::lock_guard<std::mutex> lock(task->mutex);
    if (task->terminal || task->completion_claimed)
      return OBJC3_SERVICE_ALREADY_COMPLETED;
    if (task->group_scope ||
        (task->group_scope_forced && completion->kind != OBJC3_COMPLETION_CANCELLED) ||
        (task->awaiting && task->await_cancellation_shielded))
      return OBJC3_SERVICE_NOT_READY;
    task->completion_claimed = true;
  }

  void *copied_payload = nullptr;
  objc3_value_ops copied_ops{};
  objc3_error_t *retained_error = nullptr;
  objc3_service_status copy_status = OBJC3_SERVICE_OK;
  objc3_async_result_type copied_type{};
  std::string copied_identity;
  if (completion->result_type) {
    copied_type = *completion->result_type;
    try {
      copied_identity.assign(copied_type.identity, copied_type.identity_size);
    } catch (...) {
      copy_status = OBJC3_SERVICE_OUT_OF_MEMORY;
    }
  }
  if (copy_status == OBJC3_SERVICE_OK &&
      completion->kind == OBJC3_COMPLETION_SUCCESS && completion->has_payload) {
    copied_ops = *completion->payload_ops;
    copied_payload = allocate_payload(copied_ops.payload_size,
                                      copied_ops.payload_alignment);
    if (!copied_payload) copy_status = OBJC3_SERVICE_OUT_OF_MEMORY;
    else {
      try { copy_status = copied_ops.copy_init(copied_payload, completion->payload); }
      catch (...) { copy_status = OBJC3_SERVICE_INVALID_ARGUMENT; }
    }
  } else if (completion->kind == OBJC3_COMPLETION_ERROR) {
    retained_error = objc3_error_retain(completion->error);
    if (!retained_error) copy_status = OBJC3_SERVICE_INVALID_ARGUMENT;
  }
  if (copy_status != OBJC3_SERVICE_OK) {
    free_payload_storage(copied_payload);
    if (retained_error) objc3_error_release(retained_error);
    bool resume = false;
    {
      std::lock_guard<std::mutex> lock(task->mutex);
      task->completion_claimed = false;
      resume = std::exchange(task->resume_after_completion_claim, false);
    }
    if (resume) wake_awaiting_task(task);
    return copy_status;
  }

  std::vector<objc3_task::Waiter> waiters;
  void *frame_to_destroy = nullptr;
  objc3_frame_destroy frame_destructor = nullptr;
  objc3_completion borrowed{};
  std::shared_ptr<void> continuation_owner;
  {
    std::lock_guard<std::mutex> lock(task->mutex);
    task->kind = completion->kind;
    task->has_payload = completion->kind == OBJC3_COMPLETION_SUCCESS &&
                        completion->has_payload != 0;
    task->payload = copied_payload;
    task->payload_ops = copied_ops;
    task->payload_ops_set = task->has_payload;
    task->result_type_set = completion->result_type != nullptr;
    task->result_type = copied_type;
    task->result_identity = std::move(copied_identity);
    task->result_type.identity = task->result_identity.c_str();
    task->error = retained_error;
    task->terminal = true;
    task->awaiting = false;
    task->await_cancellation_shielded = false;
    task->cancel_after_cleanup = false;
    task->await_status = nullptr;
    task->await_result_task = nullptr;
    task->completion_claimed = false;
    task->resume_after_completion_claim = false;
    task->resume_requested = false;
    continuation_owner = std::move(task->continuation_owner);
    if (continuation_owner) ++task->continuation_owners_active;
    waiters.swap(task->waiters);
    task->callbacks_active += static_cast<uint32_t>(waiters.size());
    if (!task->resume_active && task->frame) {
      frame_to_destroy = std::exchange(task->frame, nullptr);
      frame_destructor = task->destroy;
      task->frame_destroying = true;
    }
    borrowed = task_view(task);
  }
  task->completed.notify_all();
  invoke_waiters(task, waiters, borrowed);
  if (frame_to_destroy)
    finish_frame_destruction(task, frame_to_destroy, frame_destructor);
  else
    task->completed.notify_all();
  finish_continuation_owner(task, continuation_owner);
  return OBJC3_SERVICE_OK;
}

extern "C" objc3_service_status objc3_task_add_waiter(
    objc3_task_t *task, objc3_completion_callback callback, void *context) {
  if (!task || !callback) return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (!objc3_task_internal_retain(task)) return OBJC3_SERVICE_NOT_READY;
  TaskReleaseGuard task_guard{task};
  objc3_completion borrowed{};
  bool call_now = false;
  {
    std::lock_guard<std::mutex> lock(task->mutex);
    if (task->terminal) {
      borrowed = task_view(task);
      ++task->callbacks_active;
      call_now = true;
    } else {
      try { task->waiters.push_back({callback, context}); }
      catch (...) { return OBJC3_SERVICE_OUT_OF_MEMORY; }
    }
  }
  if (call_now) {
    {
      CurrentTaskScope current(task);
      try { callback(context, &borrowed); } catch (...) {}
    }
    {
      std::lock_guard<std::mutex> lock(task->mutex);
      --task->callbacks_active;
    }
    task->completed.notify_all();
    notify_quiescence(task);
  }
  return OBJC3_SERVICE_OK;
}

objc3_service_status objc3_task_add_group_observers(
    objc3_task_t *task, objc3_completion_callback completion, void *context,
    std::shared_ptr<ObjC3TaskQuiescenceObserver> quiescence) {
  if (!task || !completion || !quiescence || !quiescence->notify)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (!objc3_task_internal_retain(task)) return OBJC3_SERVICE_NOT_READY;
  TaskReleaseGuard guard{task};
  objc3_completion borrowed{};
  bool call_now = false;
  try {
    std::lock_guard<std::mutex> lock(task->mutex);
    // Allocate the completion slot before publishing either observer. A
    // completed task cannot consume context and then fail registration.
    if (!task->terminal)
      task->waiters.reserve(task->waiters.size() + 1);
    quiescence->next = task->quiescence_observers;
    task->quiescence_observers = std::move(quiescence);
    if (!task->terminal) {
      task->waiters.push_back({completion, context});
    } else {
      borrowed = task_view(task);
      ++task->callbacks_active;
      call_now = true;
    }
  } catch (...) {
    return OBJC3_SERVICE_OUT_OF_MEMORY;
  }
  if (call_now) {
    {
      CurrentTaskScope current(task);
      try { completion(context, &borrowed); } catch (...) {}
    }
    {
      std::lock_guard<std::mutex> lock(task->mutex);
      --task->callbacks_active;
    }
    task->completed.notify_all();
    notify_quiescence(task);
  }
  return OBJC3_SERVICE_OK;
}

objc3_service_status objc3_task_prepare_await(
    objc3_task_t *parent, ObjC3TaskAwaitContinuation &continuation,
    objc3_service_status *status, objc3_task_t **result_task,
    bool cancellation_shielded) {
  if (!parent || current_service_task != parent)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  try {
    std::lock_guard<std::mutex> lock(parent->mutex);
    if (parent->terminal || parent->completion_claimed)
      return OBJC3_SERVICE_ALREADY_COMPLETED;
    if (!parent->resume_active || parent->awaiting || parent->resume_requested)
      return OBJC3_SERVICE_INVALID_ARGUMENT;
    // Reserve the last generation for a compiler scope's allocation-free
    // finalizer. Body awaits can never steal or reuse its continuation token.
    if (parent->await_generation == UINT64_MAX ||
        (parent->group_scope && parent->await_generation == UINT64_MAX - 1))
      return OBJC3_SERVICE_NOT_READY;
    if (!parent->await_target)
      parent->await_target = std::make_shared<objc3_task::AwaitTarget>(parent);
    continuation = {parent->await_target, ++parent->await_generation};
    parent->await_status = status;
    parent->await_result_task = result_task;
    parent->await_cancellation_shielded = cancellation_shielded;
    parent->awaiting = true;
  } catch (...) {
    return OBJC3_SERVICE_OUT_OF_MEMORY;
  }
  return OBJC3_SERVICE_OK;
}

ObjC3TaskAwaitClaim objc3_task_claim_await(
    const ObjC3TaskAwaitContinuation &continuation, objc3_service_status status,
    objc3_task_t *owned_result, std::shared_ptr<void> continuation_owner) {
  ObjC3TaskAwaitClaim claim;
  objc3_task_t *parent = nullptr;
  {
    // The token does not retain the parent. Its lock protects the raw pointer
    // against final deletion; try_retain rejects a record already at zero.
    std::lock_guard<std::mutex> lock(continuation.target->mutex);
    parent = continuation.target->task;
    if (!objc3_task_internal_retain(parent)) return claim;
  }
  claim.parent = parent;
  {
    std::lock_guard<std::mutex> lock(parent->mutex);
    if (!parent->terminal && parent->awaiting &&
        parent->await_generation == continuation.generation) {
      if (!parent->await_cancellation_shielded &&
          parent->cancel_requested.load(std::memory_order_acquire))
        status = OBJC3_SERVICE_CANCELLED;
      if (parent->await_status) *parent->await_status = status;
      if (status == OBJC3_SERVICE_OK && owned_result && parent->await_result_task) {
        *parent->await_result_task = owned_result;
        claim.transferred_result = true;
      }
      parent->await_status = nullptr;
      parent->await_result_task = nullptr;
      parent->awaiting = false;
      parent->await_cancellation_shielded = false;
      claim.cancel_after_cleanup = std::exchange(parent->cancel_after_cleanup, false);
      parent->continuation_owner = std::move(continuation_owner);
      claim.wake = true;
    }
  }
  return claim;
}

void objc3_task_dispatch_await(ObjC3TaskAwaitClaim claim) {
  if (!claim.parent) return;
  TaskReleaseGuard guard{claim.parent};
  if (claim.wake) {
    if (claim.cancel_after_cleanup) cancel_task(claim.parent);
    else wake_awaiting_task(claim.parent);
  }
}

void objc3_task_finish_await(const ObjC3TaskAwaitContinuation &continuation,
                            objc3_service_status status,
                            std::shared_ptr<void> continuation_owner) {
  objc3_task_dispatch_await(objc3_task_claim_await(
      continuation, status, nullptr, std::move(continuation_owner)));
}

void objc3_task_abort_await(
    objc3_task_t *parent, const ObjC3TaskAwaitContinuation &continuation) {
  std::lock_guard<std::mutex> lock(parent->mutex);
  if (parent->await_generation == continuation.generation) {
    parent->awaiting = false;
    parent->await_cancellation_shielded = false;
    parent->await_status = nullptr;
    parent->await_result_task = nullptr;
  }
}

extern "C" objc3_service_status objc3_task_await(
    objc3_task_t *parent, objc3_task_t *dependency) {
  if (!parent || !dependency || parent == dependency)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  auto continuation = std::unique_ptr<ObjC3TaskAwaitContinuation>(
      new (std::nothrow) ObjC3TaskAwaitContinuation);
  if (!continuation) return OBJC3_SERVICE_OUT_OF_MEMORY;
  const auto preparation = objc3_task_prepare_await(parent, *continuation);
  if (preparation != OBJC3_SERVICE_OK) return preparation;
  if (objc3_task_is_cancel_requested(parent)) {
    // Queue the cancellation observation after the active resume returns.
    objc3_task_finish_await(*continuation);
    return OBJC3_SERVICE_OK;
  }
  // A completed dependency can call back before add_waiter returns. Transfer
  // the independent token first and never touch it on successful registration.
  auto *context = continuation.release();
  const auto status = objc3_task_add_waiter(dependency, dependency_completed,
                                           context);
  if (status != OBJC3_SERVICE_OK) {
    std::unique_ptr<ObjC3TaskAwaitContinuation> failed(context);
    objc3_task_abort_await(parent, *failed);
  }
  return status;
}

extern "C" objc3_service_status objc3_task_get_completion(
    objc3_task_t *task, objc3_completion *out_completion) {
  if (!task || !out_completion) return OBJC3_SERVICE_INVALID_ARGUMENT;
  std::lock_guard<std::mutex> lock(task->mutex);
  if (!task->terminal) return OBJC3_SERVICE_NOT_READY;
  *out_completion = task_view(task);
  return OBJC3_SERVICE_OK;
}

extern "C" objc3_service_status objc3_task_wait(objc3_task_t *task) {
  if (!task) return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (!objc3_task_internal_retain(task)) return OBJC3_SERVICE_NOT_READY;
  std::unique_lock<std::mutex> lock(task->mutex);
  if (!task->terminal && (current_service_task == task ||
      objc3::services::executor_current_identity() != 0 ||
      (task->executor && task->executor->manual))) {
    lock.unlock();
    objc3_task_internal_release(task);
    return OBJC3_SERVICE_NOT_READY;
  }
  task->completed.wait(lock, [task] { return task->terminal; });
  lock.unlock();
  objc3_task_internal_release(task);
  return OBJC3_SERVICE_OK;
}

extern "C" objc3_service_status
objc3_task_get_result_type(objc3_task_t *task,
                           const objc3_async_result_type **out_type) {
  if (!task || !out_type)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  std::lock_guard<std::mutex> lock(task->mutex);
  if (task->declared_result_type_set)
    *out_type = &task->declared_result_type;
  else if (task->result_type_set)
    *out_type = &task->result_type;
  else
    return OBJC3_SERVICE_NOT_READY;
  return OBJC3_SERVICE_OK;
}

extern "C" objc3_service_status
objc3_task_copy_result(objc3_task_t *task,
                       const objc3_async_result_type *expected,
                       const objc3_value_ops *ops, void *destination) {
  if (!task)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  const auto validation = objc3_async_result_type_validate(expected);
  if (validation != OBJC3_SERVICE_OK)
    return validation;
  if (expected->kind == OBJC3_ASYNC_VOID) {
    if (ops || destination)
      return OBJC3_SERVICE_INVALID_ARGUMENT;
  } else {
    const auto status = objc3_value_ops_validate(ops);
    if (status != OBJC3_SERVICE_OK)
      return status;
    if (!destination || (reinterpret_cast<uintptr_t>(destination) &
                         (expected->storage_alignment - 1)))
      return OBJC3_SERVICE_INVALID_ARGUMENT;
    if (ops->payload_size != expected->storage_size ||
        ops->payload_alignment != expected->storage_alignment)
      return OBJC3_SERVICE_TYPE_MISMATCH;
  }
  if (!objc3_task_internal_retain(task))
    return OBJC3_SERVICE_NOT_READY;
  TaskReleaseGuard guard{task};
  objc3_completion completion{};
  const auto status = objc3_task_get_completion(task, &completion);
  if (status != OBJC3_SERVICE_OK)
    return status;
  if (completion.kind == OBJC3_COMPLETION_CANCELLED)
    return OBJC3_SERVICE_CANCELLED;
  if (completion.kind == OBJC3_COMPLETION_ERROR)
    return OBJC3_SERVICE_FAILED;
  if (!objc3_async_result_type_equal(completion.result_type, expected))
    return OBJC3_SERVICE_TYPE_MISMATCH;
  if (expected->kind == OBJC3_ASYNC_VOID)
    return OBJC3_SERVICE_OK;
  try {
    return ops->copy_init(destination, completion.payload);
  } catch (...) {
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  }
}

extern "C" objc3_service_status
objc3_async_frame_validate(const objc3_async_frame *frame) {
  if (!frame || frame->struct_size < sizeof(*frame))
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (frame->version != OBJC3_ASYNC_ABI_VERSION)
    return OBJC3_SERVICE_UNSUPPORTED_VERSION;
  if (!frame->context || !frame->resume || !frame->destroy)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  return objc3_async_result_type_validate(frame->result_type);
}

extern "C" objc3_service_status
objc3_async_frame_dispose(objc3_async_frame *frame) {
  if (!frame)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (!frame->struct_size && !frame->version && !frame->context &&
      !frame->resume && !frame->destroy && !frame->result_type)
    return OBJC3_SERVICE_OK;
  const auto status = objc3_async_frame_validate(frame);
  if (status != OBJC3_SERVICE_OK)
    return status;
  const auto owned = *frame;
  *frame = {};
  objc3_task_destroy_frame(owned.context, owned.destroy);
  return OBJC3_SERVICE_OK;
}

namespace {
objc3_service_status start_async_frame(objc3_async_frame *frame,
                                       objc3_executor_t *executor,
                                       objc3_actor_t *actor,
                                       objc3_task_t *parent,
                                       objc3_task_t **out_task,
                                       objc3::services::TaskGroup *group = nullptr) {
  if (!out_task)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_task = nullptr;
  const auto status = objc3_async_frame_validate(frame);
  if (status != OBJC3_SERVICE_OK)
    return status;
  objc3_task_start_options options{};
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = executor;
  options.actor = actor;
  options.frame = frame->context;
  options.resume = frame->resume;
  options.destroy = frame->destroy;
  options.result_type = frame->result_type;
  const auto started = parent
      ? start_child(parent, &options, out_task, executor || actor, group)
      : objc3_task_start(&options, out_task);
  if (started == OBJC3_SERVICE_OK)
    *frame = {};
  return started;
}
} // namespace

extern "C" objc3_service_status
objc3_async_frame_start(objc3_async_frame *frame, objc3_executor_t *executor,
                        objc3_actor_t *actor, objc3_task_t **out_task) {
  return start_async_frame(frame, executor, actor, nullptr, out_task);
}

extern "C" objc3_service_status
objc3_async_frame_start_child(objc3_async_frame *frame, objc3_task_t *parent,
                              objc3_task_t **out_task) {
  if (!parent) {
    if (out_task)
      *out_task = nullptr;
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  }
  return start_async_frame(frame, nullptr, nullptr, parent, out_task);
}

extern "C" objc3_service_status objc3_async_frame_spawn_in_group(
    objc3_async_frame *frame, objc3_task_group_t *group, objc3_task_t **out_task) {
  if (!out_task) return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_task = nullptr;
  if (!group) return OBJC3_SERVICE_INVALID_ARGUMENT;
  const auto status = objc3_async_frame_validate(frame);
  if (status != OBJC3_SERVICE_OK) return status;
  if (!current_service_task) return OBJC3_SERVICE_NOT_READY;
  return start_async_frame(frame, nullptr, nullptr, current_service_task,
      out_task, reinterpret_cast<objc3::services::TaskGroup *>(group));
}

extern "C" objc3_service_status objc3_async_frame_start_child_on(
    objc3_async_frame *frame, objc3_task_t *parent, objc3_executor_t *executor,
    objc3_actor_t *actor, objc3_task_t **out_task) {
  if (!parent || bool(executor) == bool(actor)) {
    if (out_task) *out_task = nullptr;
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  }
  return start_async_frame(frame, executor, actor, parent, out_task);
}

extern "C" objc3_service_status objc3_async_frame_spawn(
    objc3_async_frame *frame, objc3_task_t **out_task) {
  if (!out_task) return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_task = nullptr;
  const auto status = objc3_async_frame_validate(frame);
  if (status != OBJC3_SERVICE_OK) return status;
  auto *parent = objc3_task_current_callback();
  if (!parent) return OBJC3_SERVICE_NOT_READY;
  return objc3_async_frame_start_child(frame, parent, out_task);
}

extern "C" objc3_service_status objc3_async_frame_detach(
    objc3_async_frame *frame, objc3_task_t **out_task) {
  if (!out_task) return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_task = nullptr;
  const auto status = objc3_async_frame_validate(frame);
  if (status != OBJC3_SERVICE_OK) return status;
  objc3_executor_t *global = nullptr;
  const auto destination = objc3_executor_get_global(&global);
  if (destination != OBJC3_SERVICE_OK) return destination;
  const auto started = objc3_async_frame_start(frame, global, nullptr, out_task);
  objc3_executor_release(global);
  return started;
}

void objc3_service_run_task_resume(void *context) {
  auto *task = static_cast<objc3_task_t *>(context);
  void *frame = nullptr;
  objc3_task_resume resume = nullptr;
  bool resume_failed = false;
  std::shared_ptr<void> continuation_owner;
  {
    std::lock_guard<std::mutex> lock(task->mutex);
    task->resume_queued = false;
    if (!task->terminal && !task->resume_active &&
        !(task->group_scope && task->group_scope_forced)) {
      task->resume_active = true;
      continuation_owner = std::move(task->continuation_owner);
      if (continuation_owner) ++task->continuation_owners_active;
      frame = task->frame;
      resume = task->resume;
    }
  }
  if (resume) {
    CurrentTaskScope current(task);
    // Drain temporaries before publishing a continuation, including a hop to
    // another worker that can resume immediately. The destination's outer
    // callback pool alone would drain too late for that handoff.
    objc3::services::CallbackAutoreleasePool pool;
    try { resume(task, frame); } catch (...) { resume_failed = true; }
  }
  // A callback violates the C contract if it throws. Contain that failure
  // without leaving a live frame suspended with no reachable continuation.
  // Keep resume_active until the common cleanup path, so cancellation
  // observers cannot destroy the native frame before this slice has ended.
  if (resume_failed) cancel_task(task);
  void *frame_to_destroy = nullptr;
  objc3_frame_destroy frame_destructor = nullptr;
  ResumeDispatch *rejected = nullptr;
  objc3_service_status dispatch_status = OBJC3_SERVICE_OK;
  {
    std::lock_guard<std::mutex> lock(task->mutex);
    if (resume) task->resume_active = false;
    const bool resume_requested = std::exchange(task->resume_requested, false);
    if (!task->terminal && resume_requested)
      dispatch_status = queue_resume_locked(task, rejected);
    if (task->terminal && !task->resume_active && task->frame) {
      frame_to_destroy = std::exchange(task->frame, nullptr);
      frame_destructor = task->destroy;
      task->frame_destroying = true;
    }
  }
  if (frame_to_destroy)
    finish_frame_destruction(task, frame_to_destroy, frame_destructor);
  else
    task->completed.notify_all();
  notify_quiescence(task);
  release_rejected_dispatch(rejected);
  if (dispatch_status != OBJC3_SERVICE_OK) cancel_task(task);
  finish_continuation_owner(task, continuation_owner);
}
