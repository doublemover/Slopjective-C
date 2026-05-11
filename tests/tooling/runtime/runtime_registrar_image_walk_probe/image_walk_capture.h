#pragma once

#include "probe_result.h"
#include "registrar_image_fixture_setup.h"

namespace objc3c::runtime::probe::runtime_registrar_image_walk {

inline RegistrationObservation CaptureRegistrationObservation() {
  RegistrationObservation observation;
  observation.copy_status =
      objc3_runtime_copy_registration_state_for_testing(
          &observation.snapshot);
  observation.last_registered_module_name = CopyRuntimeStringForReport(
      observation.snapshot.last_registered_module_name);
  observation.last_registered_translation_unit_identity_key =
      CopyRuntimeStringForReport(
          observation.snapshot.last_registered_translation_unit_identity_key);
  return observation;
}

inline ImageWalkObservation CaptureImageWalkObservation() {
  ImageWalkObservation observation;
  observation.copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&observation.snapshot);
  observation.last_walked_module_name = CopyRuntimeStringForReport(
      observation.snapshot.last_walked_module_name);
  observation.last_walked_translation_unit_identity_key =
      CopyRuntimeStringForReport(
          observation.snapshot.last_walked_translation_unit_identity_key);
  return observation;
}

}  // namespace objc3c::runtime::probe::runtime_registrar_image_walk
