#pragma once

#include "probe_state.h"

#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c {
namespace tooling {
namespace method_cache_slow_path_probe {

inline RegistrationObservation CaptureRegistrationState() {
  RegistrationObservation observation{};
  (void)objc3_runtime_copy_registration_state_for_testing(&observation.state);
  ::objc3c::runtime::probe::StabilizeRegistrationState(
      observation.state, observation.module_storage,
      observation.identity_storage);
  return observation;
}

inline SelectorTableObservation CaptureSelectorTableState() {
  SelectorTableObservation observation{};
  (void)objc3_runtime_copy_selector_lookup_table_state_for_testing(
      &observation.state);
  ::objc3c::runtime::probe::StabilizeSelectorTableState(
      observation.state, observation.last_storage);
  return observation;
}

inline MethodCacheStateObservation CaptureMethodCacheState() {
  MethodCacheStateObservation observation{};
  (void)objc3_runtime_copy_method_cache_state_for_testing(&observation.state);
  ::objc3c::runtime::probe::StabilizeMethodCacheState(
      observation.state, observation.selector_storage,
      observation.class_storage, observation.owner_storage);
  return observation;
}

inline MethodCacheEntryObservation CaptureMethodCacheEntry(
    const int class_id,
    const char *selector) {
  MethodCacheEntryObservation observation{};
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      class_id, selector, &observation.entry);
  ::objc3c::runtime::probe::StabilizeMethodCacheEntry(
      observation.entry, observation.selector_storage,
      observation.class_storage, observation.owner_storage);
  return observation;
}

} // namespace method_cache_slow_path_probe
} // namespace tooling
} // namespace objc3c
