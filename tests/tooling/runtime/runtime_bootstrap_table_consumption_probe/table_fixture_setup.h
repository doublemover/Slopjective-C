#pragma once

#include "bootstrap_record_definitions.h"
#include "runtime/public/objc3_runtime_api.h"

#include <cstdint>

namespace objc3c::runtime::probe::runtime_bootstrap_table_consumption {

inline RegistrationStateObservation CaptureRegistrationState() {
  RegistrationStateObservation state;
  state.copy_status =
      objc3_runtime_copy_registration_state_for_testing(&state.snapshot);
  state.registered_module_name =
      CopyRuntimeString(state.snapshot.last_registered_module_name);
  state.registered_translation_unit_identity_key = CopyRuntimeString(
      state.snapshot.last_registered_translation_unit_identity_key);
  state.rejected_module_name =
      CopyRuntimeString(state.snapshot.last_rejected_module_name);
  state.rejected_translation_unit_identity_key =
      CopyRuntimeString(
          state.snapshot.last_rejected_translation_unit_identity_key);
  return state;
}

inline ImageWalkStateObservation CaptureImageWalkState() {
  ImageWalkStateObservation state;
  state.copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&state.snapshot);
  return state;
}

inline StartupTableState CaptureStartupTableState() {
  return {CaptureRegistrationState(), CaptureImageWalkState()};
}

inline std::uint64_t DuplicateRegistrationOrdinal(
    const RegistrationStateObservation &registration) {
  return registration.snapshot.last_successful_registration_order_ordinal == 0
             ? 1u
             : registration.snapshot.last_successful_registration_order_ordinal;
}

inline const char *DuplicateModuleName(
    const RegistrationStateObservation &registration) {
  return registration.registered_module_name.value.empty()
             ? "invalid"
             : registration.registered_module_name.value.c_str();
}

inline const char *DuplicateTranslationUnitIdentityKey(
    const RegistrationStateObservation &registration) {
  return registration.registered_translation_unit_identity_key.value.empty()
             ? "invalid"
             : registration.registered_translation_unit_identity_key.value.c_str();
}

inline objc3_runtime_image_descriptor BuildDuplicateImageDescriptor(
    const StartupTableState &startup) {
  return {
      DuplicateModuleName(startup.registration),
      DuplicateTranslationUnitIdentityKey(startup.registration),
      DuplicateRegistrationOrdinal(startup.registration),
      startup.walk.snapshot.last_walked_class_descriptor_count,
      startup.walk.snapshot.last_walked_protocol_descriptor_count,
      startup.walk.snapshot.last_walked_category_descriptor_count,
      startup.walk.snapshot.last_walked_property_descriptor_count,
      startup.walk.snapshot.last_walked_ivar_descriptor_count,
  };
}

}  // namespace objc3c::runtime::probe::runtime_bootstrap_table_consumption
