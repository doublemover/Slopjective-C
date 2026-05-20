#include "runtime/concurrency/task_cancellation_state.h"

#include "runtime/concurrency/executor.h"
#include "runtime/concurrency/task_state_store.h"

namespace objc3c::runtime {

namespace {

int RecordTaskCancellationFailure(RuntimeTaskState &state, int reason) {
  state.last_failure_reason = reason;
  return -reason;
}

void RecordTaskCancellationSuccess(RuntimeTaskState &state,
                                   int executor_tag) {
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

int CancelRuntimeTaskGroup(RuntimeTaskState &state, int executor_tag) {
  ++state.cancel_all_call_count;
  state.last_cancel_all_executor_tag = executor_tag;
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_cancel_all_result =
        -kRuntimeTaskFailureInvalidExecutor;
    return RecordTaskCancellationFailure(
        state, kRuntimeTaskFailureInvalidExecutor);
  }
  if (!HasActiveTaskGroup(state)) {
    state.last_cancel_all_result =
        -kRuntimeTaskFailureMissingTaskGroup;
    return RecordTaskCancellationFailure(
        state, kRuntimeTaskFailureMissingTaskGroup);
  }
  if (!TaskGroupExecutorMatches(state, executor_tag)) {
    state.last_cancel_all_result =
        -kRuntimeTaskFailureExecutorMismatch;
    return RecordTaskCancellationFailure(
        state, kRuntimeTaskFailureExecutorMismatch);
  }
  if (state.group_cancelled != 0) {
    state.last_cancel_all_result =
        -kRuntimeTaskFailureTaskGroupAlreadyCancelled;
    return RecordTaskCancellationFailure(
        state, kRuntimeTaskFailureTaskGroupAlreadyCancelled);
  }
  RecordTaskCancellationSuccess(state, executor_tag);
  state.group_cancelled = 1;
  ++state.cancellation_generation;
  state.pending_group_task_count = 0;
  state.executor_ready_queues[executor_tag].clear();
  state.last_queue_depth = 0;
  state.last_executor_queue_depth = 0;
  state.lifecycle_state = kRuntimeTaskLifecycleGroupCancelled;
  state.last_cancel_all_result = 31;
  return state.last_cancel_all_result;
}

int PollRuntimeTaskCancellation(RuntimeTaskState &state, int executor_tag) {
  ++state.cancellation_poll_call_count;
  state.last_cancellation_poll_executor_tag = executor_tag;
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_cancellation_poll_result =
        -kRuntimeTaskFailureInvalidExecutor;
    return RecordTaskCancellationFailure(
        state, kRuntimeTaskFailureInvalidExecutor);
  }
  if (!HasActiveTaskGroup(state)) {
    state.last_cancellation_poll_result =
        -kRuntimeTaskFailureMissingTaskGroup;
    return RecordTaskCancellationFailure(
        state, kRuntimeTaskFailureMissingTaskGroup);
  }
  if (!TaskGroupExecutorMatches(state, executor_tag)) {
    state.last_cancellation_poll_result =
        -kRuntimeTaskFailureExecutorMismatch;
    return RecordTaskCancellationFailure(
        state, kRuntimeTaskFailureExecutorMismatch);
  }
  RecordTaskCancellationSuccess(state, executor_tag);
  state.observed_cancellation_generation = state.cancellation_generation;
  state.last_cancellation_poll_result = state.group_cancelled;
  return state.last_cancellation_poll_result;
}

int RegisterRuntimeTaskCancellationHandler(RuntimeTaskState &state,
                                           int executor_tag) {
  ++state.on_cancel_call_count;
  state.last_on_cancel_executor_tag = executor_tag;
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    return RecordTaskCancellationFailure(
        state, kRuntimeTaskFailureInvalidExecutor);
  }
  if (!HasActiveTaskGroup(state)) {
    return RecordTaskCancellationFailure(
        state, kRuntimeTaskFailureMissingTaskGroup);
  }
  if (!TaskGroupExecutorMatches(state, executor_tag)) {
    return RecordTaskCancellationFailure(
        state, kRuntimeTaskFailureExecutorMismatch);
  }
  RecordTaskCancellationSuccess(state, executor_tag);
  state.observed_cancellation_generation = state.cancellation_generation;
  return 41;
}

}  // namespace objc3c::runtime
