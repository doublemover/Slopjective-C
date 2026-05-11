#pragma once

#include "probe_state.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::protocol_category_runtime {

inline void CaptureRuntimeRegistryFixture(RuntimeRegistryObservation &fixture) {
  (void)objc3_runtime_copy_registration_state_for_testing(
      &fixture.registration_state);
  (void)objc3_runtime_copy_selector_lookup_table_state_for_testing(
      &fixture.selector_table_state);
  ::objc3c::runtime::probe::StabilizeRegistrationState(
      fixture.registration_state, fixture.registration_module,
      fixture.registration_identity);
  ::objc3c::runtime::probe::StabilizeSelectorTableState(
      fixture.selector_table_state, fixture.selector_table_last);
}

}  // namespace objc3c::runtime::probe::protocol_category_runtime
