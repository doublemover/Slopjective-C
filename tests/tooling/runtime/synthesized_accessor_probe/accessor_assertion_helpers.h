#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_ACCESSOR_ASSERTION_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_ACCESSOR_ASSERTION_HELPERS_H_

#include "accessor_fixture_definitions.h"
#include "probe_result.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::synthesized_accessor {

inline void StabilizeMethodCacheObservation(
    MethodCacheEntryObservation &observation) {
  ::objc3c::runtime::probe::StabilizeMethodCacheEntry(
      observation.entry, observation.selector, observation.class_name,
      observation.owner);
}

inline void StabilizePropertyObservation(PropertyEntryObservation &observation) {
  ::objc3c::runtime::probe::StabilizePropertyEntry(
      observation.entry, observation.queried_class, observation.resolved_class,
      observation.property_name, observation.declaration_owner,
      observation.export_owner, observation.getter_selector,
      observation.setter_selector, observation.effective_getter_selector,
      observation.effective_setter_selector, observation.ivar_binding,
      observation.synthesized_binding, observation.layout_symbol,
      observation.getter_owner, observation.setter_owner);
}

inline void CapturePropertyObservation(
    const char *property_name, PropertyEntryObservation &observation) {
  observation = PropertyEntryObservation{};
  (void)objc3_runtime_copy_property_entry_for_testing(
      kWidgetClassName, property_name, &observation.entry);
  StabilizePropertyObservation(observation);
}

inline void CaptureMethodCacheObservation(
    int receiver, const char *selector,
    MethodCacheEntryObservation &observation) {
  observation = MethodCacheEntryObservation{};
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      receiver, selector, &observation.entry);
  StabilizeMethodCacheObservation(observation);
}

inline void CaptureAccessorAssertions(int widget_instance,
                                      AccessorAssertions &assertions) {
  assertions = AccessorAssertions{};
  CapturePropertyObservation(kCountPropertyName, assertions.count_property);
  CapturePropertyObservation(kEnabledPropertyName, assertions.enabled_property);
  CapturePropertyObservation(kValuePropertyName, assertions.value_property);
  CaptureMethodCacheObservation(widget_instance, kCountGetterSelector,
                                assertions.count_entry);
  CaptureMethodCacheObservation(widget_instance, kCountSetterSelector,
                                assertions.set_count_entry);
  CaptureMethodCacheObservation(widget_instance, kEnabledGetterSelector,
                                assertions.enabled_entry);
  CaptureMethodCacheObservation(widget_instance, kEnabledSetterSelector,
                                assertions.set_enabled_entry);
  CaptureMethodCacheObservation(widget_instance, kValueGetterSelector,
                                assertions.value_entry);
  CaptureMethodCacheObservation(widget_instance, kValueSetterSelector,
                                assertions.set_value_entry);
}

}  // namespace objc3c::runtime::probe::synthesized_accessor

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_ACCESSOR_ASSERTION_HELPERS_H_
