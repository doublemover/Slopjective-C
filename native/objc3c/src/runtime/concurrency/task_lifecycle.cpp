#include "runtime/concurrency/task_state_store.h"

#include "runtime/concurrency/executor.h"
#include "runtime/concurrency/task.h"

extern "C" int objc3_runtime_spawn_task_i32(int task_kind, int executor_tag) {
  objc3c::runtime::RuntimeTaskState &state =
      objc3c::runtime::RuntimeTaskStateForCurrentThread();
  ++state.spawn_call_count;
  state.last_spawn_kind = task_kind;
  state.last_spawn_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeTaskKindIsSupported(task_kind) ||
      !objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return 100 + (task_kind * 10) + (executor_tag != 0 ? 1 : 0);
}

extern "C" int objc3_runtime_executor_hop_i32(int value, int executor_tag) {
  objc3c::runtime::RuntimeTaskState &state =
      objc3c::runtime::RuntimeTaskStateForCurrentThread();
  ++state.executor_hop_call_count;
  state.last_executor_hop_executor_tag = executor_tag;
  state.last_executor_hop_value = value;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return value;
}
