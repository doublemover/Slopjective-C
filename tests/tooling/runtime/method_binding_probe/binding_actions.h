#pragma once

#include "method_fixture_definitions.h"
#include "probe_state.h"
#include "runtime_snapshot_helpers.h"

namespace objc3c {
namespace tooling {
namespace method_binding_probe {

inline int DispatchRuntimeMethod(const RuntimeDispatch &dispatch) {
  return objc3_runtime_dispatch_i32(dispatch.receiver, dispatch.selector,
                                    dispatch.arg0, dispatch.arg1,
                                    dispatch.arg2, dispatch.arg3);
}

inline void CaptureStartupRuntimeState(MethodBindingProbeRun &run) {
  CaptureRegistrationState(run.registration);
  CaptureSelectorTableState(run.selector_table);
}

inline void CaptureInstanceMethodBindings(MethodBindingProbeRun &run) {
  run.values.instance_first = DispatchRuntimeMethod(kInstanceValueDispatch);
  CaptureMethodCacheState(run.instance_first_state);

  run.values.instance_second = DispatchRuntimeMethod(kInstanceValueDispatch);
  CaptureMethodCacheState(run.instance_second_state);
}

inline void CaptureClassMethodBindings(MethodBindingProbeRun &run) {
  run.values.class_value = DispatchRuntimeMethod(kClassValueDispatch);
  CaptureMethodCacheState(run.class_state);

  run.values.known_class_value = DispatchRuntimeMethod(kKnownClassValueDispatch);
  CaptureMethodCacheState(run.known_class_state);
}

inline void CaptureCategoryMethodBinding(MethodBindingProbeRun &run) {
  run.values.category_value = DispatchRuntimeMethod(kCategoryValueDispatch);
  CaptureMethodCacheState(run.category_state);
}

inline void CaptureMethodBindingEntries(MethodBindingProbeRun &run) {
  CaptureMethodCacheEntry(kInstanceValueEntry, run.instance_entry);
  CaptureMethodCacheEntry(kClassValueEntry, run.class_entry);
  CaptureMethodCacheEntry(kCategoryValueEntry, run.category_entry);
}

inline void CaptureMethodBindingProbeRun(MethodBindingProbeRun &run) {
  run = MethodBindingProbeRun{};
  CaptureStartupRuntimeState(run);
  CaptureInstanceMethodBindings(run);
  CaptureClassMethodBindings(run);
  CaptureCategoryMethodBinding(run);
  CaptureMethodBindingEntries(run);
}

} // namespace method_binding_probe
} // namespace tooling
} // namespace objc3c
