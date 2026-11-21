#ifndef TASK_COROUTINE_PROVIDER_H
#define TASK_COROUTINE_PROVIDER_H
#include "objc3_task.hpp"
#include <atomic>
#include <memory>
#include <string>

struct coroutine_state {
  std::atomic<bool> ready{false};
  std::atomic<unsigned> entered{0};
  std::atomic<unsigned> destroyed{0};
  std::atomic<unsigned> continued{0};
};
objc3::task_handle<int> start_leaf(objc3_executor_t *,
                                   std::shared_ptr<coroutine_state>);
objc3::task_handle<int> start_parent(objc3_executor_t *,
                                     objc3::task_handle<int>,
                                     std::shared_ptr<coroutine_state>);
objc3::task_handle<int> start_chain(objc3_executor_t *, unsigned);
objc3::task_handle<int> start_group_join(
    objc3_executor_t *, objc3::task_group_handle,
    std::shared_ptr<coroutine_state>);
objc3::task_handle<int> start_group_next(
    objc3_executor_t *, objc3::task_group_handle,
    std::shared_ptr<coroutine_state>);
objc3::task_handle<int> start_owned_child(objc3_executor_t *,
                                         std::shared_ptr<coroutine_state>);
objc3::task_handle<std::string> start_failure(objc3_executor_t *);
objc3::task_handle<void> start_foreign_error(objc3_executor_t *);
objc3::task_handle<int> start_actor_hops(
    objc3_actor_t *, objc3_actor_t *, objc3_executor_t *,
    std::shared_ptr<coroutine_state>, unsigned);
#endif
