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
  std::cout << "live_continuation_handle_count=" << snapshot.live_continuation_handle_count << "\n";
  std::cout << "last_handoff_executor_tag=" << snapshot.last_handoff_executor_tag << "\n";
  std::cout << "last_resume_return_value=" << snapshot.last_resume_return_value << "\n";

  return (run_task == 7 && load_value == 7 && copy_status == 0) ? 0 : 1;
}
