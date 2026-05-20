#include "runtime/concurrency/task_runtime_operations.h"

#include "runtime/concurrency/executor.h"
#include "runtime/concurrency/task.h"
#include "runtime/concurrency/task_state_store.h"

namespace objc3c::runtime {

namespace {

int RecordRuntimeTaskFailure(RuntimeTaskState &state, int reason) {
  state.last_failure_reason = reason;
  return -reason;
}

void RecordRuntimeTaskSuccess(RuntimeTaskState &state, int executor_tag) {
  state.last_failure_reason = kRuntimeTaskFailureNone;
  state.selected_executor_tag = executor_tag;
}

}  // namespace

int SpawnRuntimeTask(RuntimeTaskState &state,
                     int task_kind,
                     int executor_tag) {
  ++state.spawn_call_count;
  state.last_spawn_kind = task_kind;
  state.last_spawn_executor_tag = executor_tag;
  if (!RuntimeTaskKindIsSupported(task_kind)) {
    return RecordRuntimeTaskFailure(
        state, kRuntimeTaskFailureUnsupportedTaskKind);
  }
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    return RecordRuntimeTaskFailure(state,
                                    kRuntimeTaskFailureInvalidExecutor);
  }
  RecordRuntimeTaskSuccess(state, executor_tag);
  state.lifecycle_state = kRuntimeTaskLifecycleTaskSpawned;
  const int task_handle = 100 + (task_kind * 10) + (executor_tag != 0 ? 1 : 0);
  if (task_kind == 2) {
    RecordRuntimeTaskSchedulerEnqueue(state, executor_tag, task_handle);
  }
  return task_handle;
}

int HopRuntimeTaskExecutor(RuntimeTaskState &state,
                           int value,
                           int executor_tag) {
  ++state.executor_hop_call_count;
  state.last_executor_hop_executor_tag = executor_tag;
  state.last_executor_hop_value = value;
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    return RecordRuntimeTaskFailure(state,
                                    kRuntimeTaskFailureInvalidExecutor);
  }
  if (value <= 0) {
    return RecordRuntimeTaskFailure(
        state, kRuntimeTaskFailureEmptyTaskGroupQueue);
  }
  if (value != state.last_dequeued_task_handle ||
      executor_tag != state.last_dequeued_executor_tag) {
    state.race_guard_passed = 0;
    return RecordRuntimeTaskFailure(
        state, kRuntimeTaskFailureSchedulerQueueDrift);
  }
  state.race_guard_passed = 1;
  RecordRuntimeTaskSuccess(state, executor_tag);
  return value;
}

}  // namespace objc3c::runtime
