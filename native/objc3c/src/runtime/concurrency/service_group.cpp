#include "service_group.h"

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <list>
#include <memory>
#include <mutex>
#include <new>
#include <utility>
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
  struct NextWaiter {
    ObjC3TaskAwaitContinuation continuation;
    objc3_async_result_type expected{};
    std::string identity;
    bool typed{false};
    ObjC3TaskAwaitClaim claim;
    objc3_task_t *untransferred_result{nullptr};
  };
  struct JoinWaiter {
    ObjC3TaskAwaitContinuation continuation;
    objc3_service_status status{OBJC3_SERVICE_OK};
    bool cleanup{false};
    objc3_task_t *owned_parent{nullptr};
    std::shared_ptr<void> cleanup_owner;
    JoinWaiter() = default;
    JoinWaiter(const JoinWaiter &) = delete;
    ~JoinWaiter() {
      objc3_task_release(owned_parent);
    }
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
  std::list<JoinWaiter> join_waiters;
  std::list<NextWaiter> next_waiters;
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

struct GroupOperationLease final {
  TaskGroup *group;
  explicit GroupOperationLease(TaskGroup *value) : group(retain_if_alive(value)) {}
  ~GroupOperationLease() { task_group_release(group); }
};

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

struct CancellationBatch {
  struct Entry { objc3_task_t *task; bool wake; };
  std::vector<Entry> entries;
  objc3_service_status request(objc3_task_t *task) {
    if (!objc3_task_retain(task)) return OBJC3_SERVICE_NOT_READY;
    try { entries.push_back({task, false}); }
    catch (...) {
      objc3_task_release(task);
      return OBJC3_SERVICE_OUT_OF_MEMORY;
    }
    entries.back().wake = objc3_task_mark_cancel(task);
    return OBJC3_SERVICE_OK;
  }
  ~CancellationBatch() {
    for (const auto &entry : entries) {
      if (entry.wake) objc3_task_wake_cancelled(entry.task);
      objc3_task_release(entry.task);
    }
  }
};

objc3_service_status mark_group_cancel(TaskGroup *group,
                                       CancellationBatch &cancellations);

objc3_service_status cancel_group_siblings_locked(
    TaskGroup *group, CancellationBatch &cancellations,
    TaskGroup *failed_child = nullptr,
    objc3_task_t *failed_task = nullptr) {
  group->cancelled = true;
  for (auto &entry : group->tasks)
    if (!entry.completed && entry.task != failed_task) {
      const auto status = cancellations.request(entry.task);
      if (status != OBJC3_SERVICE_OK) return status;
    }
  for (auto *child : group->children) {
    if (child == failed_child) continue;
    auto *retained = task_group_retain(child);
    if (!retained) return OBJC3_SERVICE_NOT_READY;
    const objc3_service_status status = mark_group_cancel(retained, cancellations);
    task_group_release(retained);
    if (status != OBJC3_SERVICE_OK) return status;
  }
  return OBJC3_SERVICE_OK;
}

objc3_service_status mark_group_cancel(TaskGroup *group,
                                       CancellationBatch &cancellations) {
  std::lock_guard<std::mutex> lock(group->mutex);
  return cancel_group_siblings_locked(group, cancellations);
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

enum class JoinState { Pending, Ready, Failed };

bool closed_tree_locked(const TaskGroup *group) {
  if (!group->closed || group->child_closures_inflight) return false;
  for (auto *child : group->children) {
    std::lock_guard<std::mutex> lock(child->mutex);
    if (!closed_tree_locked(child)) return false;
  }
  return true;
}

// The immutable closed tree uses the same parent-to-child lock order as group
// cancellation. Failure wins over pending work, so an observer never remains
// unreachable after failure-publication resource exhaustion.
JoinState join_state_locked(const TaskGroup *group) {
  if (group->failure_propagation_failed) return JoinState::Failed;
  bool pending = !group->closed || group->registrations_inflight ||
                 group->child_closures_inflight;
  for (const auto &entry : group->tasks) {
    std::lock_guard<std::mutex> lock(entry.task->mutex);
    if (!entry.completed || !entry.task->terminal ||
        !entry.task->frame_destroyed || entry.task->frame_destroying ||
        entry.task->resume_active || entry.task->callbacks_active ||
        entry.task->continuation_owners_active)
      pending = true;
  }
  for (auto *child : group->children) {
    std::lock_guard<std::mutex> lock(child->mutex);
    switch (join_state_locked(child)) {
    case JoinState::Failed: return JoinState::Failed;
    case JoinState::Pending: pending = true; break;
    case JoinState::Ready: break;
    }
  }
  return pending ? JoinState::Pending : JoinState::Ready;
}

// Cleanup cannot propagate a stored publication failure until the actual
// children, callbacks and frames have become quiescent. A failed publication
// may intentionally leave entry.completed false; it does not erase native work.
JoinState cleanup_join_state_locked(const TaskGroup *group) {
  bool failed = group->failure_propagation_failed;
  bool pending = !group->closed || group->registrations_inflight ||
                 group->child_closures_inflight;
  for (const auto &entry : group->tasks) {
    std::lock_guard<std::mutex> lock(entry.task->mutex);
    if (!entry.task->terminal || !entry.task->frame_destroyed ||
        entry.task->frame_destroying || entry.task->resume_active ||
        entry.task->callbacks_active || entry.task->continuation_owners_active)
      pending = true;
  }
  for (auto *child : group->children) {
    std::lock_guard<std::mutex> lock(child->mutex);
    switch (cleanup_join_state_locked(child)) {
    case JoinState::Failed: failed = true; break;
    case JoinState::Pending: pending = true; break;
    case JoinState::Ready: break;
    }
  }
  if (pending) return JoinState::Pending;
  return failed ? JoinState::Failed : JoinState::Ready;
}

void take_next_notifications_locked(
    TaskGroup *group, std::list<TaskGroup::NextWaiter> &ready) {
  while (!group->next_waiters.empty()) {
    TaskGroup::TaskEntry *entry = nullptr;
    for (auto &candidate : group->tasks)
      if (candidate.completed && !candidate.consumed &&
          (!entry || candidate.completion_order < entry->completion_order))
        entry = &candidate;
    objc3_service_status status = OBJC3_SERVICE_OK;
    if (!entry) {
      if (group->failure_propagation_failed) status = OBJC3_SERVICE_NOT_READY;
      else if (group->closed && all_tasks_complete_locked(group))
        status = OBJC3_SERVICE_CLOSED;
      else
        break;
    }
    auto &waiter = group->next_waiters.front();
    if (entry && waiter.typed) {
      const objc3_async_result_type *type = nullptr;
      if (objc3_task_get_result_type(entry->task, &type) != OBJC3_SERVICE_OK ||
          !objc3_async_result_type_equal(type, &waiter.expected))
        status = OBJC3_SERVICE_TYPE_MISMATCH;
    }
    objc3_task_t *result = nullptr;
    if (status == OBJC3_SERVICE_OK) {
      result = objc3_task_retain(entry->task);
      if (!result) status = OBJC3_SERVICE_NOT_READY;
    }
    waiter.claim = objc3_task_claim_await(waiter.continuation, status, result);
    if (waiter.claim.transferred_result)
      entry->consumed = true;
    else
      waiter.untransferred_result = result;
    // Reuse registration nodes after reservation. Dispatch/release can invoke
    // arbitrary frame cleanup and must run after every group/link lock.
    ready.splice(ready.end(), group->next_waiters, group->next_waiters.begin());
  }
}

void dispatch_next_notifications(std::list<TaskGroup::NextWaiter> &ready) {
  for (auto &waiter : ready) {
    objc3_task_release(std::exchange(waiter.untransferred_result, nullptr));
    objc3_task_dispatch_await(waiter.claim);
    waiter.claim = {};
  }
}

void dispatch_join_notifications(std::list<TaskGroup::JoinWaiter> &waiters) {
  for (const auto &waiter : waiters)
    objc3_task_finish_await(waiter.continuation, waiter.status, waiter.cleanup_owner);
  // Owned cleanup parent/group leases are released only after dispatch, with
  // every task/group/link/publication lock already dropped.
  waiters.clear();
}

std::shared_ptr<GroupParentLink> take_join_notifications(
    TaskGroup *group, std::list<TaskGroup::JoinWaiter> &waiters,
    std::list<TaskGroup::NextWaiter> &next) {
  std::lock_guard<std::mutex> lock(group->mutex);
  take_next_notifications_locked(group, next);
  if (!group->join_waiters.empty()) {
    const bool ordinary_waiters = std::any_of(
        group->join_waiters.begin(), group->join_waiters.end(),
        [](const auto &waiter) { return !waiter.cleanup; });
    const bool cleanup_waiters = std::any_of(
        group->join_waiters.begin(), group->join_waiters.end(),
        [](const auto &waiter) { return waiter.cleanup; });
    const auto ordinary = ordinary_waiters ? join_state_locked(group)
                                           : JoinState::Pending;
    const auto cleanup = cleanup_waiters ? cleanup_join_state_locked(group)
                                         : JoinState::Pending;
    for (auto iterator = group->join_waiters.begin();
         iterator != group->join_waiters.end();) {
      auto current = iterator++;
      const auto state = current->cleanup ? cleanup : ordinary;
      if (state != JoinState::Pending) {
        current->status = state == JoinState::Failed ? OBJC3_SERVICE_NOT_READY
                                                    : OBJC3_SERVICE_OK;
        waiters.splice(waiters.end(), group->join_waiters, current);
      }
    }
  }
  group->changed.notify_all();
  return group->parent_link;
}

void notify_ancestors(std::shared_ptr<GroupParentLink> parent_link) {
  std::list<TaskGroup::JoinWaiter> waiters;
  std::list<TaskGroup::NextWaiter> next_waiters;
  while (parent_link) {
    waiters.clear();
    next_waiters.clear();
    std::shared_ptr<GroupParentLink> next;
    {
      // The link protects a raw ancestor through its destruction boundary.
      // No callbacks run under it, and no saturated reference count can lose
      // the final readiness notification.
      std::lock_guard<std::mutex> link_lock(parent_link->mutex);
      if (parent_link->group)
        next = take_join_notifications(parent_link->group, waiters, next_waiters);
    }
    dispatch_join_notifications(waiters);
    dispatch_next_notifications(next_waiters);
    parent_link = std::move(next);
  }
}

void notify_group_changed(TaskGroup *group) {
  std::list<TaskGroup::JoinWaiter> waiters;
  std::list<TaskGroup::NextWaiter> next;
  auto parent_link = take_join_notifications(group, waiters, next);
  dispatch_join_notifications(waiters);
  dispatch_next_notifications(next);
  notify_ancestors(std::move(parent_link));
}

void task_quiescent(void *opaque) {
  auto *link = static_cast<GroupParentLink *>(opaque);
  std::list<TaskGroup::JoinWaiter> waiters;
  std::list<TaskGroup::NextWaiter> next;
  std::shared_ptr<GroupParentLink> parent_link;
  {
    std::lock_guard<std::mutex> lock(link->mutex);
    if (link->group)
      parent_link = take_join_notifications(link->group, waiters, next);
  }
  dispatch_join_notifications(waiters);
  dispatch_next_notifications(next);
  notify_ancestors(std::move(parent_link));
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
  std::list<TaskGroup::NextWaiter> next_waiters;
  // Destroy after failure_order: cancellation flags publish under its lock,
  // while dispatch and any synchronous completion callbacks run without it.
  CancellationBatch cancellations;
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
          propagation_status = cancel_group_siblings_locked(
              group, cancellations, nullptr, task);
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
          propagation_status = cancel_group_siblings_locked(
              parent, cancellations, failed_child);
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
    // Existing async consumers reserve under the same lock that publishes
    // completion, so a concurrent poll cannot overtake registered waiters.
    take_next_notifications_locked(group, next_waiters);
  }
  if (failure_order.owns_lock()) failure_order.unlock();
  dispatch_next_notifications(next_waiters);
  notify_group_changed(group);
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
  // Registering an already-terminal task invokes its observer synchronously.
  // That observer can dispatch user cleanup which drops the caller's last
  // group owner before registration bookkeeping below has finished.
  GroupOperationLease operation(group);
  if (!operation.group) return OBJC3_SERVICE_NOT_READY;
  auto *observer = new (std::nothrow) ObserverContext{group, task};
  if (!observer) return OBJC3_SERVICE_OUT_OF_MEMORY;
  std::shared_ptr<ObjC3TaskQuiescenceObserver> quiescence;
  try {
    quiescence = std::make_shared<ObjC3TaskQuiescenceObserver>();
    quiescence->notify = task_quiescent;
    quiescence->context = group->self_link;
  } catch (...) {
    delete observer;
    return OBJC3_SERVICE_OUT_OF_MEMORY;
  }
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
      objc3_task_add_group_observers(task, task_completed, observer,
                                     std::move(quiescence));
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
    notify_group_changed(group);
    if (removed_task) objc3_task_release(removed_task);
    delete observer;
    task_group_release(group);
    return status;
  }
  bool cancel_after_registration = false;
  {
    std::lock_guard<std::mutex> lock(group->mutex);
    --group->registrations_inflight;
    auto found = std::find_if(group->tasks.begin(), group->tasks.end(),
                              [task](const auto &entry) {
                                return entry.task == task;
                              });
    if (group->cancelled && found != group->tasks.end() && !found->completed)
      cancel_after_registration = true;
  }
  if (cancel_after_registration) objc3_task_request_cancel(task);
  notify_group_changed(group);
  return OBJC3_SERVICE_OK;
}

