#include <iostream>

#include "runtime/objc3_runtime_bootstrap_internal.h"

int main() {
  objc3_runtime_metaprogramming_expansion_host_boundary_snapshot snapshot{};
  const int copy_status =
      objc3_runtime_copy_metaprogramming_expansion_host_boundary_snapshot_for_testing(
          &snapshot);

  std::cout << "copy_status=" << copy_status << "\n";
  std::cout << "property_runtime_ready=" << snapshot.property_runtime_ready << "\n";
  std::cout << "macro_host_execution_ready=" << snapshot.macro_host_execution_ready << "\n";
  std::cout << "macro_host_process_launch_ready=" << snapshot.macro_host_process_launch_ready << "\n";
  std::cout << "runtime_package_loader_ready=" << snapshot.runtime_package_loader_ready << "\n";
  std::cout << "deterministic=" << snapshot.deterministic << "\n";
  std::cout << "runtime_support_library_archive_relative_path="
            << (snapshot.runtime_support_library_archive_relative_path != nullptr
                    ? snapshot.runtime_support_library_archive_relative_path
                    : "")
            << "\n";
  std::cout << "property_behavior_runtime_model="
            << (snapshot.property_behavior_runtime_model != nullptr
                    ? snapshot.property_behavior_runtime_model
                    : "")
            << "\n";
  std::cout << "macro_expansion_host_model="
            << (snapshot.macro_expansion_host_model != nullptr
                    ? snapshot.macro_expansion_host_model
                    : "")
            << "\n";
  std::cout << "packaging_model="
            << (snapshot.packaging_model != nullptr ? snapshot.packaging_model
                                                    : "")
            << "\n";
  std::cout << "fail_closed_model="
            << (snapshot.fail_closed_model != nullptr ? snapshot.fail_closed_model
                                                      : "")
            << "\n";

  return (copy_status == 0 && snapshot.property_runtime_ready == 1 &&
          snapshot.macro_host_execution_ready == 0 &&
          snapshot.macro_host_process_launch_ready == 0 &&
          snapshot.runtime_package_loader_ready == 0 &&
          snapshot.deterministic == 1)
             ? 0
             : 1;
}
