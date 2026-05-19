#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_ALLOCATION_INVARIANT_ASSERTIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_ALLOCATION_INVARIANT_ASSERTIONS_H_

#include "fixture_definitions.h"
#include "probe_state.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::instance_allocation_runtime {

inline void StabilizeRegistrationStateObservation(
    RegistrationStateObservation &observation) {
  ::objc3c::runtime::probe::StabilizeRegistrationState(
      observation.state, observation.module, observation.identity);
}

inline void StabilizeSelectorTableStateObservation(
    SelectorTableStateObservation &observation) {
  ::objc3c::runtime::probe::StabilizeSelectorTableState(
      observation.state, observation.last_selector);
}

inline void StabilizeMethodCacheEntryObservation(
    MethodCacheEntryObservation &observation) {
  ::objc3c::runtime::probe::StabilizeMethodCacheEntry(
      observation.entry, observation.selector, observation.class_name,
      observation.owner);
}

inline void StabilizeRealizedGraphStateObservation(
    RealizedGraphStateObservation &observation) {
  ::objc3c::runtime::probe::StabilizeRealizedGraphState(
      observation.state, observation.class_name, observation.owner,
      observation.metaclass, observation.category_owner,
      observation.category_name, observation.allocated_class);
}

inline void StabilizeRealizedClassEntryObservation(
    RealizedClassEntryObservation &observation) {
  ::objc3c::runtime::probe::StabilizeRealizedClassEntry(
      observation.entry, observation.module, observation.identity,
      observation.class_name, observation.class_owner,
      observation.metaclass_owner, observation.super_class,
      observation.super_metaclass, observation.category_owner,
      observation.category_name);
}

inline void StabilizeInstanceEntryObservation(
    InstanceEntryObservation &observation) {
  ::objc3c::runtime::probe::StabilizeNullableCString(
      observation.entry.class_name, observation.class_name,
      observation.entry.class_name);
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

inline void StabilizeAllocationInvariantSnapshots(
    AllocationInvariantSnapshots &snapshots) {
  StabilizeMethodCacheEntryObservation(snapshots.count_entry);
  StabilizeMethodCacheEntryObservation(snapshots.set_count_entry);
  StabilizeMethodCacheEntryObservation(snapshots.base_count_entry);
  StabilizeMethodCacheEntryObservation(snapshots.set_base_count_entry);
  StabilizeRealizedGraphStateObservation(snapshots.graph_state);
  StabilizeRealizedClassEntryObservation(snapshots.base_entry);
  StabilizeRealizedClassEntryObservation(snapshots.widget_entry);
  StabilizeInstanceEntryObservation(snapshots.first_instance);
  StabilizeInstanceEntryObservation(snapshots.second_instance);
  StabilizePropertyEntryObservation(snapshots.base_count_property);
  StabilizePropertyEntryObservation(snapshots.count_property);
  StabilizePropertyEntryObservation(snapshots.value_property);
}

inline RegistrationStateObservation CaptureRegistrationState() {
  RegistrationStateObservation observation;
  (void)objc3_runtime_copy_registration_state_for_testing(&observation.state);
  return observation;
}

inline SelectorTableStateObservation CaptureSelectorTableState() {
  SelectorTableStateObservation observation;
  (void)objc3_runtime_copy_selector_lookup_table_state_for_testing(
      &observation.state);
  return observation;
}

inline MethodCacheEntryObservation CaptureMethodCacheEntry(int receiver,
                                                           const char *selector) {
  MethodCacheEntryObservation observation;
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      receiver, selector, &observation.entry);
  return observation;
}

inline RealizedGraphStateObservation CaptureRealizedGraphState() {
  RealizedGraphStateObservation observation;
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(
      &observation.state);
  return observation;
}

inline RealizedClassEntryObservation CaptureRealizedClassEntry(
    const char *class_name) {
  RealizedClassEntryObservation observation;
  (void)objc3_runtime_copy_realized_class_entry_for_testing(
      class_name, &observation.entry);
  return observation;
}

inline RealizedClassEntryObservation CaptureWidgetClassEntry() {
  return CaptureRealizedClassEntry(kWidgetClassName);
}

inline RealizedClassEntryObservation CaptureBaseClassEntry() {
  return CaptureRealizedClassEntry(kBaseClassName);
}

inline InstanceEntryObservation CaptureInstanceEntry(int receiver) {
  InstanceEntryObservation observation;
  (void)objc3_runtime_copy_instance_entry_for_testing(receiver,
                                                      &observation.entry);
  return observation;
}

inline PropertyEntryObservation CapturePropertyEntry(const char *property_name) {
  PropertyEntryObservation observation;
  (void)objc3_runtime_copy_property_entry_for_testing(
      kWidgetClassName, property_name, &observation.entry);
  return observation;
}

inline void CaptureAllocationInvariantSnapshots(
    const AllocationFixture &fixture,
    AllocationInvariantSnapshots &snapshots) {
  snapshots.base_count_entry =
      CaptureMethodCacheEntry(fixture.first_alloc, kBaseCountGetterSelector);
  StabilizeMethodCacheEntryObservation(snapshots.base_count_entry);
  snapshots.set_base_count_entry =
      CaptureMethodCacheEntry(fixture.first_alloc, kBaseCountSetterSelector);
  StabilizeMethodCacheEntryObservation(snapshots.set_base_count_entry);
  snapshots.count_entry =
      CaptureMethodCacheEntry(fixture.first_alloc, kCountGetterSelector);
  StabilizeMethodCacheEntryObservation(snapshots.count_entry);
  snapshots.set_count_entry =
      CaptureMethodCacheEntry(fixture.first_alloc, kCountSetterSelector);
  StabilizeMethodCacheEntryObservation(snapshots.set_count_entry);
  snapshots.graph_state = CaptureRealizedGraphState();
  StabilizeRealizedGraphStateObservation(snapshots.graph_state);
  snapshots.base_entry = CaptureBaseClassEntry();
  StabilizeRealizedClassEntryObservation(snapshots.base_entry);
  snapshots.widget_entry = CaptureWidgetClassEntry();
  StabilizeRealizedClassEntryObservation(snapshots.widget_entry);
  snapshots.first_instance = CaptureInstanceEntry(fixture.first_alloc);
  StabilizeInstanceEntryObservation(snapshots.first_instance);
  snapshots.second_instance = CaptureInstanceEntry(fixture.second_alloc);
  StabilizeInstanceEntryObservation(snapshots.second_instance);
  snapshots.base_count_property = CapturePropertyEntry(kBaseCountPropertyName);
  StabilizePropertyEntryObservation(snapshots.base_count_property);
  snapshots.count_property = CapturePropertyEntry(kCountPropertyName);
  StabilizePropertyEntryObservation(snapshots.count_property);
  snapshots.value_property = CapturePropertyEntry(kValuePropertyName);
  StabilizePropertyEntryObservation(snapshots.value_property);
}

}  // namespace objc3c::runtime::probe::instance_allocation_runtime

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_ALLOCATION_INVARIANT_ASSERTIONS_H_
