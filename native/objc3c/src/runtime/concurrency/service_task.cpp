#include "service_internal.h"

#include <cstdlib>
#include <cstring>
#include <new>
#include <utility>

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
    if (!value->has_payload)
      return value->payload == nullptr && value->payload_ops == nullptr
                 ? OBJC3_SERVICE_OK : OBJC3_SERVICE_INVALID_ARGUMENT;
    if (value->has_payload != 1) return OBJC3_SERVICE_INVALID_ARGUMENT;
    const objc3_service_status ops_status = validate_ops(value->payload_ops);
    if (ops_status != OBJC3_SERVICE_OK) return ops_status;
    return value->payload || value->payload_ops->payload_size == 0
               ? OBJC3_SERVICE_OK : OBJC3_SERVICE_INVALID_ARGUMENT;
  }
  case OBJC3_COMPLETION_ERROR:
    return !value->has_payload && !value->payload && !value->payload_ops &&
                   value->error
               ? OBJC3_SERVICE_OK : OBJC3_SERVICE_INVALID_ARGUMENT;
  case OBJC3_COMPLETION_CANCELLED:
    return !value->has_payload && !value->payload && !value->payload_ops &&
                   !value->error
               ? OBJC3_SERVICE_OK : OBJC3_SERVICE_INVALID_ARGUMENT;
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
  return value;
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
}

