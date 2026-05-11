#pragma once

#include "method_fixture_definitions.h"
#include "probe_state.h"

#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c {
namespace tooling {
namespace method_binding_probe {

inline RegistrationObservation CaptureRegistrationState() {
  RegistrationObservation observation{};
  observation.status =
      objc3_runtime_copy_registration_state_for_testing(&observation.state);
  ::objc3c::runtime::probe::StabilizeRegistrationState(
      observation.state, observation.module_storage,
      observation.identity_storage);
  return observation;
}

inline SelectorTableObservation CaptureSelectorTableState() {
  SelectorTableObservation observation{};
  observation.status =
      objc3_runtime_copy_selector_lookup_table_state_for_testing(
          &observation.state);
  ::objc3c::runtime::probe::StabilizeSelectorTableState(
      observation.state, observation.last_storage);
  return observation;
}

inline MethodCacheStateObservation CaptureMethodCacheState() {
  MethodCacheStateObservation observation{};
  observation.status =
      objc3_runtime_copy_method_cache_state_for_testing(&observation.state);
  ::objc3c::runtime::probe::StabilizeMethodCacheState(
      observation.state, observation.selector_storage,
      observation.class_storage, observation.owner_storage);
  return observation;
}

inline MethodCacheEntryObservation CaptureMethodCacheEntry(
    const MethodCacheEntryKey &key) {
  MethodCacheEntryObservation observation{};
  observation.status = objc3_runtime_copy_method_cache_entry_for_testing(
      key.receiver, key.selector, &observation.entry);
  ::objc3c::runtime::probe::StabilizeMethodCacheEntry(
      observation.entry, observation.selector_storage,
      observation.class_storage, observation.owner_storage);
  return observation;
}

} // namespace method_binding_probe
} // namespace tooling
} // namespace objc3c
