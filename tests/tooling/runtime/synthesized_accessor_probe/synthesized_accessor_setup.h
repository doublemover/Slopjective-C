#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_SYNTHESIZED_ACCESSOR_SETUP_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_SYNTHESIZED_ACCESSOR_SETUP_H_

#include "probe_result.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::synthesized_accessor {

inline void CaptureRegistrationStateObservation(
    RegistrationStateObservation &observation) {
  observation = RegistrationStateObservation{};
  (void)objc3_runtime_copy_registration_state_for_testing(&observation.state);
  ::objc3c::runtime::probe::StabilizeRegistrationState(
      observation.state, observation.module, observation.identity);
}

inline void CaptureSelectorTableStateObservation(
    SelectorTableStateObservation &observation) {
  observation = SelectorTableStateObservation{};
  (void)objc3_runtime_copy_selector_lookup_table_state_for_testing(
      &observation.state);
  ::objc3c::runtime::probe::StabilizeSelectorTableState(
      observation.state, observation.last_selector);
}

inline void CaptureSynthesizedAccessorSetup(AccessorSetup &setup) {
  setup = AccessorSetup{};
  CaptureRegistrationStateObservation(setup.registration_state);
  CaptureSelectorTableStateObservation(setup.selector_table_state);
}

}  // namespace objc3c::runtime::probe::synthesized_accessor

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_SYNTHESIZED_ACCESSOR_SETUP_H_
