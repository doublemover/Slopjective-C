#include "runtime/concurrency/task_state.h"

#include "runtime/concurrency/executor.h"
#include "runtime/concurrency/task.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdint>

namespace objc3c::runtime {
namespace detail {

struct RuntimeTaskState {
  std::uint64_t spawn_call_count = 0;
  std::uint64_t scope_call_count = 0;
  std::uint64_t add_task_call_count = 0;
  std::uint64_t wait_next_call_count = 0;
  std::uint64_t cancel_all_call_count = 0;
  std::uint64_t cancellation_poll_call_count = 0;
  std::uint64_t on_cancel_call_count = 0;
  std::uint64_t executor_hop_call_count = 0;
  int last_spawn_kind = 0;
  int last_spawn_executor_tag = 0;
  int last_scope_executor_tag = 0;
  int last_add_task_executor_tag = 0;
  int last_wait_next_executor_tag = 0;
  int last_cancel_all_executor_tag = 0;
  int last_cancellation_poll_executor_tag = 0;
  int last_on_cancel_executor_tag = 0;
  int last_executor_hop_executor_tag = 0;
  int last_executor_hop_value = 0;
  int last_wait_next_result = 0;
  int last_cancel_all_result = 0;
  int last_cancellation_poll_result = 0;
};

RuntimeTaskState &TaskStateForCurrentThread() {
  thread_local RuntimeTaskState state;
  return state;
}

}  // namespace detail

void ResetRuntimeTaskStateForTesting() {
  detail::TaskStateForCurrentThread() = detail::RuntimeTaskState{};
}

}  // namespace objc3c::runtime

extern "C" int objc3_runtime_spawn_task_i32(int task_kind, int executor_tag) {
  objc3c::runtime::detail::RuntimeTaskState &state =
      objc3c::runtime::detail::TaskStateForCurrentThread();
  ++state.spawn_call_count;
  state.last_spawn_kind = task_kind;
  state.last_spawn_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeTaskKindIsSupported(task_kind) ||
      !objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return 100 + (task_kind * 10) + (executor_tag != 0 ? 1 : 0);
}

extern "C" int objc3_runtime_enter_task_group_scope_i32(int executor_tag) {
  objc3c::runtime::detail::RuntimeTaskState &state =
      objc3c::runtime::detail::TaskStateForCurrentThread();
  ++state.scope_call_count;
  state.last_scope_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return 1;
}

extern "C" int objc3_runtime_add_task_group_task_i32(int executor_tag) {
  objc3c::runtime::detail::RuntimeTaskState &state =
      objc3c::runtime::detail::TaskStateForCurrentThread();
  ++state.add_task_call_count;
  state.last_add_task_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return 1;
}

extern "C" int objc3_runtime_wait_task_group_next_i32(int executor_tag) {
  objc3c::runtime::detail::RuntimeTaskState &state =
      objc3c::runtime::detail::TaskStateForCurrentThread();
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
  objc3c::runtime::detail::RuntimeTaskState &state =
      objc3c::runtime::detail::TaskStateForCurrentThread();
  ++state.cancel_all_call_count;
  state.last_cancel_all_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_cancel_all_result = 0;
    return 0;
  }
  state.last_cancel_all_result = 31;
  return state.last_cancel_all_result;
}

extern "C" int objc3_runtime_task_is_cancelled_i32(int executor_tag) {
  objc3c::runtime::detail::RuntimeTaskState &state =
      objc3c::runtime::detail::TaskStateForCurrentThread();
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
  objc3c::runtime::detail::RuntimeTaskState &state =
      objc3c::runtime::detail::TaskStateForCurrentThread();
  ++state.on_cancel_call_count;
  state.last_on_cancel_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return 41;
}

extern "C" int objc3_runtime_executor_hop_i32(int value, int executor_tag) {
  objc3c::runtime::detail::RuntimeTaskState &state =
      objc3c::runtime::detail::TaskStateForCurrentThread();
  ++state.executor_hop_call_count;
  state.last_executor_hop_executor_tag = executor_tag;
  state.last_executor_hop_value = value;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return value;
}

extern "C" int objc3_runtime_copy_task_runtime_state_for_testing(
    objc3_runtime_task_runtime_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  const objc3c::runtime::detail::RuntimeTaskState &state =
      objc3c::runtime::detail::TaskStateForCurrentThread();
  snapshot->spawn_call_count = state.spawn_call_count;
  snapshot->scope_call_count = state.scope_call_count;
  snapshot->add_task_call_count = state.add_task_call_count;
  snapshot->wait_next_call_count = state.wait_next_call_count;
  snapshot->cancel_all_call_count = state.cancel_all_call_count;
  snapshot->cancellation_poll_call_count = state.cancellation_poll_call_count;
  snapshot->on_cancel_call_count = state.on_cancel_call_count;
  snapshot->executor_hop_call_count = state.executor_hop_call_count;
  snapshot->last_spawn_kind = state.last_spawn_kind;
  snapshot->last_spawn_executor_tag = state.last_spawn_executor_tag;
  snapshot->last_scope_executor_tag = state.last_scope_executor_tag;
  snapshot->last_add_task_executor_tag = state.last_add_task_executor_tag;
  snapshot->last_wait_next_executor_tag = state.last_wait_next_executor_tag;
  snapshot->last_cancel_all_executor_tag = state.last_cancel_all_executor_tag;
  snapshot->last_cancellation_poll_executor_tag =
      state.last_cancellation_poll_executor_tag;
  snapshot->last_on_cancel_executor_tag = state.last_on_cancel_executor_tag;
  snapshot->last_executor_hop_executor_tag =
      state.last_executor_hop_executor_tag;
  snapshot->last_executor_hop_value = state.last_executor_hop_value;
  snapshot->last_wait_next_result = state.last_wait_next_result;
  snapshot->last_cancel_all_result = state.last_cancel_all_result;
  snapshot->last_cancellation_poll_result =
      state.last_cancellation_poll_result;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
