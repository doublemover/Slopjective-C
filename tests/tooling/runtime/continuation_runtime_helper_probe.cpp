#include <cstdint>
#include <iostream>

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace {

int CopyContinuationSnapshotFailureCode() {
  objc3_runtime_async_continuation_state_snapshot snapshot{};
  const int copy_status =
      objc3_runtime_copy_async_continuation_state_for_testing(&snapshot);
  return copy_status == 0 ? snapshot.last_operation_failure_code : -1;
}

}  // namespace

int main() {
  const int handle = objc3_runtime_allocate_async_continuation_i32(41, 9);
  const int mismatched_handoff =
      objc3_runtime_handoff_async_continuation_to_executor_i32(handle, 17);
  const int mismatched_handoff_failure_code =
      CopyContinuationSnapshotFailureCode();
  const int handed_off =
      objc3_runtime_handoff_async_continuation_to_executor_i32(handle, 9);
  const int resumed = objc3_runtime_resume_async_continuation_i32(handle, 77);
  const int double_resumed =
      objc3_runtime_resume_async_continuation_i32(handle, 88);
  const int double_resume_failure_code = CopyContinuationSnapshotFailureCode();

  const int unhandedoff_handle =
      objc3_runtime_allocate_async_continuation_i32(42, 9);
  const int unhandedoff_resume =
      objc3_runtime_resume_async_continuation_i32(unhandedoff_handle, 66);
  const int unhandedoff_resume_failure_code =
      CopyContinuationSnapshotFailureCode();
  const int unhandedoff_cancel =
      objc3_runtime_cancel_async_continuation_i32(unhandedoff_handle);

  const int missing_payload_handle =
      objc3_runtime_allocate_async_continuation_i32(43, 9);
  const int missing_payload_handed_off =
      objc3_runtime_handoff_async_continuation_to_executor_i32(
          missing_payload_handle, 9);
  const int missing_payload_resumed =
      objc3_runtime_resume_async_continuation_i32(missing_payload_handle, 0);
  const int missing_payload_failure_code =
      CopyContinuationSnapshotFailureCode();

  const int cancelled_handle =
      objc3_runtime_allocate_async_continuation_i32(44, 9);
  const int cancelled =
      objc3_runtime_cancel_async_continuation_i32(cancelled_handle);
  const int resume_after_cancel =
      objc3_runtime_resume_async_continuation_i32(cancelled_handle, 99);
  const int resume_after_cancel_failure_code =
      CopyContinuationSnapshotFailureCode();

  const int recycled_handle =
      objc3_runtime_allocate_async_continuation_i32(45, 9);
  const int stale_generation_resume =
      objc3_runtime_resume_async_continuation_i32(cancelled_handle, 100);
  const int stale_generation_failure_code =
      CopyContinuationSnapshotFailureCode();

  const int malformed_resume =
      objc3_runtime_resume_async_continuation_i32(-1, 101);
  const int malformed_failure_code = CopyContinuationSnapshotFailureCode();
  const int unknown_resume =
      objc3_runtime_resume_async_continuation_i32(2, 102);
  const int unknown_failure_code = CopyContinuationSnapshotFailureCode();
  const int invalid_allocation =
      objc3_runtime_allocate_async_continuation_i32(0, 9);
  const int invalid_allocation_failure_code =
      CopyContinuationSnapshotFailureCode();

  objc3_runtime_async_continuation_state_snapshot snapshot{};
  const int copy_status =
      objc3_runtime_copy_async_continuation_state_for_testing(&snapshot);

  std::cout << "handle=" << handle << "\n";
  std::cout << "mismatched_handoff=" << mismatched_handoff << "\n";
  std::cout << "mismatched_handoff_failure_code="
            << mismatched_handoff_failure_code << "\n";
  std::cout << "handed_off=" << handed_off << "\n";
  std::cout << "resumed=" << resumed << "\n";
  std::cout << "double_resumed=" << double_resumed << "\n";
  std::cout << "double_resume_failure_code="
            << double_resume_failure_code << "\n";
  std::cout << "unhandedoff_handle=" << unhandedoff_handle << "\n";
  std::cout << "unhandedoff_resume=" << unhandedoff_resume << "\n";
  std::cout << "unhandedoff_resume_failure_code="
            << unhandedoff_resume_failure_code << "\n";
  std::cout << "unhandedoff_cancel=" << unhandedoff_cancel << "\n";
  std::cout << "missing_payload_handle=" << missing_payload_handle << "\n";
  std::cout << "missing_payload_handed_off="
            << missing_payload_handed_off << "\n";
  std::cout << "missing_payload_resumed=" << missing_payload_resumed << "\n";
  std::cout << "missing_payload_failure_code="
            << missing_payload_failure_code << "\n";
  std::cout << "cancelled_handle=" << cancelled_handle << "\n";
  std::cout << "cancelled=" << cancelled << "\n";
  std::cout << "resume_after_cancel=" << resume_after_cancel << "\n";
  std::cout << "resume_after_cancel_failure_code="
            << resume_after_cancel_failure_code << "\n";
  std::cout << "recycled_handle=" << recycled_handle << "\n";
  std::cout << "stale_generation_resume=" << stale_generation_resume << "\n";
  std::cout << "stale_generation_failure_code="
            << stale_generation_failure_code << "\n";
  std::cout << "malformed_resume=" << malformed_resume << "\n";
  std::cout << "malformed_failure_code=" << malformed_failure_code << "\n";
  std::cout << "unknown_resume=" << unknown_resume << "\n";
  std::cout << "unknown_failure_code=" << unknown_failure_code << "\n";
  std::cout << "invalid_allocation=" << invalid_allocation << "\n";
  std::cout << "invalid_allocation_failure_code="
            << invalid_allocation_failure_code << "\n";
  std::cout << "copy_status=" << copy_status << "\n";
  std::cout << "allocation_call_count=" << snapshot.allocation_call_count
            << "\n";
  std::cout << "handoff_call_count=" << snapshot.handoff_call_count << "\n";
  std::cout << "resume_call_count=" << snapshot.resume_call_count << "\n";
  std::cout << "cancel_call_count=" << snapshot.cancel_call_count << "\n";
  std::cout << "rejected_operation_count="
            << snapshot.rejected_operation_count << "\n";
  std::cout << "live_continuation_handle_count="
            << snapshot.live_continuation_handle_count << "\n";
  std::cout << "completed_continuation_count="
            << snapshot.completed_continuation_count << "\n";
  std::cout << "cancelled_continuation_count="
            << snapshot.cancelled_continuation_count << "\n";
  std::cout << "failed_continuation_count="
            << snapshot.failed_continuation_count << "\n";
  std::cout << "last_allocated_continuation_handle="
            << snapshot.last_allocated_continuation_handle << "\n";
  std::cout << "last_allocated_continuation_slot="
            << snapshot.last_allocated_continuation_slot << "\n";
  std::cout << "last_allocated_continuation_generation="
            << snapshot.last_allocated_continuation_generation << "\n";
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
  std::cout << "last_cancel_continuation_handle="
            << snapshot.last_cancel_continuation_handle << "\n";
  std::cout << "last_cancel_return_value="
            << snapshot.last_cancel_return_value << "\n";
  std::cout << "last_operation_failure_code="
            << snapshot.last_operation_failure_code << "\n";
  std::cout << "last_observed_continuation_slot="
            << snapshot.last_observed_continuation_slot << "\n";
  std::cout << "last_observed_continuation_generation="
            << snapshot.last_observed_continuation_generation << "\n";

  return (handle == 1 && mismatched_handoff == 0 &&
          mismatched_handoff_failure_code == 12 && handed_off == 1 &&
          resumed == 77 && double_resumed == 0 &&
          double_resume_failure_code == 4 && unhandedoff_handle == 65537 &&
          unhandedoff_resume == 0 && unhandedoff_resume_failure_code == 11 &&
          unhandedoff_cancel == 65537 && missing_payload_handle == 131073 &&
          missing_payload_handed_off == 131073 &&
          missing_payload_resumed == 0 && missing_payload_failure_code == 6 &&
          cancelled_handle == 196609 && cancelled == 196609 &&
          resume_after_cancel == 0 && resume_after_cancel_failure_code == 5 &&
          recycled_handle == 262145 && stale_generation_resume == 0 &&
          stale_generation_failure_code == 3 && malformed_resume == 0 &&
          malformed_failure_code == 1 && unknown_resume == 0 &&
          unknown_failure_code == 2 && invalid_allocation == 0 &&
          invalid_allocation_failure_code == 7 && copy_status == 0 &&
          snapshot.allocation_call_count == 6 &&
          snapshot.handoff_call_count == 3 && snapshot.resume_call_count == 8 &&
          snapshot.cancel_call_count == 2 &&
          snapshot.rejected_operation_count == 9 &&
          snapshot.live_continuation_handle_count == 1 &&
          snapshot.completed_continuation_count == 1 &&
          snapshot.cancelled_continuation_count == 2 &&
          snapshot.failed_continuation_count == 1 &&
          snapshot.last_allocated_continuation_handle == 0 &&
          snapshot.last_allocated_continuation_slot == 0 &&
          snapshot.last_allocated_continuation_generation == 0 &&
          snapshot.last_operation_failure_code == 7)
             ? 0
             : 1;
}
