#include "runtime/concurrency/task_state_store.h"

#include "runtime/objc3_runtime_bootstrap_internal.h"

extern "C" int objc3_runtime_copy_task_runtime_state_for_testing(
    objc3_runtime_task_runtime_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  const objc3c::runtime::RuntimeTaskState &state =
      objc3c::runtime::RuntimeTaskStateForCurrentThread();
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
