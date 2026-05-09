#pragma once

namespace objc3c::runtime {

struct RuntimeTaskState;

int SpawnRuntimeTask(RuntimeTaskState &state,
                     int task_kind,
                     int executor_tag);
int HopRuntimeTaskExecutor(RuntimeTaskState &state,
                           int value,
                           int executor_tag);
int EnterRuntimeTaskGroupScope(RuntimeTaskState &state, int executor_tag);
int AddRuntimeTaskGroupTask(RuntimeTaskState &state, int executor_tag);
int WaitRuntimeTaskGroupNext(RuntimeTaskState &state, int executor_tag);
int CancelRuntimeTaskGroup(RuntimeTaskState &state, int executor_tag);
int PollRuntimeTaskCancellation(RuntimeTaskState &state, int executor_tag);
int RegisterRuntimeTaskCancellationHandler(RuntimeTaskState &state,
                                           int executor_tag);

}  // namespace objc3c::runtime
