#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_INSTALLATION_LOADER_LIFECYCLE_PROBE_LOADER_LIFECYCLE_ASSERTIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_INSTALLATION_LOADER_LIFECYCLE_PROBE_LOADER_LIFECYCLE_ASSERTIONS_H_

#include "fixture_runtime_bootstrap.h"
#include "installation_state_helpers.h"
#include "runtime/public/objc3_runtime_api.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::installation_loader_lifecycle_probe {

struct RegistrationAttemptResult {
  int status = 0;
  RegistrationState registration;
  ImageWalkState walk;
};

inline RegistrationAttemptResult CaptureRegistrationAttemptResult(int status) {
  return {
      status,
      CaptureRegistrationState(),
      CaptureImageWalkState(),
  };
}

inline RegistrationAttemptResult RunDuplicateRegistrationAttempt(
    const StartupState &startup) {
  const objc3_runtime_image_descriptor duplicate_image{
      startup.registration.module_name.empty()
          ? "invalid-module"
          : startup.registration.module_name.c_str(),
      startup.registration.translation_unit_identity_key.empty()
          ? "invalid-identity"
          : startup.registration.translation_unit_identity_key.c_str(),
      DuplicateRegistrationOrdinal(startup.registration.snapshot),
      startup.walk.snapshot.last_walked_class_descriptor_count,
      startup.walk.snapshot.last_walked_protocol_descriptor_count,
      startup.walk.snapshot.last_walked_category_descriptor_count,
      startup.walk.snapshot.last_walked_property_descriptor_count,
      startup.walk.snapshot.last_walked_ivar_descriptor_count,
  };
  return CaptureRegistrationAttemptResult(
      objc3_runtime_register_image(&duplicate_image));
}

inline RegistrationAttemptResult RunOutOfOrderRegistrationAttempt(
    const StartupState &startup) {
  const std::string out_of_order_identity =
      startup.registration.translation_unit_identity_key.empty()
          ? "out-of-order-identity"
          : startup.registration.translation_unit_identity_key + "-out-of-order";
  const objc3_runtime_image_descriptor out_of_order_image{
      "out-of-order-module",
      out_of_order_identity.c_str(),
      startup.registration.snapshot.next_expected_registration_order_ordinal + 1u,
      startup.walk.snapshot.last_walked_class_descriptor_count,
      startup.walk.snapshot.last_walked_protocol_descriptor_count,
      startup.walk.snapshot.last_walked_category_descriptor_count,
      startup.walk.snapshot.last_walked_property_descriptor_count,
      startup.walk.snapshot.last_walked_ivar_descriptor_count,
  };
  return CaptureRegistrationAttemptResult(
      objc3_runtime_register_image(&out_of_order_image));
}

inline RegistrationAttemptResult RunStagedRegistrationAttempt(
    const objc3_runtime_image_descriptor &compiled_image_descriptor,
    objc3_runtime_registration_table *registration_table) {
  objc3_runtime_stage_registration_table_for_bootstrap(registration_table);
  return CaptureRegistrationAttemptResult(
      objc3_runtime_register_image(&compiled_image_descriptor));
}

}  // namespace objc3c::runtime::installation_loader_lifecycle_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_INSTALLATION_LOADER_LIFECYCLE_PROBE_LOADER_LIFECYCLE_ASSERTIONS_H_
