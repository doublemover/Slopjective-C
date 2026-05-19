#include "runtime/concurrency/task_state_store.h"

#include "runtime/concurrency/task_cancellation_state.h"
#include "runtime/concurrency/task_group_operations.h"

extern "C" int objc3_runtime_enter_task_group_scope_i32(int executor_tag) {
  objc3c::runtime::RuntimeTaskState &state =
      objc3c::runtime::RuntimeTaskStateForCurrentThread();
  return objc3c::runtime::EnterRuntimeTaskGroupScope(state, executor_tag);
}

extern "C" int objc3_runtime_add_task_group_task_i32(int executor_tag) {
  objc3c::runtime::RuntimeTaskState &state =
      objc3c::runtime::RuntimeTaskStateForCurrentThread();
  return objc3c::runtime::AddRuntimeTaskGroupTask(state, executor_tag);
}

extern "C" int objc3_runtime_wait_task_group_next_i32(int executor_tag) {
  objc3c::runtime::RuntimeTaskState &state =
      objc3c::runtime::RuntimeTaskStateForCurrentThread();
  return objc3c::runtime::WaitRuntimeTaskGroupNext(state, executor_tag);
}

extern "C" int objc3_runtime_cancel_task_group_i32(int executor_tag) {
  objc3c::runtime::RuntimeTaskState &state =
      objc3c::runtime::RuntimeTaskStateForCurrentThread();
  return objc3c::runtime::CancelRuntimeTaskGroup(state, executor_tag);
}
