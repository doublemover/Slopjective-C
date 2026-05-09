#include "runtime/concurrency/task_cancellation_state.h"

#include "runtime/concurrency/executor.h"
#include "runtime/concurrency/task_state_store.h"

namespace objc3c::runtime {

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
