#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/public/objc3_runtime_api.h"

#include <string>

namespace objc3c::runtime::probe::runtime_bootstrap_table_consumption {

struct RuntimeStringObservation {
  bool is_null = true;
  std::string value;

  const char *c_str_or_null() const {
    return is_null ? nullptr : value.c_str();
  }
};

inline RuntimeStringObservation CopyRuntimeString(const char *value) {
  if (value == nullptr) {
    return {};
  }
  return {false, value};
}

struct RegistrationStateObservation {
  objc3_runtime_registration_state_snapshot snapshot{};
  int copy_status = 0;
  RuntimeStringObservation registered_module_name;
  RuntimeStringObservation registered_translation_unit_identity_key;
  RuntimeStringObservation rejected_module_name;
  RuntimeStringObservation rejected_translation_unit_identity_key;
};

struct ImageWalkStateObservation {
  objc3_runtime_image_walk_state_snapshot snapshot{};
  int copy_status = 0;
};

struct StartupTableState {
  RegistrationStateObservation registration;
  ImageWalkStateObservation walk;
};

struct DuplicateConsumptionAttempt {
  int registration_status = 0;
  RegistrationStateObservation registration;
  ImageWalkStateObservation walk;
};

struct BootstrapTableConsumptionProbeResult {
  StartupTableState startup;
  DuplicateConsumptionAttempt duplicate;
};

}  // namespace objc3c::runtime::probe::runtime_bootstrap_table_consumption
