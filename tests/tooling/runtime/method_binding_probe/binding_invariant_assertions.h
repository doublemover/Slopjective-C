#pragma once

#include "method_fixture_definitions.h"
#include "probe_state.h"

namespace objc3c {
namespace tooling {
namespace method_binding_probe {

inline bool RuntimeSnapshotCopiesSucceeded(const MethodBindingProbeRun &run) {
  return run.registration.status == 0 && run.selector_table.status == 0 &&
         run.instance_first_state.status == 0 &&
         run.instance_second_state.status == 0 && run.class_state.status == 0 &&
         run.known_class_state.status == 0 && run.category_state.status == 0 &&
         run.instance_entry.status == 0 && run.class_entry.status == 0 &&
         run.category_entry.status == 0;
}

inline bool MethodBindingReturnValuesMatch(const MethodBindingProbeRun &run) {
  return run.values.instance_first == kExpectedInstanceValue &&
         run.values.instance_second == kExpectedInstanceValue &&
         run.values.class_value == kExpectedClassValue &&
         run.values.known_class_value == kExpectedClassValue &&
         run.values.category_value == kExpectedCategoryValue;
}

inline bool MethodBindingDispatchesResolveLiveMethods(
    const MethodBindingProbeRun &run) {
  return run.instance_first_state.state.last_dispatch_resolved_live_method ==
             1 &&
         run.instance_second_state.state.last_dispatch_resolved_live_method ==
             1 &&
         run.class_state.state.last_dispatch_resolved_live_method == 1 &&
         run.known_class_state.state.last_dispatch_resolved_live_method == 1 &&
         run.category_state.state.last_dispatch_resolved_live_method == 1 &&
         run.instance_first_state.state.last_dispatch_strict_error == 0 &&
         run.instance_second_state.state.last_dispatch_strict_error == 0 &&
         run.class_state.state.last_dispatch_strict_error == 0 &&
         run.known_class_state.state.last_dispatch_strict_error == 0 &&
         run.category_state.state.last_dispatch_strict_error == 0;
}

inline bool MethodBindingEntriesResolve(const MethodBindingProbeRun &run) {
  return run.instance_entry.entry.found == 1 &&
         run.instance_entry.entry.resolved == 1 &&
         run.class_entry.entry.found == 1 &&
         run.class_entry.entry.resolved == 1 &&
         run.category_entry.entry.found == 1 &&
         run.category_entry.entry.resolved == 1;
}

inline bool MethodBindingProbeInvariantsSatisfied(
    const MethodBindingProbeRun &run) {
  return RuntimeSnapshotCopiesSucceeded(run) &&
         MethodBindingReturnValuesMatch(run) &&
         MethodBindingDispatchesResolveLiveMethods(run) &&
         MethodBindingEntriesResolve(run);
}

} // namespace method_binding_probe
} // namespace tooling
} // namespace objc3c
