#include "runtime/concurrency/task_state_store.h"

#include "runtime/concurrency/executor.h"

extern "C" int objc3_runtime_enter_task_group_scope_i32(int executor_tag) {
  objc3c::runtime::RuntimeTaskState &state =
      objc3c::runtime::RuntimeTaskStateForCurrentThread();
  ++state.scope_call_count;
  state.last_scope_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return 1;
}

extern "C" int objc3_runtime_add_task_group_task_i32(int executor_tag) {
  objc3c::runtime::RuntimeTaskState &state =
      objc3c::runtime::RuntimeTaskStateForCurrentThread();
  ++state.add_task_call_count;
  state.last_add_task_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return 1;
}

extern "C" int objc3_runtime_wait_task_group_next_i32(int executor_tag) {
  objc3c::runtime::RuntimeTaskState &state =
      objc3c::runtime::RuntimeTaskStateForCurrentThread();
  ++state.wait_next_call_count;
  state.last_wait_next_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_wait_next_result = 0;
    return 0;
  }
  state.last_wait_next_result = 23;
  return state.last_wait_next_result;
}

extern "C" int objc3_runtime_cancel_task_group_i32(int executor_tag) {
  objc3c::runtime::RuntimeTaskState &state =
      objc3c::runtime::RuntimeTaskStateForCurrentThread();
  ++state.cancel_all_call_count;
  state.last_cancel_all_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_cancel_all_result = 0;
    return 0;
  }
  state.last_cancel_all_result = 31;
  return state.last_cancel_all_result;
}
