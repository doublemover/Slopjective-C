#pragma once

#include "probe_result.h"
#include "property_metadata_fixtures.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::runtime_property_metadata_reflection {

inline void StabilizePropertyRegistryObservation(
    PropertyRegistryObservation &observation) {
  ::objc3c::runtime::probe::StabilizePropertyRegistryState(
      observation.state, observation.queried_class,
      observation.queried_property, observation.resolved_class,
      observation.resolved_owner);
}

inline void StabilizePropertyEntryObservation(
    PropertyEntryObservation &observation) {
  ::objc3c::runtime::probe::StabilizePropertyEntry(
      observation.entry, observation.queried_class, observation.resolved_class,
      observation.property_name, observation.declaration_owner,
      observation.export_owner, observation.getter_selector,
      observation.setter_selector, observation.effective_getter_selector,
      observation.effective_setter_selector, observation.ivar_binding,
      observation.synthesized_binding, observation.layout_symbol,
      observation.getter_owner, observation.setter_owner);
}

inline void CapturePropertyRegistryObservation(
    PropertyRegistryObservation &observation) {
  (void)objc3_runtime_copy_property_registry_state_for_testing(
      &observation.state);
  StabilizePropertyRegistryObservation(observation);
}

inline void CapturePropertyEntryObservation(
    const PropertyMetadataQuery &query, PropertyEntryObservation &observation) {
  (void)objc3_runtime_copy_property_entry_for_testing(
      query.class_name, query.property_name, &observation.entry);
  StabilizePropertyEntryObservation(observation);
}

inline void CaptureRegistryStateBeforeReflection(
    ReflectionAssertions &assertions) {
  CapturePropertyRegistryObservation(assertions.registry_state_before);
}

inline void CaptureDeclaredPropertyReflections(
    ReflectionAssertions &assertions) {
  CapturePropertyEntryObservation(kTokenPropertyQuery,
                                  assertions.token_property);
  CapturePropertyEntryObservation(kValuePropertyQuery,
                                  assertions.value_property);
  CapturePropertyEntryObservation(kCountPropertyQuery,
                                  assertions.count_property);
}

inline void CaptureRegistryStateAfterCountReflection(
    ReflectionAssertions &assertions) {
  CapturePropertyRegistryObservation(assertions.registry_state_after_count);
}

inline void CaptureMissingPropertyReflections(ReflectionAssertions &assertions) {
  CapturePropertyEntryObservation(kMissingPropertyQuery,
                                  assertions.missing_property);
  CapturePropertyEntryObservation(kMissingClassPropertyQuery,
                                  assertions.missing_class_property);
}

inline void CaptureRegistryStateAfterMissingReflection(
    ReflectionAssertions &assertions) {
  CapturePropertyRegistryObservation(assertions.registry_state_after_missing);
}

}  // namespace objc3c::runtime::probe::runtime_property_metadata_reflection
