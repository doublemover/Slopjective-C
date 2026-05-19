#pragma once

#include "probe_state.h"

#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c {
namespace tooling {
namespace method_cache_slow_path_probe {

inline void CaptureRegistrationState(RegistrationObservation &observation) {
  observation = RegistrationObservation{};
  (void)objc3_runtime_copy_registration_state_for_testing(&observation.state);
  ::objc3c::runtime::probe::StabilizeRegistrationState(
      observation.state, observation.module_storage,
      observation.identity_storage);
}

inline void CaptureSelectorTableState(SelectorTableObservation &observation) {
  observation = SelectorTableObservation{};
  (void)objc3_runtime_copy_selector_lookup_table_state_for_testing(
      &observation.state);
  ::objc3c::runtime::probe::StabilizeSelectorTableState(
      observation.state, observation.last_storage);
}

inline void CaptureMethodCacheState(MethodCacheStateObservation &observation) {
  observation = MethodCacheStateObservation{};
  (void)objc3_runtime_copy_method_cache_state_for_testing(&observation.state);
  ::objc3c::runtime::probe::StabilizeMethodCacheState(
      observation.state, observation.selector_storage,
      observation.class_storage, observation.owner_storage);
}

inline void CaptureMethodCacheEntry(
    const int class_id,
    const char *selector,
    MethodCacheEntryObservation &observation) {
  observation = MethodCacheEntryObservation{};
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      class_id, selector, &observation.entry);
  ::objc3c::runtime::probe::StabilizeMethodCacheEntry(
      observation.entry, observation.selector_storage,
      observation.class_storage, observation.owner_storage);
}

} // namespace method_cache_slow_path_probe
} // namespace tooling
} // namespace objc3c
