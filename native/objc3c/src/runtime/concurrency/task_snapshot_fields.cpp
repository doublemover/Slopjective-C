#include "runtime/concurrency/task_snapshot_fields.h"

#include "runtime/concurrency/task_state_store.h"

namespace objc3c::runtime {

void ResetRuntimeTaskRuntimeStateSnapshot(
    objc3_runtime_task_runtime_state_snapshot &snapshot) {
  snapshot.spawn_call_count = 0;
  snapshot.scope_call_count = 0;
  snapshot.add_task_call_count = 0;
  snapshot.wait_next_call_count = 0;
  snapshot.cancel_all_call_count = 0;
  snapshot.cancellation_poll_call_count = 0;
  snapshot.on_cancel_call_count = 0;
  snapshot.executor_hop_call_count = 0;
  snapshot.last_spawn_kind = 0;
  snapshot.last_spawn_executor_tag = 0;
  snapshot.last_scope_executor_tag = 0;
  snapshot.last_add_task_executor_tag = 0;
  snapshot.last_wait_next_executor_tag = 0;
  snapshot.last_cancel_all_executor_tag = 0;
  snapshot.last_cancellation_poll_executor_tag = 0;
  snapshot.last_on_cancel_executor_tag = 0;
  snapshot.last_executor_hop_executor_tag = 0;
  snapshot.last_executor_hop_value = 0;
  snapshot.last_wait_next_result = 0;
  snapshot.last_cancel_all_result = 0;
  snapshot.last_cancellation_poll_result = 0;
}

void PopulateRuntimeTaskRuntimeStateSnapshot(
    const RuntimeTaskState &state,
    objc3_runtime_task_runtime_state_snapshot &snapshot) {
  snapshot.spawn_call_count = state.spawn_call_count;
  snapshot.scope_call_count = state.scope_call_count;
  snapshot.add_task_call_count = state.add_task_call_count;
  snapshot.wait_next_call_count = state.wait_next_call_count;
  snapshot.cancel_all_call_count = state.cancel_all_call_count;
  snapshot.cancellation_poll_call_count = state.cancellation_poll_call_count;
  snapshot.on_cancel_call_count = state.on_cancel_call_count;
  snapshot.executor_hop_call_count = state.executor_hop_call_count;
  snapshot.last_spawn_kind = state.last_spawn_kind;
  snapshot.last_spawn_executor_tag = state.last_spawn_executor_tag;
  snapshot.last_scope_executor_tag = state.last_scope_executor_tag;
  snapshot.last_add_task_executor_tag = state.last_add_task_executor_tag;
  snapshot.last_wait_next_executor_tag = state.last_wait_next_executor_tag;
  snapshot.last_cancel_all_executor_tag = state.last_cancel_all_executor_tag;
  snapshot.last_cancellation_poll_executor_tag =
      state.last_cancellation_poll_executor_tag;
  snapshot.last_on_cancel_executor_tag = state.last_on_cancel_executor_tag;
  snapshot.last_executor_hop_executor_tag =
      state.last_executor_hop_executor_tag;
  snapshot.last_executor_hop_value = state.last_executor_hop_value;
  snapshot.last_wait_next_result = state.last_wait_next_result;
  snapshot.last_cancel_all_result = state.last_cancel_all_result;
  snapshot.last_cancellation_poll_result =
      state.last_cancellation_poll_result;
}

}  // namespace objc3c::runtime
