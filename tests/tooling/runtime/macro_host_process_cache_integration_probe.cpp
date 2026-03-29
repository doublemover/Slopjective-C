#include <iostream>

#include "runtime/objc3_runtime_bootstrap_internal.h"

int main() {
  objc3_runtime_metaprogramming_macro_host_process_cache_integration_snapshot snapshot{};
  const int copy_status =
      objc3_runtime_copy_metaprogramming_macro_host_process_cache_integration_snapshot_for_testing(
          &snapshot);

  std::cout << "copy_status=" << copy_status << "\n";
  std::cout << "property_runtime_ready=" << snapshot.property_runtime_ready << "\n";
  std::cout << "macro_host_execution_ready=" << snapshot.macro_host_execution_ready
            << "\n";
  std::cout << "macro_host_process_launch_ready="
            << snapshot.macro_host_process_launch_ready << "\n";
  std::cout << "runtime_package_loader_ready="
            << snapshot.runtime_package_loader_ready << "\n";
  std::cout << "deterministic=" << snapshot.deterministic << "\n";
  std::cout << "host_executable_relative_path="
            << (snapshot.host_executable_relative_path != nullptr
                    ? snapshot.host_executable_relative_path
                    : "")
            << "\n";
  std::cout << "cache_root_relative_path="
            << (snapshot.cache_root_relative_path != nullptr
                    ? snapshot.cache_root_relative_path
                    : "")
            << "\n";
  std::cout << "host_model="
            << (snapshot.host_model != nullptr ? snapshot.host_model : "") << "\n";
  std::cout << "toolchain_model="
            << (snapshot.toolchain_model != nullptr ? snapshot.toolchain_model : "")
            << "\n";
  std::cout << "cache_model="
            << (snapshot.cache_model != nullptr ? snapshot.cache_model : "") << "\n";
  std::cout << "fail_closed_model="
            << (snapshot.fail_closed_model != nullptr ? snapshot.fail_closed_model
                                                      : "")
            << "\n";

  return (copy_status == 0 && snapshot.property_runtime_ready == 1 &&
          snapshot.macro_host_execution_ready == 1 &&
          snapshot.macro_host_process_launch_ready == 1 &&
          snapshot.runtime_package_loader_ready == 0 &&
          snapshot.deterministic == 1)
             ? 0
             : 1;
}
