#pragma once

namespace objc3c::runtime {

struct RuntimeTaskState;

int EnterRuntimeTaskGroupScope(RuntimeTaskState &state, int executor_tag);
int AddRuntimeTaskGroupTask(RuntimeTaskState &state, int executor_tag);
int WaitRuntimeTaskGroupNext(RuntimeTaskState &state, int executor_tag);

}  // namespace objc3c::runtime
