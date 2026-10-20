#include "service_group.h"

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <new>
#include <vector>

namespace objc3::services {

struct TaskGroup;

struct GroupParentLink final {
  std::mutex mutex;
  TaskGroup *group{};
};

struct FailurePublicationState final {
  std::mutex mutex;
};

struct TaskGroup final {
  struct TaskEntry {
    objc3_task_t *task{};
    bool completed{false};
    bool consumed{false};
    bool failed{false};
    uint64_t completion_order{0};
  };

  std::atomic<uint32_t> references{1};
  std::mutex lifetime_mutex;
  mutable std::mutex mutex;
  std::shared_ptr<FailurePublicationState> failure_publication;
  std::shared_ptr<GroupParentLink> self_link;
  std::shared_ptr<GroupParentLink> parent_link;
  std::condition_variable changed;
  std::vector<TaskEntry> tasks;
  std::vector<TaskGroup *> children;
  uint32_t registrations_inflight{0};
  uint32_t child_closures_inflight{0};
  uint64_t next_completion_order{1};
  bool closed{false};
  bool cancelled{false};
  bool first_failure_seen{false};
  bool failure_propagation_failed{false};

  TaskGroup()
      : failure_publication(std::make_shared<FailurePublicationState>()),
        self_link(std::make_shared<GroupParentLink>()) {
    self_link->group = this;
  }

