#include "runtime/concurrency/task_group_operations.h"

#include "runtime/concurrency/executor.h"
#include "runtime/concurrency/task_state_store.h"

namespace objc3c::runtime {

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

}  // namespace objc3c::runtime
