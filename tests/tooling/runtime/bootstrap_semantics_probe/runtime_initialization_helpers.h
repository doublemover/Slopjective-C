#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_SEMANTICS_PROBE_RUNTIME_INITIALIZATION_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_SEMANTICS_PROBE_RUNTIME_INITIALIZATION_HELPERS_H_

#include "runtime/public/objc3_runtime_api.h"

#include <cstdint>
#include <string>

namespace objc3c::runtime::bootstrap_semantics_probe {

struct OwnedSnapshot {
  std::uint64_t registered_image_count = 0;
  std::uint64_t registered_descriptor_total = 0;
  std::uint64_t next_expected_registration_order_ordinal = 0;
  std::uint64_t last_successful_registration_order_ordinal = 0;
  int last_registration_status = 0;
  std::string last_registered_module_name;
  std::string last_registered_translation_unit_identity_key;
  std::string last_rejected_module_name;
  std::string last_rejected_translation_unit_identity_key;
  std::uint64_t last_rejected_registration_order_ordinal = 0;
};

struct SnapshotCapture {
  int copy_status = 0;
  OwnedSnapshot snapshot;
};

inline std::string CopyRuntimeString(const char *value) {
  return value != nullptr ? value : "";
}

inline OwnedSnapshot CopySnapshot(
    const objc3_runtime_registration_state_snapshot &snapshot) {
  OwnedSnapshot owned;
  owned.registered_image_count = snapshot.registered_image_count;
  owned.registered_descriptor_total = snapshot.registered_descriptor_total;
  owned.next_expected_registration_order_ordinal =
      snapshot.next_expected_registration_order_ordinal;
  owned.last_successful_registration_order_ordinal =
      snapshot.last_successful_registration_order_ordinal;
  owned.last_registration_status = snapshot.last_registration_status;
  owned.last_registered_module_name =
      CopyRuntimeString(snapshot.last_registered_module_name);
  owned.last_registered_translation_unit_identity_key =
      CopyRuntimeString(snapshot.last_registered_translation_unit_identity_key);
  owned.last_rejected_module_name =
      CopyRuntimeString(snapshot.last_rejected_module_name);
  owned.last_rejected_translation_unit_identity_key =
      CopyRuntimeString(snapshot.last_rejected_translation_unit_identity_key);
  owned.last_rejected_registration_order_ordinal =
      snapshot.last_rejected_registration_order_ordinal;
  return owned;
}

inline void ResetRuntimeForProbe() {
  objc3_runtime_reset_for_testing();
}

inline SnapshotCapture CaptureRegistrationSnapshot() {
  objc3_runtime_registration_state_snapshot raw{};
  SnapshotCapture capture;
  capture.copy_status =
      objc3_runtime_copy_registration_state_for_testing(&raw);
  capture.snapshot = CopySnapshot(raw);
  return capture;
}

}  // namespace objc3c::runtime::bootstrap_semantics_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_SEMANTICS_PROBE_RUNTIME_INITIALIZATION_HELPERS_H_
