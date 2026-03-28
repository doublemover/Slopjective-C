#include <cstdint>
#include <iostream>

#include "runtime/objc3_runtime_bootstrap_internal.h"

int main() {
  const int handle = objc3_runtime_allocate_async_continuation_i32(41, 9);
  const int handed_off =
      objc3_runtime_handoff_async_continuation_to_executor_i32(handle, 17);
  const int resumed = objc3_runtime_resume_async_continuation_i32(handle, 77);

  objc3_runtime_async_continuation_state_snapshot snapshot{};
  const int copy_status =
      objc3_runtime_copy_async_continuation_state_for_testing(&snapshot);

  std::cout << "handle=" << handle << "\n";
  std::cout << "handed_off=" << handed_off << "\n";
  std::cout << "resumed=" << resumed << "\n";
  std::cout << "copy_status=" << copy_status << "\n";
  std::cout << "allocation_call_count=" << snapshot.allocation_call_count
            << "\n";
  std::cout << "handoff_call_count=" << snapshot.handoff_call_count << "\n";
  std::cout << "resume_call_count=" << snapshot.resume_call_count << "\n";
  std::cout << "live_continuation_handle_count="
            << snapshot.live_continuation_handle_count << "\n";
  std::cout << "last_allocated_continuation_handle="
            << snapshot.last_allocated_continuation_handle << "\n";
  std::cout << "last_allocated_resume_entry_tag="
            << snapshot.last_allocated_resume_entry_tag << "\n";
  std::cout << "last_allocated_executor_tag="
            << snapshot.last_allocated_executor_tag << "\n";
  std::cout << "last_handoff_continuation_handle="
            << snapshot.last_handoff_continuation_handle << "\n";
  std::cout << "last_handoff_executor_tag="
            << snapshot.last_handoff_executor_tag << "\n";
  std::cout << "last_resume_continuation_handle="
            << snapshot.last_resume_continuation_handle << "\n";
  std::cout << "last_resume_result_value="
            << snapshot.last_resume_result_value << "\n";
  std::cout << "last_resume_return_value="
            << snapshot.last_resume_return_value << "\n";

  return (handle == 1 && handed_off == 1 && resumed == 77 && copy_status == 0)
             ? 0
             : 1;
}
