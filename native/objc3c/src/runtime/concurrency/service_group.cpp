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

  TaskGroup() : self_link(std::make_shared<GroupParentLink>()) {
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

TaskGroup *retain_if_alive(TaskGroup *group) {
  if (!group) return nullptr;
  std::lock_guard<std::mutex> lock(group->lifetime_mutex);
  if (group->references.load(std::memory_order_acquire) == 0) return nullptr;
  group->references.fetch_add(1, std::memory_order_relaxed);
  return group;
}

TaskGroup *retain_parent(TaskGroup *group) {
  if (!group->parent_link) return nullptr;
  std::lock_guard<std::mutex> lock(group->parent_link->mutex);
  return retain_if_alive(group->parent_link->group);
}

void propagate_nested_failure(TaskGroup *failed_group);

void cancel_group_siblings_locked(TaskGroup *group,
                                  TaskGroup *failed_child = nullptr,
                                  objc3_task_t *failed_task = nullptr) {
  group->cancelled = true;
  for (auto &entry : group->tasks)
    if (!entry.completed && entry.task != failed_task)
      objc3_task_request_cancel(entry.task);
  for (auto *child : group->children)
    if (child != failed_child) (void)task_group_cancel(child);
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

void task_completed(void *opaque, const objc3_completion *completion) {
  auto *observer = static_cast<ObserverContext *>(opaque);
  TaskGroup *group = observer->group;
  objc3_task_t *task = observer->task;
  bool propagate_failure = false;
  bool publish_completion = false;
  {
    std::lock_guard<std::mutex> lock(group->mutex);
    auto found = std::find_if(group->tasks.begin(), group->tasks.end(),
                              [task](const auto &entry) {
                                return entry.task == task;
                              });
    if (found != group->tasks.end() && !found->completed) {
      found->failed = completion->kind == OBJC3_COMPLETION_ERROR;
      publish_completion = true;
      if (found->failed && !group->first_failure_seen) {
        group->first_failure_seen = true;
        cancel_group_siblings_locked(group, nullptr, task);
        propagate_failure = true;
      }
    }
  }
  if (propagate_failure) propagate_nested_failure(group);
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
  group->changed.notify_all();
  delete observer;
  task_group_release(group);
}

void propagate_nested_failure(TaskGroup *failed_group) {
  TaskGroup *parent = retain_parent(failed_group);
  if (!parent) return;
  bool propagate = false;
  {
    std::lock_guard<std::mutex> lock(parent->mutex);
    if (!parent->first_failure_seen) {
      parent->first_failure_seen = true;
      cancel_group_siblings_locked(parent, failed_group);
      propagate = true;
    }
  }
  parent->changed.notify_all();
  if (propagate) propagate_nested_failure(parent);
  task_group_release(parent);
}

} // namespace

TaskGroup *task_group_retain(TaskGroup *group) {
  return retain_if_alive(group);
}

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
    try { parent->children.push_back(group); }
    catch (...) { delete group; return OBJC3_SERVICE_OUT_OF_MEMORY; }
    group->parent_link = parent->self_link;
    task_group_retain(group); // parent owns the association
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
  objc3_task_retain(task);
  task_group_retain(group);
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
    TaskGroup::TaskEntry *next = nullptr;
    for (auto &entry : group->tasks)
      if (entry.completed && !entry.consumed &&
          (!next || entry.completion_order < next->completion_order))
        next = &entry;
    if (next) {
      next->consumed = true;
      *out_task = objc3_task_retain(next->task);
      return OBJC3_SERVICE_OK;
    }
    if (group->closed && all_tasks_complete_locked(group))
      return OBJC3_SERVICE_CLOSED;
    group->changed.wait(lock, [group] {
      if (group->closed && all_tasks_complete_locked(group)) return true;
      return std::any_of(group->tasks.begin(), group->tasks.end(),
                         [](const auto &entry) {
                           return entry.completed && !entry.consumed;
                         });
    });
  }
}

objc3_service_status task_group_join(TaskGroup *group) {
  if (!group) return OBJC3_SERVICE_INVALID_ARGUMENT;
  objc3_task_t *current = objc3_task_current_callback();
  if (current && contains_task(group, current)) return OBJC3_SERVICE_NOT_READY;

  std::vector<objc3_task_t *> tasks;
  std::vector<TaskGroup *> children;
  bool allocation_failed = false;
  {
    std::unique_lock<std::mutex> lock(group->mutex);
    if (!group->closed) return OBJC3_SERVICE_NOT_READY;
    group->changed.wait(lock, [group] {
      return group->registrations_inflight == 0 &&
             group->child_closures_inflight == 0;
    });
    try {
      tasks.reserve(group->tasks.size());
      for (const auto &entry : group->tasks)
        tasks.push_back(objc3_task_retain(entry.task));
      children = group->children;
      for (auto *child : children) task_group_retain(child);
    } catch (...) {
      allocation_failed = true;
    }
    if (allocation_failed) lock.unlock();
  }
  if (allocation_failed) {
    for (auto *task : tasks) objc3_task_release(task);
    for (auto *child : children) task_group_release(child);
    return OBJC3_SERVICE_OUT_OF_MEMORY;
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
  return status;
}

} // namespace objc3::services
