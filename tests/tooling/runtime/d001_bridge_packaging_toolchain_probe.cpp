#include <iostream>

#include "runtime/objc3_runtime_bootstrap_internal.h"

int main() {
  objc3_runtime_interop_bridge_packaging_toolchain_snapshot snapshot{};
  const int copy_status =
      objc3_runtime_copy_interop_bridge_packaging_toolchain_snapshot_for_testing(
          &snapshot);

  std::cout << "copy_status=" << copy_status << "\n";
  std::cout << "packaging_topology_ready=" << snapshot.packaging_topology_ready << "\n";
  std::cout << "operator_visible_evidence_ready=" << snapshot.operator_visible_evidence_ready << "\n";
  std::cout << "header_generation_ready=" << snapshot.header_generation_ready << "\n";
  std::cout << "module_generation_ready=" << snapshot.module_generation_ready << "\n";
  std::cout << "bridge_generation_ready=" << snapshot.bridge_generation_ready << "\n";
  std::cout << "deterministic=" << snapshot.deterministic << "\n";
  std::cout << "runtime_support_library_archive_relative_path="
            << (snapshot.runtime_support_library_archive_relative_path != nullptr
                    ? snapshot.runtime_support_library_archive_relative_path
                    : "")
            << "\n";
  std::cout << "registration_manifest_model="
            << (snapshot.registration_manifest_model != nullptr
                    ? snapshot.registration_manifest_model
                    : "")
            << "\n";
  std::cout << "cross_module_link_plan_model="
            << (snapshot.cross_module_link_plan_model != nullptr
                    ? snapshot.cross_module_link_plan_model
                    : "")
            << "\n";
  std::cout << "operator_visible_evidence_model="
            << (snapshot.operator_visible_evidence_model != nullptr
                    ? snapshot.operator_visible_evidence_model
                    : "")
            << "\n";
  std::cout << "fail_closed_model="
            << (snapshot.fail_closed_model != nullptr
                    ? snapshot.fail_closed_model
                    : "")
            << "\n";

  return (copy_status == 0 && snapshot.packaging_topology_ready == 1 &&
          snapshot.operator_visible_evidence_ready == 1 &&
          snapshot.header_generation_ready == 0 &&
          snapshot.module_generation_ready == 0 &&
          snapshot.bridge_generation_ready == 0 &&
          snapshot.deterministic == 1)
             ? 0
             : 1;
}