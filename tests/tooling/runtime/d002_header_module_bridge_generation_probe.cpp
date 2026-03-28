#include <iostream>

#include "runtime/objc3_runtime_bootstrap_internal.h"

int main() {
  objc3_runtime_interop_bridge_generation_snapshot snapshot{};
  const int copy_status =
      objc3_runtime_copy_interop_bridge_generation_snapshot_for_testing(
          &snapshot);

  std::cout << "copy_status=" << copy_status << "\n";
  std::cout << "runtime_generation_ready=" << snapshot.runtime_generation_ready
            << "\n";
  std::cout << "cross_module_packaging_ready="
            << snapshot.cross_module_packaging_ready << "\n";
  std::cout << "header_generation_ready=" << snapshot.header_generation_ready
            << "\n";
  std::cout << "module_generation_ready=" << snapshot.module_generation_ready
            << "\n";
  std::cout << "bridge_generation_ready=" << snapshot.bridge_generation_ready
            << "\n";
  std::cout << "deterministic=" << snapshot.deterministic << "\n";
  std::cout << "header_artifact_relative_path="
            << (snapshot.header_artifact_relative_path != nullptr
                    ? snapshot.header_artifact_relative_path
                    : "")
            << "\n";
  std::cout << "module_artifact_relative_path="
            << (snapshot.module_artifact_relative_path != nullptr
                    ? snapshot.module_artifact_relative_path
                    : "")
            << "\n";
  std::cout << "bridge_artifact_relative_path="
            << (snapshot.bridge_artifact_relative_path != nullptr
                    ? snapshot.bridge_artifact_relative_path
                    : "")
            << "\n";
  std::cout << "generation_model="
            << (snapshot.generation_model != nullptr ? snapshot.generation_model
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

  return (copy_status == 0 && snapshot.runtime_generation_ready == 1 &&
          snapshot.cross_module_packaging_ready == 1 &&
          snapshot.header_generation_ready == 1 &&
          snapshot.module_generation_ready == 1 &&
          snapshot.bridge_generation_ready == 1 && snapshot.deterministic == 1)
             ? 0
             : 1;
}
