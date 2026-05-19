#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BACKED_STORAGE_OWNERSHIP_REFLECTION_PROBE_OWNERSHIP_REFLECTION_CAPTURE_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BACKED_STORAGE_OWNERSHIP_REFLECTION_PROBE_OWNERSHIP_REFLECTION_CAPTURE_H_

#include "probe_result.h"
#include "storage_fixture_definitions.h"
#include "support/runtime_snapshot_stabilizer_common.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::runtime_backed_storage_ownership_reflection {

inline void StabilizeStorageAccessorImplementationObservation(
    StorageAccessorImplementationObservation &observation) {
  objc3_runtime_storage_accessor_implementation_snapshot &snapshot =
      observation.snapshot;
  ::objc3c::runtime::probe::StabilizeNullableCString(
      snapshot.property_registry_state_snapshot_symbol,
      observation.property_registry_state_snapshot_symbol,
      snapshot.property_registry_state_snapshot_symbol);
  ::objc3c::runtime::probe::StabilizeNullableCString(
      snapshot.property_entry_snapshot_symbol,
      observation.property_entry_snapshot_symbol,
      snapshot.property_entry_snapshot_symbol);
  ::objc3c::runtime::probe::StabilizeNullableCString(
      snapshot.current_property_read_symbol,
      observation.current_property_read_symbol,
      snapshot.current_property_read_symbol);
  ::objc3c::runtime::probe::StabilizeNullableCString(
      snapshot.current_property_write_symbol,
      observation.current_property_write_symbol,
      snapshot.current_property_write_symbol);
  ::objc3c::runtime::probe::StabilizeNullableCString(
      snapshot.current_property_exchange_symbol,
      observation.current_property_exchange_symbol,
      snapshot.current_property_exchange_symbol);
  ::objc3c::runtime::probe::StabilizeNullableCString(
      snapshot.bind_current_property_context_symbol,
      observation.bind_current_property_context_symbol,
      snapshot.bind_current_property_context_symbol);
  ::objc3c::runtime::probe::StabilizeNullableCString(
      snapshot.clear_current_property_context_symbol,
      observation.clear_current_property_context_symbol,
      snapshot.clear_current_property_context_symbol);
  ::objc3c::runtime::probe::StabilizeNullableCString(
      snapshot.weak_current_property_load_symbol,
      observation.weak_current_property_load_symbol,
      snapshot.weak_current_property_load_symbol);
  ::objc3c::runtime::probe::StabilizeNullableCString(
      snapshot.weak_current_property_store_symbol,
      observation.weak_current_property_store_symbol,
      snapshot.weak_current_property_store_symbol);
  ::objc3c::runtime::probe::StabilizeNullableCString(
      snapshot.implementation_model, observation.implementation_model,
      snapshot.implementation_model);
  ::objc3c::runtime::probe::StabilizeNullableCString(
      snapshot.reflection_model, observation.reflection_model,
      snapshot.reflection_model);
  ::objc3c::runtime::probe::StabilizeNullableCString(
      snapshot.fail_closed_model, observation.fail_closed_model,
      snapshot.fail_closed_model);
}

inline void CaptureRealizedBoxEntry(StorageReflectionFixture &fixture) {
  fixture = StorageReflectionFixture{};
  (void)objc3_runtime_copy_realized_class_entry_for_testing(
      kBoxClassName, &fixture.box_entry.snapshot);
  ::objc3c::runtime::probe::StabilizeRealizedClassEntry(fixture.box_entry);
}

inline void CaptureStorageOwnershipProperty(
    const StorageOwnershipPropertyQuery &query,
    StorageOwnershipPropertyObservation &observation) {
  observation = StorageOwnershipPropertyObservation{};
  (void)objc3_runtime_copy_property_entry_for_testing(
      query.class_name, query.property_name, &observation.snapshot);
  ::objc3c::runtime::probe::StabilizePropertyEntry(observation);
}

inline void CaptureStorageAccessorImplementationSurface(
    StorageAccessorImplementationObservation &observation) {
  observation = StorageAccessorImplementationObservation{};
  (void)objc3_runtime_copy_storage_accessor_implementation_snapshot_for_testing(
      &observation.snapshot);
  StabilizeStorageAccessorImplementationObservation(observation);
}

}  // namespace objc3c::runtime::probe::runtime_backed_storage_ownership_reflection

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BACKED_STORAGE_OWNERSHIP_REFLECTION_PROBE_OWNERSHIP_REFLECTION_CAPTURE_H_
