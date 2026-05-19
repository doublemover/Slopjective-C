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

inline void StabilizeAllocationInvariantSnapshots(
    AllocationInvariantSnapshots &snapshots) {
  StabilizeMethodCacheEntryObservation(snapshots.count_entry);
  StabilizeMethodCacheEntryObservation(snapshots.set_count_entry);
  StabilizeRealizedGraphStateObservation(snapshots.graph_state);
  StabilizeRealizedClassEntryObservation(snapshots.widget_entry);
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

inline RealizedClassEntryObservation CaptureWidgetClassEntry() {
  RealizedClassEntryObservation observation;
  (void)objc3_runtime_copy_realized_class_entry_for_testing(
      kWidgetClassName, &observation.entry);
  return observation;
}

inline void CaptureAllocationInvariantSnapshots(
    const AllocationFixture &fixture,
    AllocationInvariantSnapshots &snapshots) {
  snapshots.count_entry =
      CaptureMethodCacheEntry(fixture.first_alloc, kCountGetterSelector);
  StabilizeMethodCacheEntryObservation(snapshots.count_entry);
  snapshots.set_count_entry =
      CaptureMethodCacheEntry(fixture.first_alloc, kCountSetterSelector);
  StabilizeMethodCacheEntryObservation(snapshots.set_count_entry);
  snapshots.graph_state = CaptureRealizedGraphState();
  StabilizeRealizedGraphStateObservation(snapshots.graph_state);
  snapshots.widget_entry = CaptureWidgetClassEntry();
  StabilizeRealizedClassEntryObservation(snapshots.widget_entry);
}

}  // namespace objc3c::runtime::probe::instance_allocation_runtime

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_ALLOCATION_INVARIANT_ASSERTIONS_H_
