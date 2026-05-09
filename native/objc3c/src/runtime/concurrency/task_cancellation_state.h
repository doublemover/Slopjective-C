#pragma once

namespace objc3c::runtime {

struct RuntimeTaskState;

int CancelRuntimeTaskGroup(RuntimeTaskState &state, int executor_tag);
int PollRuntimeTaskCancellation(RuntimeTaskState &state, int executor_tag);
int RegisterRuntimeTaskCancellationHandler(RuntimeTaskState &state,
                                           int executor_tag);

}  // namespace objc3c::runtime