objc3_service_status task_group_close(TaskGroup *group) {
  if (!group) return OBJC3_SERVICE_INVALID_ARGUMENT;
  GroupOperationLease operation(group);
  if (!operation.group) return OBJC3_SERVICE_NOT_READY;
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
  lock.unlock();
  notify_group_changed(group);
  return OBJC3_SERVICE_OK;
}

objc3_service_status task_group_cancel(TaskGroup *group) {
  if (!group) return OBJC3_SERVICE_INVALID_ARGUMENT;
  CancellationBatch cancellations;
  return mark_group_cancel(group, cancellations);
}

objc3_service_status task_group_next_completed(TaskGroup *group,
                                               objc3_task_t **out_task) {
  if (!group || !out_task) return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_task = nullptr;
  GroupOperationLease operation(group);
  if (!operation.group) return OBJC3_SERVICE_NOT_READY;
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
    if (executor_current_identity() != 0) return OBJC3_SERVICE_NOT_READY;
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
  // Completion observers own their association only through publication,
  // whereas join also waits for native frame and callback cleanup. Keep the
  // group record alive throughout that longer quiescence operation.
  GroupOperationLease operation(group);
  if (!operation.group) return OBJC3_SERVICE_NOT_READY;
  objc3_task_t *current = objc3_task_current_callback();
  if (current && contains_task(group, current)) return OBJC3_SERVICE_NOT_READY;

  std::vector<objc3_task_t *> tasks;
  std::vector<TaskGroup *> children;
  objc3_service_status capture_status = OBJC3_SERVICE_OK;
  {
    std::unique_lock<std::mutex> lock(group->mutex);
    if (!group->closed) return OBJC3_SERVICE_NOT_READY;
    if (group->failure_propagation_failed) return OBJC3_SERVICE_NOT_READY;
    if ((group->registrations_inflight != 0 || group->child_closures_inflight != 0) &&
        executor_current_identity() != 0)
      return OBJC3_SERVICE_NOT_READY;
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

namespace {
objc3_service_status await_group(TaskGroup *group, objc3_task_t *parent,
                                 objc3_service_status *out_status, bool cleanup) {
  if (!group || !parent || !out_status || objc3_task_current_callback() != parent)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  std::list<TaskGroup::JoinWaiter> registration;
  try { registration.emplace_back(); }
  catch (...) { return OBJC3_SERVICE_OUT_OF_MEMORY; }
  auto &waiter = registration.front();
  waiter.cleanup = cleanup;
  if (cleanup) {
    waiter.owned_parent = objc3_task_retain(parent);
    if (!waiter.owned_parent) return OBJC3_SERVICE_NOT_READY;
    auto *retained = task_group_retain(group);
    if (!retained) return OBJC3_SERVICE_NOT_READY;
    try {
      // shared_ptr invokes this deleter even if allocating its control block
      // fails. All ownership acquisition precedes suspension publication.
      waiter.cleanup_owner = std::shared_ptr<void>(retained, [](void *opaque) {
        task_group_release(static_cast<TaskGroup *>(opaque));
      });
    } catch (...) { return OBJC3_SERVICE_OUT_OF_MEMORY; }
  }
  bool notify = false;
  {
    std::lock_guard<std::mutex> lock(group->mutex);
    // Closing fixes membership before checking for a cyclic dependency. A
    // concurrent recursive close is safe: child locks serialize membership.
    if (!closed_tree_locked(group)) return OBJC3_SERVICE_NOT_READY;
    for (const auto &entry : group->tasks)
      if (entry.task == parent) return OBJC3_SERVICE_INVALID_ARGUMENT;
    for (auto *child : group->children)
      if (contains_task(child, parent)) return OBJC3_SERVICE_INVALID_ARGUMENT;
    const auto state = cleanup ? cleanup_join_state_locked(group)
                               : join_state_locked(group);
    notify = (!cleanup && objc3_task_is_cancel_requested(parent)) ||
             state != JoinState::Pending;
    if (state == JoinState::Failed) waiter.status = OBJC3_SERVICE_NOT_READY;
    // Nodes, optional ownership and control blocks already exist. Splice and
    // prepare publish exactly one suspension owner without fallible work after.
    const auto status = objc3_task_prepare_await(
        parent, waiter.continuation, out_status, nullptr, cleanup);
    if (status != OBJC3_SERVICE_OK) return status;
    if (!notify)
      group->join_waiters.splice(group->join_waiters.end(), registration);
  }
  // Even an empty or previously joined group queues the continuation. Native
  // resumes remain serialized and keep the caller's selected destination.
  if (notify) dispatch_join_notifications(registration);
  return OBJC3_SERVICE_OK;
}
} // namespace

bool task_group_contains_target(
    TaskGroup *group, const std::shared_ptr<objc3_task::AwaitTarget> &target) {
  if (!group || !target) return false;
  std::lock_guard<std::mutex> lock(group->mutex);
  for (const auto &entry : group->tasks)
    if (entry.task->await_target == target) return true;
  for (auto *child : group->children)
    if (task_group_contains_target(child, target)) return true;
  return false;
}

objc3_service_status task_group_await(TaskGroup *group, objc3_task_t *parent,
                                      objc3_service_status *out_status) {
  return await_group(group, parent, out_status, false);
}

objc3_service_status task_group_await_cleanup(TaskGroup *group,
                                              objc3_task_t *parent,
                                              objc3_service_status *out_status) {
  return await_group(group, parent, out_status, true);
}

objc3_service_status task_group_await_next(
    TaskGroup *group, objc3_task_t *parent,
    const objc3_async_result_type *expected_type,
    objc3_service_status *out_status, objc3_task_t **out_task) {
  if (!group || !parent || !out_status || !out_task || *out_task ||
      objc3_task_current_callback() != parent)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (expected_type) {
    const auto status = objc3_async_result_type_validate(expected_type);
    if (status != OBJC3_SERVICE_OK) return status;
  }
  std::list<TaskGroup::NextWaiter> registration;
  try {
    registration.emplace_back();
    if (expected_type) {
      auto &waiter = registration.front();
      waiter.expected = *expected_type;
      waiter.identity.assign(expected_type->identity, expected_type->identity_size);
      waiter.expected.identity = waiter.identity.c_str();
      waiter.typed = true;
    }
  } catch (...) {
    return OBJC3_SERVICE_OUT_OF_MEMORY;
  }
  std::list<TaskGroup::NextWaiter> ready;
  {
    std::lock_guard<std::mutex> lock(group->mutex);
    if (!closed_tree_locked(group)) return OBJC3_SERVICE_NOT_READY;
    for (const auto &entry : group->tasks)
      if (entry.task == parent) return OBJC3_SERVICE_INVALID_ARGUMENT;
    for (auto *child : group->children)
      if (contains_task(child, parent)) return OBJC3_SERVICE_INVALID_ARGUMENT;
    const auto status = objc3_task_prepare_await(
        parent, registration.front().continuation, out_status, out_task);
    if (status != OBJC3_SERVICE_OK) return status;
    // All descriptor/node allocation precedes suspension publication. Splice
    // and reservation cannot fail after the caller's output slots become live.
    if (objc3_task_is_cancel_requested(parent)) {
      registration.front().claim = objc3_task_claim_await(
          registration.front().continuation, OBJC3_SERVICE_CANCELLED);
      ready.splice(ready.end(), registration);
    } else {
      group->next_waiters.splice(group->next_waiters.end(), registration);
      take_next_notifications_locked(group, ready);
    }
  }
  dispatch_next_notifications(ready);
  return OBJC3_SERVICE_OK;
}

} // namespace objc3::services
