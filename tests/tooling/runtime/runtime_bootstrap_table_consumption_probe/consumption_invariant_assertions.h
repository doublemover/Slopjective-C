#pragma once

#include "bootstrap_record_definitions.h"
#include "table_fixture_setup.h"
#include "runtime/public/objc3_runtime_api.h"

namespace objc3c::runtime::probe::runtime_bootstrap_table_consumption {

inline DuplicateConsumptionAttempt RunDuplicateRegistrationConsumption(
    const StartupTableState &startup) {
  const objc3_runtime_image_descriptor duplicate_image =
      BuildDuplicateImageDescriptor(startup);

  DuplicateConsumptionAttempt attempt;
  attempt.registration_status = objc3_runtime_register_image(&duplicate_image);
  attempt.registration = CaptureRegistrationState();
  attempt.walk = CaptureImageWalkState();
  return attempt;
}

}  // namespace objc3c::runtime::probe::runtime_bootstrap_table_consumption
