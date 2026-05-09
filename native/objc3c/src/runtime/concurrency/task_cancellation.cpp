#include "runtime/concurrency/task_state_store.h"

#include "runtime/concurrency/executor.h"

extern "C" int objc3_runtime_task_is_cancelled_i32(int executor_tag) {
  objc3c::runtime::RuntimeTaskState &state =
      objc3c::runtime::RuntimeTaskStateForCurrentThread();
  ++state.cancellation_poll_call_count;
  state.last_cancellation_poll_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_cancellation_poll_result = 0;
    return 0;
  }
  state.last_cancellation_poll_result = 0;
  return state.last_cancellation_poll_result;
}

extern "C" int objc3_runtime_task_on_cancel_i32(int executor_tag) {
  objc3c::runtime::RuntimeTaskState &state =
      objc3c::runtime::RuntimeTaskStateForCurrentThread();
  ++state.on_cancel_call_count;
  state.last_on_cancel_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return 41;
}
