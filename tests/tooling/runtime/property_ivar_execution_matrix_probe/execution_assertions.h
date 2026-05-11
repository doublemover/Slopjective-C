#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_EXECUTION_ASSERTIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_EXECUTION_ASSERTIONS_H_

#include "probe_result.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::property_ivar_execution_matrix {

inline void StabilizePropertyEntryObservation(
    PropertyEntryObservation &observation) {
  ::objc3c::runtime::probe::StabilizePropertyEntry(
      observation.entry, observation.queried_class,
      observation.resolved_class, observation.property_name,
      observation.declaration_owner, observation.export_owner,
      observation.getter_selector, observation.setter_selector,
      observation.effective_getter_selector,
      observation.effective_setter_selector, observation.ivar_binding,
      observation.synthesized_binding, observation.layout_symbol,
      observation.getter_owner, observation.setter_owner);
}

inline void StabilizePropertyRegistryObservation(
    PropertyRegistryObservation &observation) {
  ::objc3c::runtime::probe::StabilizePropertyRegistryState(
      observation.state, observation.queried_class,
      observation.queried_property, observation.resolved_class,
      observation.resolved_owner);
}

inline void StabilizeMethodCacheEntryObservation(
    MethodCacheEntryObservation &observation) {
  ::objc3c::runtime::probe::StabilizeMethodCacheEntry(
      observation.entry, observation.selector, observation.class_name,
      observation.owner);
}

inline void CopyPropertyEntry(const char *property_name,
                              PropertyEntryObservation &observation) {
  (void)objc3_runtime_copy_property_entry_for_testing(
      kWidgetClassName, property_name, &observation.entry);
}

inline void CapturePropertyEntryAssertions(
    PropertyIvarExecutionAssertions &assertions) {
  CopyPropertyEntry(kCountPropertyName, assertions.count_property);
  CopyPropertyEntry(kEnabledPropertyName, assertions.enabled_property);
  CopyPropertyEntry(kValuePropertyName, assertions.value_property);
  CopyPropertyEntry(kTokenPropertyName, assertions.token_property);

  StabilizePropertyEntryObservation(assertions.count_property);
  StabilizePropertyEntryObservation(assertions.enabled_property);
  StabilizePropertyEntryObservation(assertions.value_property);
  StabilizePropertyEntryObservation(assertions.token_property);
}

inline PropertyRegistryObservation CapturePropertyRegistryState() {
  PropertyRegistryObservation observation;
  (void)objc3_runtime_copy_property_registry_state_for_testing(
      &observation.state);
  StabilizePropertyRegistryObservation(observation);
  return observation;
}

inline void CopyMethodCacheEntry(int widget_instance, const char *selector,
                                 MethodCacheEntryObservation &observation) {
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      widget_instance, selector, &observation.entry);
}

inline void CaptureMethodCacheAssertions(
    int widget_instance, PropertyIvarExecutionAssertions &assertions) {
  CopyMethodCacheEntry(widget_instance, kCountGetterSelector,
                       assertions.count_method);
  CopyMethodCacheEntry(widget_instance, kEnabledGetterSelector,
                       assertions.enabled_method);
  CopyMethodCacheEntry(widget_instance, kValueGetterSelector,
                       assertions.value_method);
  CopyMethodCacheEntry(widget_instance, kTokenGetterSelector,
                       assertions.token_method);

  StabilizeMethodCacheEntryObservation(assertions.count_method);
  StabilizeMethodCacheEntryObservation(assertions.enabled_method);
  StabilizeMethodCacheEntryObservation(assertions.value_method);
  StabilizeMethodCacheEntryObservation(assertions.token_method);
}

inline PropertyIvarExecutionAssertions CapturePropertyIvarExecutionAssertions(
    int widget_instance) {
  PropertyIvarExecutionAssertions assertions;
  CapturePropertyEntryAssertions(assertions);
  assertions.registry_state = CapturePropertyRegistryState();
  CaptureMethodCacheAssertions(widget_instance, assertions);
  return assertions;
}

}  // namespace objc3c::runtime::probe::property_ivar_execution_matrix

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_EXECUTION_ASSERTIONS_H_
