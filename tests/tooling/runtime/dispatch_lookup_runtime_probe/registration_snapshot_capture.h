#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_REGISTRATION_SNAPSHOT_CAPTURE_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_REGISTRATION_SNAPSHOT_CAPTURE_H_

#include "probe_state.h"
#include "runtime/public/objc3_runtime_api.h"

namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe {

inline RegistrationSnapshotCapture CaptureRegistrationSnapshot() {
  RegistrationSnapshotCapture capture;
  capture.snapshot_status =
      objc3_runtime_copy_registration_state_for_testing(&capture.snapshot);
  capture.last_registered_module_name =
      capture.snapshot.last_registered_module_name != nullptr
          ? capture.snapshot.last_registered_module_name
          : "";
  capture.last_registered_translation_unit_identity_key =
      capture.snapshot.last_registered_translation_unit_identity_key != nullptr
          ? capture.snapshot.last_registered_translation_unit_identity_key
          : "";
  return capture;
}

}  // namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_REGISTRATION_SNAPSHOT_CAPTURE_H_
