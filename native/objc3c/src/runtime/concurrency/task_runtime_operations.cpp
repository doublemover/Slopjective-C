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

}  // namespace objc3c::runtime
