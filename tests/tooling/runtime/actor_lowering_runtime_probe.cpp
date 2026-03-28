#include <iostream>

#include "runtime/objc3_runtime_bootstrap_internal.h"

int main() {
  objc3_runtime_reset_for_testing();

  const int isolation = objc3_runtime_actor_enter_isolation_thunk_i32(1);
  const int nonisolated = objc3_runtime_actor_enter_nonisolated_i32(7, 0);
  const int hopped = objc3_runtime_actor_hop_to_executor_i32(9, 1);

  objc3_runtime_actor_runtime_state_snapshot snapshot{};
  const int copy_status = objc3_runtime_copy_actor_runtime_state_for_testing(&snapshot);

  std::cout << "copy_status=" << copy_status << "\n";
  std::cout << "isolation=" << isolation << "\n";
  std::cout << "nonisolated=" << nonisolated << "\n";
  std::cout << "hopped=" << hopped << "\n";
  std::cout << "isolation_thunk_call_count=" << snapshot.isolation_thunk_call_count << "\n";
  std::cout << "nonisolated_entry_call_count=" << snapshot.nonisolated_entry_call_count << "\n";
  std::cout << "hop_to_executor_call_count=" << snapshot.hop_to_executor_call_count << "\n";
  std::cout << "last_isolation_executor_tag=" << snapshot.last_isolation_executor_tag << "\n";
  std::cout << "last_nonisolated_value=" << snapshot.last_nonisolated_value << "\n";
  std::cout << "last_nonisolated_executor_tag=" << snapshot.last_nonisolated_executor_tag << "\n";
  std::cout << "last_hop_value=" << snapshot.last_hop_value << "\n";
  std::cout << "last_hop_executor_tag=" << snapshot.last_hop_executor_tag << "\n";
  std::cout << "last_hop_result=" << snapshot.last_hop_result << "\n";

  return (copy_status == 0 && isolation == 1 && nonisolated == 7 && hopped == 9 &&
          snapshot.isolation_thunk_call_count == 1 &&
          snapshot.nonisolated_entry_call_count == 1 &&
          snapshot.hop_to_executor_call_count == 1 &&
          snapshot.last_isolation_executor_tag == 1 &&
          snapshot.last_nonisolated_value == 7 &&
          snapshot.last_nonisolated_executor_tag == 0 &&
          snapshot.last_hop_value == 9 &&
          snapshot.last_hop_executor_tag == 1 &&
          snapshot.last_hop_result == 9)
             ? 0
             : 1;
}
