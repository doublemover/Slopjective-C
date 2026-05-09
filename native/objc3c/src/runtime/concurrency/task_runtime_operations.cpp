#include "runtime/concurrency/task_runtime_operations.h"

#include "runtime/concurrency/executor.h"
#include "runtime/concurrency/task.h"
#include "runtime/concurrency/task_state_store.h"

namespace objc3c::runtime {

int SpawnRuntimeTask(RuntimeTaskState &state,
                     int task_kind,
                     int executor_tag) {
  ++state.spawn_call_count;
  state.last_spawn_kind = task_kind;
  state.last_spawn_executor_tag = executor_tag;
  if (!RuntimeTaskKindIsSupported(task_kind) ||
      !RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return 100 + (task_kind * 10) + (executor_tag != 0 ? 1 : 0);
}

int HopRuntimeTaskExecutor(RuntimeTaskState &state,
                           int value,
                           int executor_tag) {
  ++state.executor_hop_call_count;
  state.last_executor_hop_executor_tag = executor_tag;
  state.last_executor_hop_value = value;
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return value;
}

int EnterRuntimeTaskGroupScope(RuntimeTaskState &state, int executor_tag) {
  ++state.scope_call_count;
  state.last_scope_executor_tag = executor_tag;
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return 1;
}

int AddRuntimeTaskGroupTask(RuntimeTaskState &state, int executor_tag) {
  ++state.add_task_call_count;
  state.last_add_task_executor_tag = executor_tag;
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return 1;
}

int WaitRuntimeTaskGroupNext(RuntimeTaskState &state, int executor_tag) {
  ++state.wait_next_call_count;
  state.last_wait_next_executor_tag = executor_tag;
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_wait_next_result = 0;
    return 0;
  }
  state.last_wait_next_result = 23;
  return state.last_wait_next_result;
}

int CancelRuntimeTaskGroup(RuntimeTaskState &state, int executor_tag) {
  ++state.cancel_all_call_count;
  state.last_cancel_all_executor_tag = executor_tag;
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_cancel_all_result = 0;
    return 0;
  }
  state.last_cancel_all_result = 31;
  return state.last_cancel_all_result;
}

int PollRuntimeTaskCancellation(RuntimeTaskState &state, int executor_tag) {
  ++state.cancellation_poll_call_count;
  state.last_cancellation_poll_executor_tag = executor_tag;
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_cancellation_poll_result = 0;
    return 0;
  }
  state.last_cancellation_poll_result = 0;
  return state.last_cancellation_poll_result;
}

int RegisterRuntimeTaskCancellationHandler(RuntimeTaskState &state,
                                           int executor_tag) {
  ++state.on_cancel_call_count;
  state.last_on_cancel_executor_tag = executor_tag;
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return 41;
}

}  // namespace objc3c::runtime
