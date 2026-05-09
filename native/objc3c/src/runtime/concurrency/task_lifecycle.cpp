#include "runtime/concurrency/task_state_store.h"

#include "runtime/concurrency/task_runtime_operations.h"

extern "C" int objc3_runtime_spawn_task_i32(int task_kind, int executor_tag) {
  objc3c::runtime::RuntimeTaskState &state =
      objc3c::runtime::RuntimeTaskStateForCurrentThread();
  return objc3c::runtime::SpawnRuntimeTask(state, task_kind, executor_tag);
}

extern "C" int objc3_runtime_executor_hop_i32(int value, int executor_tag) {
  objc3c::runtime::RuntimeTaskState &state =
      objc3c::runtime::RuntimeTaskStateForCurrentThread();
  return objc3c::runtime::HopRuntimeTaskExecutor(state, value, executor_tag);
}
