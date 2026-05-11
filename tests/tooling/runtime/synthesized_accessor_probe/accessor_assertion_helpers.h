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

inline MethodCacheEntryObservation CaptureMethodCacheObservation(
    int receiver, const char *selector) {
  MethodCacheEntryObservation observation;
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      receiver, selector, &observation.entry);
  StabilizeMethodCacheObservation(observation);
  return observation;
}

inline AccessorAssertions CaptureAccessorAssertions(int widget_instance) {
  AccessorAssertions assertions;
  assertions.count_entry =
      CaptureMethodCacheObservation(widget_instance, kCountGetterSelector);
  assertions.set_count_entry =
      CaptureMethodCacheObservation(widget_instance, kCountSetterSelector);
  assertions.enabled_entry =
      CaptureMethodCacheObservation(widget_instance, kEnabledGetterSelector);
  assertions.set_enabled_entry =
      CaptureMethodCacheObservation(widget_instance, kEnabledSetterSelector);
  assertions.value_entry =
      CaptureMethodCacheObservation(widget_instance, kValueGetterSelector);
  assertions.set_value_entry =
      CaptureMethodCacheObservation(widget_instance, kValueSetterSelector);
  return assertions;
}

}  // namespace objc3c::runtime::probe::synthesized_accessor

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_ACCESSOR_ASSERTION_HELPERS_H_
