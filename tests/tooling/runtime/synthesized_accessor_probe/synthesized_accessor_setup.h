#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_SYNTHESIZED_ACCESSOR_SETUP_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_SYNTHESIZED_ACCESSOR_SETUP_H_

#include "probe_result.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::synthesized_accessor {

inline RegistrationStateObservation CaptureRegistrationStateObservation() {
  RegistrationStateObservation observation;
  (void)objc3_runtime_copy_registration_state_for_testing(&observation.state);
  ::objc3c::runtime::probe::StabilizeRegistrationState(
      observation.state, observation.module, observation.identity);
  return observation;
}

inline SelectorTableStateObservation CaptureSelectorTableStateObservation() {
  SelectorTableStateObservation observation;
  (void)objc3_runtime_copy_selector_lookup_table_state_for_testing(
      &observation.state);
  ::objc3c::runtime::probe::StabilizeSelectorTableState(
      observation.state, observation.last_selector);
  return observation;
}

inline void CaptureSynthesizedAccessorSetup(AccessorSetup &setup) {
  setup.registration_state = CaptureRegistrationStateObservation();
  setup.selector_table_state = CaptureSelectorTableStateObservation();
}

}  // namespace objc3c::runtime::probe::synthesized_accessor

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_SYNTHESIZED_ACCESSOR_SETUP_H_
