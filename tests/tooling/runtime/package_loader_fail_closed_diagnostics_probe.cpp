#include <iostream>

#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/public/objc3_runtime_registration_status.h"

int main() {
  const int packaging_null_status =
      objc3_runtime_copy_interop_bridge_packaging_toolchain_snapshot_for_testing(
          nullptr);
  const int bridge_null_status =
      objc3_runtime_copy_interop_bridge_generation_snapshot_for_testing(nullptr);

  objc3_runtime_interop_bridge_packaging_toolchain_snapshot packaging_snapshot{};
  objc3_runtime_interop_bridge_generation_snapshot bridge_snapshot{};
  const int packaging_copy_status =
      objc3_runtime_copy_interop_bridge_packaging_toolchain_snapshot_for_testing(
          &packaging_snapshot);
  const int bridge_copy_status =
      objc3_runtime_copy_interop_bridge_generation_snapshot_for_testing(
          &bridge_snapshot);

  const bool fail_closed_statuses =
      packaging_null_status ==
          OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR &&
      bridge_null_status == OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  const bool diagnostics_present =
      packaging_snapshot.fail_closed_model != nullptr &&
      bridge_snapshot.fail_closed_model != nullptr;
  const bool no_public_fallback_claim =
      packaging_snapshot.header_generation_ready == 0 &&
      packaging_snapshot.module_generation_ready == 0 &&
      packaging_snapshot.bridge_generation_ready == 0 &&
      bridge_snapshot.header_generation_ready == 1 &&
      bridge_snapshot.module_generation_ready == 1 &&
      bridge_snapshot.bridge_generation_ready == 1;

  std::cout << "packaging_null_status=" << packaging_null_status << "\n";
  std::cout << "bridge_null_status=" << bridge_null_status << "\n";
  std::cout << "packaging_copy_status=" << packaging_copy_status << "\n";
  std::cout << "bridge_copy_status=" << bridge_copy_status << "\n";
  std::cout << "invalid_descriptor_status="
            << OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR << "\n";
  std::cout << "fail_closed_statuses=" << (fail_closed_statuses ? 1 : 0)
            << "\n";
  std::cout << "diagnostics_present=" << (diagnostics_present ? 1 : 0)
            << "\n";
  std::cout << "no_public_fallback_claim=" << (no_public_fallback_claim ? 1 : 0)
            << "\n";
  std::cout << "packaging_fail_closed_model="
            << (packaging_snapshot.fail_closed_model != nullptr
                    ? packaging_snapshot.fail_closed_model
                    : "")
            << "\n";
  std::cout << "bridge_fail_closed_model="
            << (bridge_snapshot.fail_closed_model != nullptr
                    ? bridge_snapshot.fail_closed_model
                    : "")
            << "\n";

  return (packaging_copy_status == OBJC3_RUNTIME_REGISTRATION_STATUS_OK &&
          bridge_copy_status == OBJC3_RUNTIME_REGISTRATION_STATUS_OK &&
          fail_closed_statuses && diagnostics_present &&
          no_public_fallback_claim)
             ? 0
             : 1;
}