  ~TaskGroup() {
    {
      std::lock_guard<std::mutex> lock(self_link->mutex);
      if (self_link->group == this) self_link->group = nullptr;
    }
    for (const auto &entry : tasks) objc3_task_release(entry.task);
    for (auto *child : children) task_group_release(child);
  }
};

namespace {

struct ObserverContext { TaskGroup *group; objc3_task_t *task; };
#if defined(OBJC3_RUNTIME_SERVICE_TESTING)
std::atomic<TaskGroupFailurePropagationHook> failure_propagation_test_hook{nullptr};
std::atomic<void *> failure_propagation_test_context{nullptr};
#endif

TaskGroup *retain_if_alive(TaskGroup *group) {
  if (!group) return nullptr;
  std::lock_guard<std::mutex> lock(group->lifetime_mutex);
  if (!try_retain(group->references)) return nullptr;
  return group;
}

TaskGroup *retain_parent(TaskGroup *group, bool *retain_failed = nullptr) {
  if (retain_failed) *retain_failed = false;
  if (!group->parent_link) return nullptr;
  std::lock_guard<std::mutex> lock(group->parent_link->mutex);
  TaskGroup *parent = group->parent_link->group;
  if (!parent) return nullptr;
  parent = retain_if_alive(parent);
  if (!parent && retain_failed) *retain_failed = true;
  return parent;
}

struct AncestorLeases final {
  std::vector<TaskGroup *> groups;
  ~AncestorLeases() {
    for (auto *group : groups) task_group_release(group);
  }
};

objc3_service_status snapshot_failure_ancestors(TaskGroup *group,
                                                AncestorLeases &ancestors) {
  TaskGroup *child = group;
  for (;;) {
    bool retain_failed = false;
    TaskGroup *parent = retain_parent(child, &retain_failed);
    if (!parent) {
      if (retain_failed) return OBJC3_SERVICE_NOT_READY;
      return OBJC3_SERVICE_OK;
    }
    try {
      ancestors.groups.push_back(parent);
    } catch (...) {
      task_group_release(parent);
      return OBJC3_SERVICE_OUT_OF_MEMORY;
    }
    child = parent;
  }
}

objc3_service_status cancel_group_siblings_locked(
    TaskGroup *group, TaskGroup *failed_child = nullptr,
    objc3_task_t *failed_task = nullptr) {
  group->cancelled = true;
  for (auto &entry : group->tasks)
    if (!entry.completed && entry.task != failed_task)
      objc3_task_request_cancel(entry.task);
  for (auto *child : group->children) {
    if (child == failed_child) continue;
    const objc3_service_status status = task_group_cancel(child);
    if (status != OBJC3_SERVICE_OK) return status;
  }
  return OBJC3_SERVICE_OK;
}

bool contains_task(TaskGroup *group, objc3_task_t *task) {
  std::lock_guard<std::mutex> lock(group->mutex);
  for (const auto &entry : group->tasks)
    if (entry.task == task) return true;
  for (auto *child : group->children)
    if (contains_task(child, task)) return true;
  return false;
}

bool all_tasks_complete_locked(const TaskGroup *group) {
  if (group->registrations_inflight || group->child_closures_inflight)
    return false;
  return std::all_of(group->tasks.begin(), group->tasks.end(),
                     [](const auto &entry) { return entry.completed; });
}

objc3_service_status take_completed_locked(TaskGroup *group,
                                             objc3_task_t **out_task,
                                             bool *available = nullptr) {
  if (available) *available = false;
  TaskGroup::TaskEntry *next = nullptr;
  for (auto &entry : group->tasks)
    if (entry.completed && !entry.consumed &&
        (!next || entry.completion_order < next->completion_order))
      next = &entry;
  if (next) {
    if (available) *available = true;
    *out_task = objc3_task_retain(next->task);
    if (!*out_task) return OBJC3_SERVICE_NOT_READY;
    next->consumed = true;
    return OBJC3_SERVICE_OK;
  }
  if (group->failure_propagation_failed) return OBJC3_SERVICE_NOT_READY;
  return group->closed && all_tasks_complete_locked(group)
             ? OBJC3_SERVICE_CLOSED : OBJC3_SERVICE_NOT_READY;
}

void task_completed(void *opaque, const objc3_completion *completion) {
  auto *observer = static_cast<ObserverContext *>(opaque);
  TaskGroup *group = observer->group;
  objc3_task_t *task = observer->task;
  bool propagate_failure = false;
  bool publish_completion = false;
  objc3_service_status propagation_status = OBJC3_SERVICE_OK;
  AncestorLeases ancestors;
  const bool is_failure = completion->kind == OBJC3_COMPLETION_ERROR;
  if (is_failure)
    propagation_status = snapshot_failure_ancestors(group, ancestors);
  std::unique_lock<std::mutex> failure_order(
      group->failure_publication->mutex, std::defer_lock);
  if (is_failure) failure_order.lock();
  {
    std::lock_guard<std::mutex> lock(group->mutex);
    auto found = std::find_if(group->tasks.begin(), group->tasks.end(),
                              [task](const auto &entry) {
                                return entry.task == task;
                              });
    if (found != group->tasks.end() && !found->completed) {
      found->failed = completion->kind == OBJC3_COMPLETION_ERROR;
      publish_completion = !group->failure_propagation_failed &&
                           propagation_status == OBJC3_SERVICE_OK;
      if (found->failed && publish_completion) {
        if (!group->first_failure_seen) {
          group->first_failure_seen = true;
          propagation_status = cancel_group_siblings_locked(group, nullptr, task);
        }
        propagate_failure = propagation_status == OBJC3_SERVICE_OK;
      }
    }
  }
  if (propagate_failure && propagation_status == OBJC3_SERVICE_OK) {
#if defined(OBJC3_RUNTIME_SERVICE_TESTING)
    if (const auto hook = failure_propagation_test_hook.load(
            std::memory_order_acquire))
      hook(failure_propagation_test_context.load(std::memory_order_acquire));
#endif
    for (size_t index = 0; index < ancestors.groups.size(); ++index) {
      TaskGroup *parent = ancestors.groups[index];
      TaskGroup *failed_child = index == 0 ? group : ancestors.groups[index - 1];
      {
        std::lock_guard<std::mutex> lock(parent->mutex);
        // Check the failure state even if a prior child already set the
        // first-failure bit. A later child must not publish through a failed
        // ancestor whose earlier propagation could not be completed.
        if (parent->failure_propagation_failed) {
          propagation_status = OBJC3_SERVICE_NOT_READY;
        } else if (!parent->first_failure_seen) {
          parent->first_failure_seen = true;
          propagation_status = cancel_group_siblings_locked(parent, failed_child);
        }
      }
      parent->changed.notify_all();
      if (propagation_status != OBJC3_SERVICE_OK) {
        std::lock_guard<std::mutex> lock(parent->mutex);
        parent->failure_propagation_failed = true;
        break;
      }
    }
  }
  if (propagation_status != OBJC3_SERVICE_OK) {
    std::lock_guard<std::mutex> lock(group->mutex);
    group->failure_propagation_failed = true;
    publish_completion = false;
  }
  // Publish the child failure only after ancestor sibling cancellation. A
  // concurrent consumer may poll without waiting for the condition variable.
  if (publish_completion) {
    std::lock_guard<std::mutex> lock(group->mutex);
    auto found = std::find_if(group->tasks.begin(), group->tasks.end(),
                             [task](const auto &entry) { return entry.task == task; });
    if (found != group->tasks.end() && !found->completed) {
      found->completed = true;
      found->completion_order = group->next_completion_order++;
    }
  }
  if (failure_order.owns_lock()) failure_order.unlock();
  group->changed.notify_all();
  delete observer;
  task_group_release(group);
}

} // namespace

TaskGroup *task_group_retain(TaskGroup *group) {
  return retain_if_alive(group);
}

#if defined(OBJC3_RUNTIME_SERVICE_TESTING)
void task_group_test_set_failure_propagation_hook(
    TaskGroupFailurePropagationHook hook, void *context) {
  failure_propagation_test_context.store(context, std::memory_order_release);
  failure_propagation_test_hook.store(hook, std::memory_order_release);
}

objc3_service_status task_group_test_set_references(TaskGroup *group,
                                                    uint32_t references) {
  if (!group || references == 0) return OBJC3_SERVICE_INVALID_ARGUMENT;
  std::lock_guard<std::mutex> lock(group->lifetime_mutex);
  group->references.store(references, std::memory_order_release);
  return OBJC3_SERVICE_OK;
}
#endif

void task_group_release(TaskGroup *group) {
  if (!group) return;
  bool destroy = false;
  {
    std::lock_guard<std::mutex> lock(group->lifetime_mutex);
    destroy = group->references.fetch_sub(1, std::memory_order_acq_rel) == 1;
  }
  if (destroy) delete group;
}

objc3_service_status task_group_create(TaskGroup *parent, TaskGroup **out_group) {
  if (!out_group) return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_group = nullptr;
  TaskGroup *group = nullptr;
  try { group = new (std::nothrow) TaskGroup; }
  catch (...) { return OBJC3_SERVICE_OUT_OF_MEMORY; }
  if (!group) return OBJC3_SERVICE_OUT_OF_MEMORY;
  bool cancelled = false;
  if (parent) {
    std::lock_guard<std::mutex> lock(parent->mutex);
    if (parent->closed) { delete group; return OBJC3_SERVICE_CLOSED; }
    if (!task_group_retain(group)) {
      delete group;
      return OBJC3_SERVICE_NOT_READY;
    }
    try { parent->children.push_back(group); }
    catch (...) {
      task_group_release(group); // release the unlinked parent association
      task_group_release(group); // release the new group's initial owner
      return OBJC3_SERVICE_OUT_OF_MEMORY;
    }
    group->failure_publication = parent->failure_publication;
    group->parent_link = parent->self_link;
    cancelled = parent->cancelled;
  }
  *out_group = group;
  if (cancelled) (void)task_group_cancel(group);
  return OBJC3_SERVICE_OK;
}

objc3_service_status task_group_add_task(TaskGroup *group, objc3_task_t *task) {
  if (!group || !task) return OBJC3_SERVICE_INVALID_ARGUMENT;
  auto *observer = new (std::nothrow) ObserverContext{group, task};
  if (!observer) return OBJC3_SERVICE_OUT_OF_MEMORY;
  if (!objc3_task_retain(task)) {
    delete observer;
    return OBJC3_SERVICE_NOT_READY;
  }
  if (!task_group_retain(group)) {
    objc3_task_release(task);
    delete observer;
    return OBJC3_SERVICE_NOT_READY;
  }
  bool cancelled = false;
  objc3_service_status insertion = OBJC3_SERVICE_OK;
  {
    std::lock_guard<std::mutex> lock(group->mutex);
    if (group->closed) insertion = OBJC3_SERVICE_CLOSED;
    else if (std::any_of(group->tasks.begin(), group->tasks.end(),
                         [task](const auto &entry) {
                           return entry.task == task;
                         }))
      insertion = OBJC3_SERVICE_INVALID_ARGUMENT;
    else {
      try {
        group->tasks.push_back({task});
        ++group->registrations_inflight;
        cancelled = group->cancelled;
      } catch (...) { insertion = OBJC3_SERVICE_OUT_OF_MEMORY; }
    }
  }
  if (insertion != OBJC3_SERVICE_OK) {
    objc3_task_release(task);
    task_group_release(group);
    delete observer;
    return insertion;
  }
  if (cancelled) objc3_task_request_cancel(task);
  const objc3_service_status status =
      objc3_task_add_waiter(task, task_completed, observer);
  if (status != OBJC3_SERVICE_OK) {
    objc3_task_t *removed_task = nullptr;
    {
      std::lock_guard<std::mutex> lock(group->mutex);
      auto found = std::find_if(group->tasks.begin(), group->tasks.end(),
                                [task](const auto &entry) {
                                  return entry.task == task;
                                });
      if (found != group->tasks.end()) {
        removed_task = found->task;
        group->tasks.erase(found);
      }
      --group->registrations_inflight;
    }
    group->changed.notify_all();
    if (removed_task) objc3_task_release(removed_task);
    delete observer;
    task_group_release(group);
    return status;
  }
  {
    std::lock_guard<std::mutex> lock(group->mutex);
    --group->registrations_inflight;
    auto found = std::find_if(group->tasks.begin(), group->tasks.end(),
                              [task](const auto &entry) {
                                return entry.task == task;
                              });
    if (group->cancelled && found != group->tasks.end() && !found->completed)
      objc3_task_request_cancel(task);
  }
  group->changed.notify_all();
  return OBJC3_SERVICE_OK;
}

objc3_service_status task_group_close(TaskGroup *group) {
  if (!group) return OBJC3_SERVICE_INVALID_ARGUMENT;
  std::unique_lock<std::mutex> lock(group->mutex);
  group->closed = true;
  size_t index = 0;
  while (index < group->children.size()) {
    auto *child = task_group_retain(group->children[index++]);
    if (!child) return OBJC3_SERVICE_NOT_READY;
    ++group->child_closures_inflight;
    lock.unlock();
    const auto status = task_group_close(child);
    task_group_release(child);
    lock.lock();
    --group->child_closures_inflight;
    group->changed.notify_all();
    if (status != OBJC3_SERVICE_OK) return status;
  }
  group->changed.notify_all();
  return OBJC3_SERVICE_OK;
}

objc3_service_status task_group_cancel(TaskGroup *group) {
  if (!group) return OBJC3_SERVICE_INVALID_ARGUMENT;
  std::unique_lock<std::mutex> lock(group->mutex);
  group->cancelled = true;
  for (auto &entry : group->tasks)
    if (!entry.completed) objc3_task_request_cancel(entry.task);
  size_t index = 0;
  while (index < group->children.size()) {
    auto *child = task_group_retain(group->children[index++]);
    if (!child) return OBJC3_SERVICE_NOT_READY;
    lock.unlock();
    const auto status = task_group_cancel(child);
    task_group_release(child);
    lock.lock();
    if (status != OBJC3_SERVICE_OK) return status;
  }
  return OBJC3_SERVICE_OK;
}

objc3_service_status task_group_next_completed(TaskGroup *group,
                                               objc3_task_t **out_task) {
  if (!group || !out_task) return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_task = nullptr;
  objc3_task_t *current = objc3_task_current_callback();
  if (current && contains_task(group, current)) {
    std::lock_guard<std::mutex> lock(group->mutex);
    const bool available = std::any_of(
        group->tasks.begin(), group->tasks.end(), [](const auto &entry) {
          return entry.completed && !entry.consumed;
        });
    if (!available) return OBJC3_SERVICE_NOT_READY;
  }
  std::unique_lock<std::mutex> lock(group->mutex);
  for (;;) {
    bool available = false;
    const auto result = take_completed_locked(group, out_task, &available);
    if (result != OBJC3_SERVICE_NOT_READY || available || group->failure_propagation_failed)
      return result;
    group->changed.wait(lock, [group] {
      if (group->failure_propagation_failed) return true;
      if (group->closed && all_tasks_complete_locked(group)) return true;
      return std::any_of(group->tasks.begin(), group->tasks.end(),
                         [](const auto &entry) {
                           return entry.completed && !entry.consumed;
                         });
    });
  }
}

objc3_service_status task_group_try_next_completed(TaskGroup *group,
                                                   objc3_task_t **out_task) {
  if (!out_task) return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_task = nullptr;
  if (!group) return OBJC3_SERVICE_INVALID_ARGUMENT;
  std::lock_guard<std::mutex> lock(group->mutex);
  return take_completed_locked(group, out_task);
}

objc3_service_status task_group_join(TaskGroup *group) {
  if (!group) return OBJC3_SERVICE_INVALID_ARGUMENT;
  objc3_task_t *current = objc3_task_current_callback();
  if (current && contains_task(group, current)) return OBJC3_SERVICE_NOT_READY;

  std::vector<objc3_task_t *> tasks;
  std::vector<TaskGroup *> children;
  objc3_service_status capture_status = OBJC3_SERVICE_OK;
  {
    std::unique_lock<std::mutex> lock(group->mutex);
    if (!group->closed) return OBJC3_SERVICE_NOT_READY;
    if (group->failure_propagation_failed) return OBJC3_SERVICE_NOT_READY;
    group->changed.wait(lock, [group] {
      return group->registrations_inflight == 0 &&
             group->child_closures_inflight == 0;
    });
    if (group->failure_propagation_failed) return OBJC3_SERVICE_NOT_READY;
    try {
      tasks.reserve(group->tasks.size());
      children.reserve(group->children.size());
    } catch (...) {
      capture_status = OBJC3_SERVICE_OUT_OF_MEMORY;
    }
    if (capture_status == OBJC3_SERVICE_OK) {
      for (const auto &entry : group->tasks) {
        auto *task = objc3_task_retain(entry.task);
        if (!task) {
          capture_status = OBJC3_SERVICE_NOT_READY;
          break;
        }
        try { tasks.push_back(task); }
        catch (...) {
          objc3_task_release(task);
          capture_status = OBJC3_SERVICE_OUT_OF_MEMORY;
          break;
        }
      }
    }
    if (capture_status == OBJC3_SERVICE_OK) {
      for (auto *owned_child : group->children) {
        auto *child = task_group_retain(owned_child);
        if (!child) {
          capture_status = OBJC3_SERVICE_NOT_READY;
          break;
        }
        try { children.push_back(child); }
        catch (...) {
          task_group_release(child);
          capture_status = OBJC3_SERVICE_OUT_OF_MEMORY;
          break;
        }
      }
    }
    if (capture_status != OBJC3_SERVICE_OK) lock.unlock();
  }
  if (capture_status != OBJC3_SERVICE_OK) {
    for (auto *task : tasks) objc3_task_release(task);
    for (auto *child : children) task_group_release(child);
    return capture_status;
  }

  objc3_service_status status = OBJC3_SERVICE_OK;
  size_t child_index = 0;
  for (; child_index < children.size(); ++child_index) {
    status = task_group_join(children[child_index]);
    task_group_release(children[child_index]);
    if (status != OBJC3_SERVICE_OK) { ++child_index; break; }
  }
  for (; child_index < children.size(); ++child_index)
    task_group_release(children[child_index]);
  if (status == OBJC3_SERVICE_OK) {
    for (auto *task : tasks) {
      status = objc3_task_wait_quiescent(task);
      if (status != OBJC3_SERVICE_OK) break;
    }
  }
  for (auto *task : tasks) objc3_task_release(task);
  if (status == OBJC3_SERVICE_OK) {
    std::lock_guard<std::mutex> lock(group->mutex);
    if (group->failure_propagation_failed)
      status = OBJC3_SERVICE_NOT_READY;
  }
  return status;
}

} // namespace objc3::services
