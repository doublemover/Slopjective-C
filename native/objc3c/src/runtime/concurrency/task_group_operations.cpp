#include "runtime/concurrency/task_group_operations.h"

#include "runtime/concurrency/executor.h"
#include "runtime/concurrency/task_state_store.h"

namespace objc3c::runtime {

namespace {

int RecordTaskGroupFailure(RuntimeTaskState &state, int reason) {
  state.last_failure_reason = reason;
  return -reason;
}

void RecordTaskGroupSuccess(RuntimeTaskState &state, int executor_tag) {
  state.last_failure_reason = kRuntimeTaskFailureNone;
  state.selected_executor_tag = executor_tag;
}

bool HasActiveTaskGroup(const RuntimeTaskState &state) {
  return state.active_group_executor_tag >= 0;
}

bool TaskGroupExecutorMatches(const RuntimeTaskState &state,
                              int executor_tag) {
  return state.active_group_executor_tag == executor_tag;
}

}  // namespace

int EnterRuntimeTaskGroupScope(RuntimeTaskState &state, int executor_tag) {
  ++state.scope_call_count;
  state.last_scope_executor_tag = executor_tag;
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    return RecordTaskGroupFailure(state,
                                  kRuntimeTaskFailureInvalidExecutor);
  }
  if (HasActiveTaskGroup(state) && state.pending_group_task_count > 0) {
    return RecordTaskGroupFailure(
        state, kRuntimeTaskFailureTaskGroupAlreadyActive);
  }
  RecordTaskGroupSuccess(state, executor_tag);
  state.active_group_executor_tag = executor_tag;
  state.active_group_task_count = 0;
  state.pending_group_task_count = 0;
  state.completed_group_task_count = 0;
  state.cancelled_group_task_count = 0;
  state.group_cancelled = 0;
  state.observed_cancellation_generation = state.cancellation_generation;
  state.last_queue_depth = 0;
  state.last_queue_drain_result = 0;
  state.lifecycle_state = kRuntimeTaskLifecycleGroupActive;
  return 1;
}

int AddRuntimeTaskGroupTask(RuntimeTaskState &state, int executor_tag) {
  ++state.add_task_call_count;
  state.last_add_task_executor_tag = executor_tag;
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    return RecordTaskGroupFailure(state,
                                  kRuntimeTaskFailureInvalidExecutor);
  }
  if (!HasActiveTaskGroup(state)) {
    return RecordTaskGroupFailure(state,
                                  kRuntimeTaskFailureMissingTaskGroup);
  }
  if (!TaskGroupExecutorMatches(state, executor_tag)) {
    return RecordTaskGroupFailure(state,
                                  kRuntimeTaskFailureExecutorMismatch);
  }
  if (state.group_cancelled != 0) {
    return RecordTaskGroupFailure(
        state, kRuntimeTaskFailureTaskGroupAlreadyCancelled);
  }
  RecordTaskGroupSuccess(state, executor_tag);
  ++state.active_group_task_count;
  ++state.pending_group_task_count;
  const int scheduled_result =
      20 + executor_tag + state.active_group_task_count;
  RecordRuntimeTaskSchedulerEnqueue(state, executor_tag, scheduled_result);
  state.last_queue_depth = state.pending_group_task_count;
  state.lifecycle_state = kRuntimeTaskLifecycleGroupActive;
  return 1;
}

int WaitRuntimeTaskGroupNext(RuntimeTaskState &state, int executor_tag) {
  ++state.wait_next_call_count;
  state.last_wait_next_executor_tag = executor_tag;
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_wait_next_result =
        -kRuntimeTaskFailureInvalidExecutor;
    return RecordTaskGroupFailure(state,
                                  kRuntimeTaskFailureInvalidExecutor);
  }
  if (!HasActiveTaskGroup(state)) {
    state.last_wait_next_result =
        -kRuntimeTaskFailureMissingTaskGroup;
    return RecordTaskGroupFailure(state,
                                  kRuntimeTaskFailureMissingTaskGroup);
  }
  if (!TaskGroupExecutorMatches(state, executor_tag)) {
    state.last_wait_next_result =
        -kRuntimeTaskFailureExecutorMismatch;
    return RecordTaskGroupFailure(state,
                                  kRuntimeTaskFailureExecutorMismatch);
  }
  if (state.group_cancelled != 0) {
    state.last_wait_next_result =
        -kRuntimeTaskFailureTaskGroupAlreadyCancelled;
    return RecordTaskGroupFailure(
        state, kRuntimeTaskFailureTaskGroupAlreadyCancelled);
  }
  if (state.pending_group_task_count <= 0) {
    state.last_wait_next_result =
        -kRuntimeTaskFailureEmptyTaskGroupQueue;
    return RecordTaskGroupFailure(
        state, kRuntimeTaskFailureEmptyTaskGroupQueue);
  }
  state.last_wait_next_result = DrainRuntimeTaskSchedulerQueue(state,
                                                               executor_tag);
  if (state.last_wait_next_result <= 0) {
    state.last_wait_next_result =
        -kRuntimeTaskFailureEmptyTaskGroupQueue;
    return RecordTaskGroupFailure(
        state, kRuntimeTaskFailureEmptyTaskGroupQueue);
  }
  --state.pending_group_task_count;
  ++state.completed_group_task_count;
  state.last_queue_depth = state.pending_group_task_count;
  state.last_queue_drain_result = state.last_wait_next_result;
  if (state.pending_group_task_count == 0) {
    state.lifecycle_state = kRuntimeTaskLifecycleGroupDrained;
  }
  RecordTaskGroupSuccess(state, executor_tag);
  return state.last_wait_next_result;
}

}  // namespace objc3c::runtime
