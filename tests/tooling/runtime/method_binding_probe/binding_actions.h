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
  run.registration = CaptureRegistrationState();
  run.selector_table = CaptureSelectorTableState();
}

inline void CaptureInstanceMethodBindings(MethodBindingProbeRun &run) {
  run.values.instance_first = DispatchRuntimeMethod(kInstanceValueDispatch);
  run.instance_first_state = CaptureMethodCacheState();

  run.values.instance_second = DispatchRuntimeMethod(kInstanceValueDispatch);
  run.instance_second_state = CaptureMethodCacheState();
}

inline void CaptureClassMethodBindings(MethodBindingProbeRun &run) {
  run.values.class_value = DispatchRuntimeMethod(kClassValueDispatch);
  run.class_state = CaptureMethodCacheState();

  run.values.known_class_value = DispatchRuntimeMethod(kKnownClassValueDispatch);
  run.known_class_state = CaptureMethodCacheState();
}

inline void CaptureCategoryMethodBinding(MethodBindingProbeRun &run) {
  run.values.category_value = DispatchRuntimeMethod(kCategoryValueDispatch);
  run.category_state = CaptureMethodCacheState();
}

inline void CaptureMethodBindingEntries(MethodBindingProbeRun &run) {
  run.instance_entry = CaptureMethodCacheEntry(kInstanceValueEntry);
  run.class_entry = CaptureMethodCacheEntry(kClassValueEntry);
  run.category_entry = CaptureMethodCacheEntry(kCategoryValueEntry);
}

inline MethodBindingProbeRun CaptureMethodBindingProbeRun() {
  MethodBindingProbeRun run{};

  CaptureStartupRuntimeState(run);
  CaptureInstanceMethodBindings(run);
  CaptureClassMethodBindings(run);
  CaptureCategoryMethodBinding(run);
  CaptureMethodBindingEntries(run);

  return run;
}

} // namespace method_binding_probe
} // namespace tooling
} // namespace objc3c
