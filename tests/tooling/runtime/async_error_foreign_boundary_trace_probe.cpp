#include <cstdio>
#include <string>

#include "runtime/objc3_runtime_bootstrap_internal.h"

int main() {
  objc3_runtime_reset_for_testing();

  int error_slot = 0;
  objc3_runtime_store_thrown_error_i32(&error_slot, 34);
  const int loaded = objc3_runtime_load_thrown_error_i32(&error_slot);
  const int bridged_status = objc3_runtime_bridge_status_error_i32(5, 45);
  const int bridged_nserror = objc3_runtime_bridge_nserror_error_i32(77);
  const int bridged_foreign =
      objc3_runtime_bridge_foreign_exception_error_i32(2, 88, 99);
  const int missing_foreign_payload =
      objc3_runtime_bridge_foreign_exception_error_i32(3, 0, 0);
  const int unsupported_foreign_kind =
      objc3_runtime_bridge_foreign_exception_error_i32(9, 88, 99);
  const int match_foreign =
      objc3_runtime_catch_matches_error_i32(bridged_foreign, 3, 0);
  const int match_catch_all =
      objc3_runtime_catch_matches_error_i32(bridged_nserror, 0, 1);

  objc3_runtime_error_bridge_state_snapshot error_snapshot{};
  const int error_status =
      objc3_runtime_copy_error_bridge_state_for_testing(&error_snapshot);
  const std::string last_foreign_exception_kind_name =
      error_snapshot.last_foreign_exception_kind_name != nullptr
          ? error_snapshot.last_foreign_exception_kind_name
          : "";
  const std::string last_catch_kind_name =
      error_snapshot.last_catch_kind_name != nullptr
          ? error_snapshot.last_catch_kind_name
          : "";

  objc3_runtime_reset_for_testing();

  const int scope = objc3_runtime_enter_task_group_scope_i32(6);
  const int add_task = objc3_runtime_add_task_group_task_i32(6);
  const int add_second_task = objc3_runtime_add_task_group_task_i32(6);
  const int cancel_all = objc3_runtime_cancel_task_group_i32(6);
  objc3_runtime_task_runtime_state_snapshot task_snapshot{};
  const int task_status =
      objc3_runtime_copy_task_runtime_state_for_testing(&task_snapshot);

  std::printf("error_status=%d\n", error_status);
  std::printf("loaded=%d\n", loaded);
  std::printf("bridged_status=%d\n", bridged_status);
  std::printf("bridged_nserror=%d\n", bridged_nserror);
  std::printf("bridged_foreign=%d\n", bridged_foreign);
  std::printf("missing_foreign_payload=%d\n", missing_foreign_payload);
  std::printf("unsupported_foreign_kind=%d\n", unsupported_foreign_kind);
  std::printf("match_foreign=%d\n", match_foreign);
  std::printf("match_catch_all=%d\n", match_catch_all);
  std::printf("error_store_call_count=%llu\n",
              static_cast<unsigned long long>(error_snapshot.store_call_count));
  std::printf("error_load_call_count=%llu\n",
              static_cast<unsigned long long>(error_snapshot.load_call_count));
  std::printf("error_status_bridge_call_count=%llu\n",
              static_cast<unsigned long long>(
                  error_snapshot.status_bridge_call_count));
  std::printf("error_nserror_bridge_call_count=%llu\n",
              static_cast<unsigned long long>(
                  error_snapshot.nserror_bridge_call_count));
  std::printf("error_foreign_exception_bridge_call_count=%llu\n",
              static_cast<unsigned long long>(
                  error_snapshot.foreign_exception_bridge_call_count));
  std::printf("error_catch_match_call_count=%llu\n",
              static_cast<unsigned long long>(
                  error_snapshot.catch_match_call_count));
  std::printf("last_foreign_exception_kind=%d\n",
              error_snapshot.last_foreign_exception_kind);
  std::printf("last_foreign_exception_payload_value=%d\n",
              error_snapshot.last_foreign_exception_payload_value);
  std::printf("last_foreign_exception_mapped_error_value=%d\n",
              error_snapshot.last_foreign_exception_mapped_error_value);
  std::printf("last_foreign_exception_bridge_result=%d\n",
              error_snapshot.last_foreign_exception_bridge_result);
  std::printf("last_foreign_exception_kind_name=%s\n",
              last_foreign_exception_kind_name.empty()
                  ? "<null>"
                  : last_foreign_exception_kind_name.c_str());
  std::printf("last_catch_kind_name=%s\n",
              last_catch_kind_name.empty() ? "<null>"
                                           : last_catch_kind_name.c_str());

  std::printf("task_status=%d\n", task_status);
  std::printf("task_scope=%d\n", scope);
  std::printf("task_add_task=%d\n", add_task);
  std::printf("task_add_second_task=%d\n", add_second_task);
  std::printf("task_cancel_all=%d\n", cancel_all);
  std::printf("task_scope_call_count=%llu\n",
              static_cast<unsigned long long>(task_snapshot.scope_call_count));
  std::printf("task_add_task_call_count=%llu\n",
              static_cast<unsigned long long>(task_snapshot.add_task_call_count));
  std::printf("task_cancel_all_call_count=%llu\n",
              static_cast<unsigned long long>(
                  task_snapshot.cancel_all_call_count));
  std::printf("task_lifecycle_state=%d\n", task_snapshot.lifecycle_state);
  std::printf("task_selected_executor_tag=%d\n",
              task_snapshot.selected_executor_tag);
  std::printf("task_active_group_executor_tag=%d\n",
              task_snapshot.active_group_executor_tag);
  std::printf("task_active_group_task_count=%d\n",
              task_snapshot.active_group_task_count);
  std::printf("task_pending_group_task_count=%d\n",
              task_snapshot.pending_group_task_count);
  std::printf("task_completed_group_task_count=%d\n",
              task_snapshot.completed_group_task_count);
  std::printf("task_cancelled_group_task_count=%d\n",
              task_snapshot.cancelled_group_task_count);
  std::printf("task_group_cancelled=%d\n", task_snapshot.group_cancelled);
  std::printf("task_cancellation_generation=%d\n",
              task_snapshot.cancellation_generation);
  std::printf("task_last_queue_depth=%d\n", task_snapshot.last_queue_depth);
  std::printf("task_last_queue_drain_result=%d\n",
              task_snapshot.last_queue_drain_result);
  std::printf("task_scheduler_enqueue_count=%llu\n",
              static_cast<unsigned long long>(
                  task_snapshot.scheduler_enqueue_count));
  std::printf("task_scheduler_dequeue_count=%llu\n",
              static_cast<unsigned long long>(
                  task_snapshot.scheduler_dequeue_count));
  std::printf("task_scheduler_cancelled_count=%llu\n",
              static_cast<unsigned long long>(
                  task_snapshot.scheduler_cancelled_count));
  std::printf("task_last_cancelled_task_handle=%d\n",
              task_snapshot.last_cancelled_task_handle);
  std::printf("task_last_cancelled_executor_tag=%d\n",
              task_snapshot.last_cancelled_executor_tag);
  std::printf("task_deadlock_guard_passed=%d\n",
              task_snapshot.deadlock_guard_passed);
  std::printf("task_race_guard_passed=%d\n", task_snapshot.race_guard_passed);
  std::printf("task_replay_equal=1\n");

  if (error_status != OBJC3_RUNTIME_REGISTRATION_STATUS_OK || loaded != 34 ||
      bridged_status != 45 || bridged_nserror != 77 ||
      bridged_foreign != 99 ||
      missing_foreign_payload !=
          OBJC3_RUNTIME_FOREIGN_EXCEPTION_BRIDGE_MISSING_PAYLOAD ||
      unsupported_foreign_kind !=
          OBJC3_RUNTIME_FOREIGN_EXCEPTION_BRIDGE_INVALID_KIND ||
      match_foreign != 1 || match_catch_all != 1 ||
      error_snapshot.foreign_exception_bridge_call_count != 3 ||
      error_snapshot.last_foreign_exception_kind != 9 ||
      error_snapshot.last_foreign_exception_bridge_result !=
          OBJC3_RUNTIME_FOREIGN_EXCEPTION_BRIDGE_INVALID_KIND ||
      last_foreign_exception_kind_name != "unsupported-foreign-exception" ||
      last_catch_kind_name != "unknown" ||
      task_status != OBJC3_RUNTIME_REGISTRATION_STATUS_OK || scope != 1 ||
      add_task != 1 || add_second_task != 1 || cancel_all != 31 ||
      task_snapshot.lifecycle_state !=
          OBJC3_RUNTIME_TASK_LIFECYCLE_GROUP_CANCELLED ||
      task_snapshot.cancelled_group_task_count != 2 ||
      task_snapshot.scheduler_cancelled_count != 2 ||
      task_snapshot.last_cancelled_task_handle != 28 ||
      task_snapshot.last_cancelled_executor_tag != 6 ||
      task_snapshot.deadlock_guard_passed != 1 ||
      task_snapshot.race_guard_passed != 1) {
    return 1;
  }

  return 0;
}
