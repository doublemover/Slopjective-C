#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdint>
#include <string>

namespace objc3c::runtime::probe::runtime_registrar_image_walk {

struct RegistrationObservation {
  objc3_runtime_registration_state_snapshot snapshot{};
  int copy_status = 0;
  std::string last_registered_module_name;
  std::string last_registered_translation_unit_identity_key;
};

struct ImageWalkObservation {
  objc3_runtime_image_walk_state_snapshot snapshot{};
  int copy_status = 0;
  std::string last_walked_module_name;
  std::string last_walked_translation_unit_identity_key;
};

struct SelectorInvariantObservation {
  std::uint64_t known_selector_stable_id = 0;
  std::uint64_t unknown_selector_stable_id = 0;
};

struct ProbeResult {
  RegistrationObservation registration;
  ImageWalkObservation image_walk;
  SelectorInvariantObservation selectors;
};

}  // namespace objc3c::runtime::probe::runtime_registrar_image_walk
