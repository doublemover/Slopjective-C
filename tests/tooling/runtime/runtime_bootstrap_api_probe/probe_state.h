#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BOOTSTRAP_API_PROBE_PROBE_STATE_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BOOTSTRAP_API_PROBE_PROBE_STATE_H_

#include "runtime/public/objc3_runtime_api.h"

#include <cstdint>
#include <string>

namespace objc3c::runtime::probe::runtime_bootstrap_api {

struct RuntimeStringObservation {
  bool is_null = true;
  std::string value;

  const char *c_str_or_null() const {
    return is_null ? nullptr : value.c_str();
  }
};

struct RegistrationStateObservation {
  objc3_runtime_registration_state_snapshot snapshot{};
  int copy_status = 0;
  RuntimeStringObservation last_registered_module_name;
  RuntimeStringObservation last_registered_translation_unit_identity_key;
};

struct BootstrapApiScenario {
  int initial_copy_status = 0;
  int register_status = 0;
  RegistrationStateObservation post_register_registration;
  std::uint64_t selector_stable_id = 0;
  int dispatch_result = 0;
  int expected_dispatch_result = 0;
};

struct ResetScenario {
  RegistrationStateObservation post_reset_registration;
  std::uint64_t selector_after_reset_stable_id = 0;
};

struct BootstrapApiProbeResult {
  BootstrapApiScenario bootstrap;
  ResetScenario reset;
};

}  // namespace objc3c::runtime::probe::runtime_bootstrap_api

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BOOTSTRAP_API_PROBE_PROBE_STATE_H_
