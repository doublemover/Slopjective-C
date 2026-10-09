#ifndef OBJC3_RUNTIME_SERVICE_GROUP_H
#define OBJC3_RUNTIME_SERVICE_GROUP_H

#include "service_internal.h"

namespace objc3::services {

struct TaskGroup;

objc3_service_status task_group_create(TaskGroup *parent, TaskGroup **out_group);
TaskGroup *task_group_retain(TaskGroup *group);
void task_group_release(TaskGroup *group);
objc3_service_status task_group_add_task(TaskGroup *group, objc3_task_t *task);
objc3_service_status task_group_close(TaskGroup *group);
objc3_service_status task_group_cancel(TaskGroup *group);
objc3_service_status task_group_next_completed(TaskGroup *group,
                                              objc3_task_t **out_task);
objc3_service_status task_group_join(TaskGroup *group);

} // namespace objc3::services

#endif
