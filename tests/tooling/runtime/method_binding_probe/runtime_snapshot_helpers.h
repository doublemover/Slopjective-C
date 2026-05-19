#pragma once

#include "method_fixture_definitions.h"
#include "probe_state.h"

#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c {
namespace tooling {
namespace method_binding_probe {

inline void CaptureRegistrationState(RegistrationObservation &observation) {
  observation = RegistrationObservation{};
  observation.status =
      objc3_runtime_copy_registration_state_for_testing(&observation.state);
  ::objc3c::runtime::probe::StabilizeRegistrationState(
      observation.state, observation.module_storage,
      observation.identity_storage);
}

inline void CaptureSelectorTableState(SelectorTableObservation &observation) {
  observation = SelectorTableObservation{};
  observation.status =
      objc3_runtime_copy_selector_lookup_table_state_for_testing(
          &observation.state);
  ::objc3c::runtime::probe::StabilizeSelectorTableState(
      observation.state, observation.last_storage);
}

inline void CaptureMethodCacheState(MethodCacheStateObservation &observation) {
  observation = MethodCacheStateObservation{};
  observation.status =
      objc3_runtime_copy_method_cache_state_for_testing(&observation.state);
  ::objc3c::runtime::probe::StabilizeMethodCacheState(
      observation.state, observation.selector_storage,
      observation.class_storage, observation.owner_storage);
}

inline void CaptureMethodCacheEntry(
    const MethodCacheEntryKey &key, MethodCacheEntryObservation &observation) {
  observation = MethodCacheEntryObservation{};
  observation.status = objc3_runtime_copy_method_cache_entry_for_testing(
      key.receiver, key.selector, &observation.entry);
  ::objc3c::runtime::probe::StabilizeMethodCacheEntry(
      observation.entry, observation.selector_storage,
      observation.class_storage, observation.owner_storage);
}

} // namespace method_binding_probe
} // namespace tooling
} // namespace objc3c
