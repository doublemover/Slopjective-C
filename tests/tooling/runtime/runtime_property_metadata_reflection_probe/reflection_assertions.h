#pragma once

#include "probe_result.h"
#include "property_metadata_fixtures.h"
#include "support/runtime_snapshot_stabilizers.h"

#include <cstdlib>
#include <cstring>

namespace objc3c::runtime::probe::runtime_property_metadata_reflection {

inline void PoisonPropertyEntrySnapshot(
    objc3_runtime_property_entry_snapshot &snapshot) {
  constexpr const char *kPoisonedSnapshotField =
      "poisoned-property-entry-snapshot-field";
  snapshot.found = 1;
  snapshot.inherited = 1;
  snapshot.setter_available = 1;
  snapshot.has_runtime_getter = 1;
  snapshot.has_runtime_setter = 1;
  snapshot.attribute_count = 99;
  snapshot.is_readonly = 1;
  snapshot.is_nonatomic = 1;
  snapshot.is_strong = 1;
  snapshot.is_assign = 1;
  snapshot.is_weak = 1;
  snapshot.is_copy = 1;
  snapshot.has_custom_getter = 1;
  snapshot.has_custom_setter = 1;
  snapshot.base_identity = 99;
  snapshot.slot_index = 99;
  snapshot.offset_bytes = 99;
  snapshot.size_bytes = 99;
  snapshot.alignment_bytes = 99;
  snapshot.padding_bytes = 99;
  snapshot.inherited_slot_count = 99;
  snapshot.inherited_size_bytes = 99;
  snapshot.owner_size_bytes = 99;
  snapshot.init_order_index = 99;
  snapshot.destroy_order_index = 99;
  snapshot.layout_valid = 1;
  snapshot.instance_size_bytes = 99;
  snapshot.queried_class_name = kPoisonedSnapshotField;
  snapshot.resolved_class_name = kPoisonedSnapshotField;
  snapshot.property_name = kPoisonedSnapshotField;
  snapshot.declaration_owner_identity = kPoisonedSnapshotField;
  snapshot.export_owner_identity = kPoisonedSnapshotField;
  snapshot.getter_selector = kPoisonedSnapshotField;
  snapshot.setter_selector = kPoisonedSnapshotField;
  snapshot.effective_getter_selector = kPoisonedSnapshotField;
  snapshot.effective_setter_selector = kPoisonedSnapshotField;
  snapshot.ivar_binding_symbol = kPoisonedSnapshotField;
  snapshot.synthesized_binding_symbol = kPoisonedSnapshotField;
  snapshot.ivar_layout_symbol = kPoisonedSnapshotField;
  snapshot.ivar_layout_replay_key = kPoisonedSnapshotField;
  snapshot.property_attribute_profile = kPoisonedSnapshotField;
  snapshot.property_behavior_name = kPoisonedSnapshotField;
  snapshot.ownership_lifetime_profile = kPoisonedSnapshotField;
  snapshot.ownership_runtime_hook_profile = kPoisonedSnapshotField;
  snapshot.accessor_ownership_profile = kPoisonedSnapshotField;
  snapshot.getter_owner_identity = kPoisonedSnapshotField;
  snapshot.setter_owner_identity = kPoisonedSnapshotField;
}

inline bool MissingPropertyEntrySnapshotWasCleared(
    const objc3_runtime_property_entry_snapshot &snapshot) {
  return snapshot.found == 0 && snapshot.inherited == 0 &&
         snapshot.setter_available == 0 && snapshot.has_runtime_getter == 0 &&
         snapshot.has_runtime_setter == 0 && snapshot.attribute_count == 0 &&
         snapshot.is_readonly == 0 && snapshot.is_nonatomic == 0 &&
         snapshot.is_strong == 0 && snapshot.is_assign == 0 &&
         snapshot.is_weak == 0 && snapshot.is_copy == 0 &&
         snapshot.has_custom_getter == 0 && snapshot.has_custom_setter == 0 &&
         snapshot.base_identity == 0 && snapshot.slot_index == 0 &&
         snapshot.offset_bytes == 0 && snapshot.size_bytes == 0 &&
         snapshot.alignment_bytes == 0 && snapshot.padding_bytes == 0 &&
         snapshot.inherited_slot_count == 0 &&
         snapshot.inherited_size_bytes == 0 &&
         snapshot.owner_size_bytes == 0 && snapshot.init_order_index == 0 &&
         snapshot.destroy_order_index == 0 && snapshot.layout_valid == 0 &&
         snapshot.instance_size_bytes == 0 &&
         snapshot.resolved_class_name == nullptr &&
         snapshot.property_name == nullptr &&
         snapshot.declaration_owner_identity == nullptr &&
         snapshot.export_owner_identity == nullptr &&
         snapshot.getter_selector == nullptr &&
         snapshot.setter_selector == nullptr &&
         snapshot.effective_getter_selector == nullptr &&
         snapshot.effective_setter_selector == nullptr &&
         snapshot.ivar_binding_symbol == nullptr &&
         snapshot.synthesized_binding_symbol == nullptr &&
         snapshot.ivar_layout_symbol == nullptr &&
         snapshot.ivar_layout_replay_key == nullptr &&
         snapshot.property_attribute_profile == nullptr &&
         snapshot.property_behavior_name == nullptr &&
         snapshot.ownership_lifetime_profile == nullptr &&
         snapshot.ownership_runtime_hook_profile == nullptr &&
         snapshot.accessor_ownership_profile == nullptr &&
         snapshot.getter_owner_identity == nullptr &&
         snapshot.setter_owner_identity == nullptr;
}

inline void AssertMissingPropertyEntrySnapshotCleared(
    const PropertyMetadataQuery &query,
    const PropertyEntryObservation &observation) {
  if (!MissingPropertyEntrySnapshotWasCleared(observation.entry) ||
      observation.queried_class != query.class_name ||
      observation.entry.queried_class_name == nullptr) {
    std::abort();
  }
}

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
      observation.property_behavior_name, observation.getter_owner,
      observation.setter_owner);
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
  if (assertions.token_property.entry.property_behavior_name == nullptr ||
      std::strcmp(assertions.token_property.entry.property_behavior_name,
                  "Projected") != 0 ||
      assertions.value_property.entry.property_behavior_name == nullptr ||
      std::strcmp(assertions.value_property.entry.property_behavior_name,
                  "Observed") != 0 ||
      assertions.count_property.entry.property_behavior_name != nullptr) {
    std::abort();
  }
}

inline void CaptureRegistryStateAfterCountReflection(
    ReflectionAssertions &assertions) {
  CapturePropertyRegistryObservation(assertions.registry_state_after_count);
}

inline void CaptureMissingPropertyReflections(ReflectionAssertions &assertions) {
  PoisonPropertyEntrySnapshot(assertions.missing_property.entry);
  CapturePropertyEntryObservation(kMissingPropertyQuery,
                                  assertions.missing_property);
  AssertMissingPropertyEntrySnapshotCleared(kMissingPropertyQuery,
                                            assertions.missing_property);
  PoisonPropertyEntrySnapshot(assertions.missing_class_property.entry);
  CapturePropertyEntryObservation(kMissingClassPropertyQuery,
                                  assertions.missing_class_property);
  AssertMissingPropertyEntrySnapshotCleared(
      kMissingClassPropertyQuery,
      assertions.missing_class_property);
}

inline void CaptureRegistryStateAfterMissingReflection(
    ReflectionAssertions &assertions) {
  CapturePropertyRegistryObservation(assertions.registry_state_after_missing);
}

}  // namespace objc3c::runtime::probe::runtime_property_metadata_reflection