void finish_frame_destruction(objc3_task_t *task, void *frame,
                              objc3_frame_destroy destroy) noexcept {
  if (frame) {
    CurrentTaskScope current(task);
    objc3_task_destroy_frame(frame, destroy);
  }
  {
    std::lock_guard<std::mutex> lock(task->mutex);
    task->frame_destroying = false;
    task->frame_destroyed = true;
  }
  task->completed.notify_all();
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

objc3_task_t::~objc3_task() {
  // A last handle released while suspended becomes an implicit cancellation.
  // The release path normally terminalizes first; this is a defensive fallback.
  release_payload(this);
  if (error) objc3_error_release(error);
  if (frame && destroy) objc3_task_destroy_frame(frame, destroy);
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
    return task->terminal && task->frame_destroyed && !task->frame_destroying &&
           !task->resume_active && task->callbacks_active == 0;
  };
  // Blocking a serial executor can prevent this task's queued resume or
  // cleanup from running. This is a logical executor check, independent of
  // whether a manual pump or a worker currently runs the callback.
  if (!quiescent() && task->executor &&
      objc3::services::executor_is_current_identity(task->executor->identity)) {
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

void objc3_task_internal_release(objc3_task_t *task) {
  if (task->references.fetch_sub(1, std::memory_order_acq_rel) == 1) {
    bool needs_cancel;
    {
      std::lock_guard<std::mutex> lock(task->mutex);
      needs_cancel = !task->terminal;
    }
    if (needs_cancel) {
      // Keep the record alive across cancellation callbacks and their reentry.
      task->references.store(1, std::memory_order_release);
      objc3_completion cancelled{};
      cancelled.struct_size = sizeof(cancelled);
      cancelled.version = OBJC3_SERVICE_ABI_VERSION;
      cancelled.kind = OBJC3_COMPLETION_CANCELLED;
      (void)objc3_task_complete(task, &cancelled);
      objc3_task_internal_release(task);
    } else {
      delete task;
    }
  }
}

extern "C" objc3_service_status objc3_task_start(
    const objc3_task_start_options *options, objc3_task_t **out_task) {
  if (!out_task) return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_task = nullptr;
  if (!options || options->struct_size < sizeof(objc3_task_start_options) ||
      options->version != OBJC3_SERVICE_ABI_VERSION || !options->executor ||
      !options->resume || (options->frame && !options->destroy))
    return options && options->version != OBJC3_SERVICE_ABI_VERSION
               ? OBJC3_SERVICE_UNSUPPORTED_VERSION
               : OBJC3_SERVICE_INVALID_ARGUMENT;
  auto *task = new (std::nothrow) objc3_task;
  if (!task) return OBJC3_SERVICE_OUT_OF_MEMORY;
  task->executor = objc3_executor_retain(options->executor);
  if (!task->executor) {
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
      objc3_executor_release(task->executor);
      task->executor = nullptr;
      delete task;
      return OBJC3_SERVICE_OUT_OF_MEMORY;
    }
  }
  {
    std::lock_guard<std::mutex> lock(task->mutex);
    task->resume_queued = true;
  }
  if (!objc3_task_internal_retain(task)) {
    task->frame = nullptr; // Ownership transfers only after start succeeds.
    objc3_executor_release(task->executor);
    task->executor = nullptr;
    delete task;
    return OBJC3_SERVICE_NOT_READY;
  }
  const objc3_service_status queued = objc3_executor_enqueue(
      task->executor, objc3_service_run_task_resume, task);
  if (queued != OBJC3_SERVICE_OK) {
    task->frame = nullptr;
    {
      std::lock_guard<std::mutex> lock(task->mutex);
      task->terminal = true;
    }
    objc3_task_internal_release(task);
    objc3_task_internal_release(task); // also release start's initial reference
    return queued;
  }
  *out_task = task;
  return OBJC3_SERVICE_OK;
}

extern "C" objc3_task_t *objc3_task_retain(objc3_task_t *task) {
  return objc3_task_internal_retain(task) ? task : nullptr;
}

extern "C" void objc3_task_release(objc3_task_t *task) {
  if (task) objc3_task_internal_release(task);
}

extern "C" void objc3_task_request_cancel(objc3_task_t *task) {
  if (task) task->cancel_requested.store(true, std::memory_order_release);
}

extern "C" int objc3_task_is_cancel_requested(const objc3_task_t *task) {
  return task && task->cancel_requested.load(std::memory_order_acquire) ? 1 : 0;
}

extern "C" objc3_service_status objc3_task_schedule_resume(objc3_task_t *task) {
  if (!task) return OBJC3_SERVICE_INVALID_ARGUMENT;
  {
    std::lock_guard<std::mutex> lock(task->mutex);
    if (task->terminal || task->completion_claimed)
      return OBJC3_SERVICE_ALREADY_COMPLETED;
    if (task->resume_queued) return OBJC3_SERVICE_OK;
    task->resume_queued = true;
  }
  if (!objc3_task_internal_retain(task)) {
    std::lock_guard<std::mutex> lock(task->mutex);
    task->resume_queued = false;
    return OBJC3_SERVICE_NOT_READY;
  }
  const objc3_service_status status = objc3_executor_enqueue(
      task->executor, objc3_service_run_task_resume, task);
  if (status != OBJC3_SERVICE_OK) {
    {
      std::lock_guard<std::mutex> lock(task->mutex);
      task->resume_queued = false;
    }
    objc3_task_internal_release(task);
  }
  return status;
}

extern "C" objc3_service_status objc3_task_complete(
    objc3_task_t *task, const objc3_completion *completion) {
  if (!task) return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (!objc3_task_internal_retain(task)) return OBJC3_SERVICE_NOT_READY;
  TaskReleaseGuard task_guard{task};
  const objc3_service_status validation = validate_completion(completion);
  if (validation != OBJC3_SERVICE_OK) return validation;
  {
    std::lock_guard<std::mutex> lock(task->mutex);
    if (task->terminal || task->completion_claimed)
      return OBJC3_SERVICE_ALREADY_COMPLETED;
    task->completion_claimed = true;
  }

  void *copied_payload = nullptr;
  objc3_value_ops copied_ops{};
  objc3_error_t *retained_error = nullptr;
  objc3_service_status copy_status = OBJC3_SERVICE_OK;
  if (completion->kind == OBJC3_COMPLETION_SUCCESS && completion->has_payload) {
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
    std::lock_guard<std::mutex> lock(task->mutex);
    task->completion_claimed = false;
    return copy_status;
  }

  std::vector<objc3_task::Waiter> waiters;
  void *frame_to_destroy = nullptr;
  objc3_frame_destroy frame_destructor = nullptr;
  objc3_completion borrowed{};
  {
    std::lock_guard<std::mutex> lock(task->mutex);
    task->kind = completion->kind;
    task->has_payload = completion->kind == OBJC3_COMPLETION_SUCCESS &&
                        completion->has_payload != 0;
    task->payload = copied_payload;
    task->payload_ops = copied_ops;
    task->payload_ops_set = task->has_payload;
    task->error = retained_error;
    task->terminal = true;
    task->completion_claimed = false;
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
  }
  return OBJC3_SERVICE_OK;
}

extern "C" objc3_service_status objc3_task_wait(objc3_task_t *task) {
  if (!task) return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (!objc3_task_internal_retain(task)) return OBJC3_SERVICE_NOT_READY;
  std::unique_lock<std::mutex> lock(task->mutex);
  if (!task->terminal && (current_service_task == task ||
      (task->executor && objc3::services::executor_is_current_identity(
                             task->executor->identity)))) {
    lock.unlock();
    objc3_task_internal_release(task);
    return OBJC3_SERVICE_NOT_READY;
  }
  task->completed.wait(lock, [task] { return task->terminal; });
  lock.unlock();
  objc3_task_internal_release(task);
  return OBJC3_SERVICE_OK;
}

void objc3_service_run_task_resume(void *context) {
  auto *task = static_cast<objc3_task_t *>(context);
  void *frame = nullptr;
  objc3_task_resume resume = nullptr;
  {
    std::lock_guard<std::mutex> lock(task->mutex);
    task->resume_queued = false;
    if (!task->terminal && !task->resume_active) {
      task->resume_active = true;
      frame = task->frame;
      resume = task->resume;
    }
  }
  if (resume) {
    CurrentTaskScope current(task);
    try { resume(task, frame); } catch (...) {}
  }
  void *frame_to_destroy = nullptr;
  objc3_frame_destroy frame_destructor = nullptr;
  {
    std::lock_guard<std::mutex> lock(task->mutex);
    if (resume) task->resume_active = false;
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
  objc3_task_internal_release(task);
}
