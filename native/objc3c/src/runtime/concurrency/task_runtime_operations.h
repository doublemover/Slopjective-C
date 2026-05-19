#pragma once

namespace objc3c::runtime {

struct RuntimeTaskState;

int SpawnRuntimeTask(RuntimeTaskState &state,
                     int task_kind,
                     int executor_tag);
int HopRuntimeTaskExecutor(RuntimeTaskState &state,
                           int value,
                           int executor_tag);

}  // namespace objc3c::runtime
