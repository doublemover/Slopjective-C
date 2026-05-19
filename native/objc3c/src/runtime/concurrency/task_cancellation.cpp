#include "runtime/concurrency/task_state_store.h"

#include "runtime/concurrency/task_cancellation_state.h"

extern "C" int objc3_runtime_task_is_cancelled_i32(int executor_tag) {
  objc3c::runtime::RuntimeTaskState &state =
      objc3c::runtime::RuntimeTaskStateForCurrentThread();
  return objc3c::runtime::PollRuntimeTaskCancellation(state, executor_tag);
}

extern "C" int objc3_runtime_task_on_cancel_i32(int executor_tag) {
  objc3c::runtime::RuntimeTaskState &state =
      objc3c::runtime::RuntimeTaskStateForCurrentThread();
  return objc3c::runtime::RegisterRuntimeTaskCancellationHandler(
      state, executor_tag);
}
