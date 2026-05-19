#pragma once

#include "cache_assertion_helpers.h"
#include "probe_state.h"
#include "runtime_snapshot_helpers.h"
#include "selector_class_fixtures.h"

namespace objc3c {
namespace tooling {
namespace method_cache_slow_path_probe {

inline void CaptureStartupState(SlowPathProbeRun &run) {
  CaptureRegistrationState(run.registration);
  CaptureSelectorTableState(run.selector_table);
}

inline void CaptureInstanceDispatches(SlowPathProbeRun &run) {
  run.instance_first = CallWidgetCurrentValue();
  CaptureMethodCacheState(run.instance_first_state);

  run.instance_second = CallWidgetCurrentValue();
  CaptureMethodCacheState(run.instance_second_state);
}

inline void CaptureClassDispatches(SlowPathProbeRun &run) {
  run.class_self = CallWidgetSharedThroughSelf();
  CaptureMethodCacheState(run.class_self_state);

  run.known_class = CallWidgetSharedThroughKnownClass();
  CaptureMethodCacheState(run.known_class_state);
}

inline void CaptureStrictErrorDispatches(SlowPathProbeRun &run) {
  run.strict_error_first = DispatchStrictErrorSelector();
  run.strict_error_expected = ExpectedStrictDispatchValue();
  CaptureMethodCacheState(run.strict_error_first_state);

  run.strict_error_second = DispatchStrictErrorSelector();
  CaptureMethodCacheState(run.strict_error_second_state);
}

inline void CaptureCacheEntries(SlowPathProbeRun &run) {
  CaptureMethodCacheEntry(kWidgetInstanceClassId, kCurrentValueSelector,
                          run.instance_entry);
  CaptureMethodCacheEntry(kWidgetClassObjectId, kSharedSelector,
                          run.class_entry);
  CaptureMethodCacheEntry(kWidgetInstanceClassId, kStrictErrorSelector,
                          run.strict_error_entry);
}

inline void CaptureSlowPathProbeRun(SlowPathProbeRun &run) {
  run = SlowPathProbeRun{};
  CaptureStartupState(run);
  CaptureInstanceDispatches(run);
  CaptureClassDispatches(run);
  CaptureStrictErrorDispatches(run);
  CaptureCacheEntries(run);
}

} // namespace method_cache_slow_path_probe
} // namespace tooling
} // namespace objc3c
