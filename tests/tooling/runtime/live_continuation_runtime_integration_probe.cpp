#include <cstdint>
#include <iostream>

#include "runtime/objc3_runtime_bootstrap_internal.h"

extern "C" int runTask();
extern "C" int objc3_method_Loader_instance_loadValue();

int main() {
  const int run_task = runTask();
  const int load_value = objc3_method_Loader_instance_loadValue();

  objc3_runtime_async_continuation_state_snapshot snapshot{};
  const int copy_status =
      objc3_runtime_copy_async_continuation_state_for_testing(&snapshot);

  std::cout << "runTask=" << run_task << "\n";
  std::cout << "loadValue=" << load_value << "\n";
  std::cout << "copy_status=" << copy_status << "\n";
  std::cout << "allocation_call_count=" << snapshot.allocation_call_count << "\n";
  std::cout << "handoff_call_count=" << snapshot.handoff_call_count << "\n";
  std::cout << "resume_call_count=" << snapshot.resume_call_count << "\n";
  std::cout << "cancel_call_count=" << snapshot.cancel_call_count << "\n";
  std::cout << "rejected_operation_count=" << snapshot.rejected_operation_count << "\n";
  std::cout << "live_continuation_handle_count=" << snapshot.live_continuation_handle_count << "\n";
  std::cout << "completed_continuation_count=" << snapshot.completed_continuation_count << "\n";
  std::cout << "cancelled_continuation_count=" << snapshot.cancelled_continuation_count << "\n";
  std::cout << "failed_continuation_count=" << snapshot.failed_continuation_count << "\n";
  std::cout << "last_allocated_continuation_slot=" << snapshot.last_allocated_continuation_slot << "\n";
  std::cout << "last_allocated_continuation_generation=" << snapshot.last_allocated_continuation_generation << "\n";
  std::cout << "last_handoff_executor_tag=" << snapshot.last_handoff_executor_tag << "\n";
  std::cout << "last_resume_return_value=" << snapshot.last_resume_return_value << "\n";
  std::cout << "last_operation_failure_code=" << snapshot.last_operation_failure_code << "\n";

  return (run_task == 7 && load_value == 7 && copy_status == 0 &&
          snapshot.allocation_call_count == 4 && snapshot.handoff_call_count == 4 &&
          snapshot.resume_call_count == 4 && snapshot.cancel_call_count == 0 &&
          snapshot.rejected_operation_count == 0 &&
          snapshot.live_continuation_handle_count == 0 &&
          snapshot.completed_continuation_count == 4 &&
          snapshot.cancelled_continuation_count == 0 &&
          snapshot.failed_continuation_count == 0 &&
          snapshot.last_allocated_continuation_slot == 1 &&
          snapshot.last_allocated_continuation_generation == 3 &&
          snapshot.last_operation_failure_code == 0)
             ? 0
             : 1;
}
