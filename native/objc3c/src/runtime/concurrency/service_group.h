#ifndef OBJC3_RUNTIME_SERVICE_GROUP_H
#define OBJC3_RUNTIME_SERVICE_GROUP_H

#include "service_internal.h"

#include <cstdint>

namespace objc3::services {

struct TaskGroup;

objc3_service_status task_group_create(TaskGroup *parent, TaskGroup **out_group);
TaskGroup *task_group_retain(TaskGroup *group);
void task_group_release(TaskGroup *group);
// Token identity avoids retaining an awaiting ancestor or reusing its address.
bool task_group_contains_target(
    TaskGroup *group, const std::shared_ptr<objc3_task::AwaitTarget> &target);
objc3_service_status task_group_add_task(TaskGroup *group, objc3_task_t *task,
                                        bool scope_body = false);
objc3_service_status task_group_close(TaskGroup *group);
objc3_service_status task_group_cancel(TaskGroup *group);
objc3_service_status task_group_next_completed(TaskGroup *group,
                                              objc3_task_t **out_task);
objc3_service_status task_group_try_next_completed(TaskGroup *group,
                                                  objc3_task_t **out_task);
objc3_service_status task_group_join(TaskGroup *group);
objc3_service_status task_group_await(TaskGroup *group, objc3_task_t *parent,
                                      objc3_service_status *out_status);
objc3_service_status task_group_await_cleanup(TaskGroup *group,
                                              objc3_task_t *parent,
                                              objc3_service_status *out_status);
objc3_service_status task_group_scope_enter(objc3_task_t *parent);
TaskGroup *task_group_scope_current(objc3_task_t *parent);
objc3_service_status task_group_scope_finish(objc3_task_t *parent, bool cancel,
                                             objc3_service_status *out_status);
// Forced continuation loss must finish scope work before reclaiming the frame.
bool task_group_scope_abort(objc3_task_t *parent) noexcept;
objc3_service_status task_group_await_next(
    TaskGroup *group, objc3_task_t *parent,
    const objc3_async_result_type *expected_type,
    objc3_service_status *out_status, objc3_task_t **out_task);

#if defined(OBJC3_RUNTIME_SERVICE_TESTING)
using TaskGroupFailurePropagationHook = void (*)(void *context);
void task_group_test_set_failure_propagation_hook(
    TaskGroupFailurePropagationHook hook, void *context);
objc3_service_status task_group_test_set_references(TaskGroup *group,
                                                    uint32_t references);
#endif

} // namespace objc3::services

#endif
