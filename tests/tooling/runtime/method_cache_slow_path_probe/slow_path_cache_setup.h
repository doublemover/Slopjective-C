#pragma once

#include "cache_assertion_helpers.h"
#include "probe_state.h"
#include "runtime_snapshot_helpers.h"
#include "selector_class_fixtures.h"

namespace objc3c {
namespace tooling {
namespace method_cache_slow_path_probe {

inline void CaptureStartupState(SlowPathProbeRun &run) {
  run.registration = CaptureRegistrationState();
  run.selector_table = CaptureSelectorTableState();
}

inline void CaptureInstanceDispatches(SlowPathProbeRun &run) {
  run.instance_first = CallWidgetCurrentValue();
  run.instance_first_state = CaptureMethodCacheState();

  run.instance_second = CallWidgetCurrentValue();
  run.instance_second_state = CaptureMethodCacheState();
}

inline void CaptureClassDispatches(SlowPathProbeRun &run) {
  run.class_self = CallWidgetSharedThroughSelf();
  run.class_self_state = CaptureMethodCacheState();

  run.known_class = CallWidgetSharedThroughKnownClass();
  run.known_class_state = CaptureMethodCacheState();
}

inline void CaptureStrictErrorDispatches(SlowPathProbeRun &run) {
  run.strict_error_first = DispatchStrictErrorSelector();
  run.strict_error_expected = ExpectedStrictDispatchValue();
  run.strict_error_first_state = CaptureMethodCacheState();

  run.strict_error_second = DispatchStrictErrorSelector();
  run.strict_error_second_state = CaptureMethodCacheState();
}

inline void CaptureCacheEntries(SlowPathProbeRun &run) {
  run.instance_entry =
      CaptureMethodCacheEntry(kWidgetInstanceClassId, kCurrentValueSelector);
  run.class_entry =
      CaptureMethodCacheEntry(kWidgetClassObjectId, kSharedSelector);
  run.strict_error_entry =
      CaptureMethodCacheEntry(kWidgetInstanceClassId, kStrictErrorSelector);
}

inline SlowPathProbeRun CaptureSlowPathProbeRun() {
  SlowPathProbeRun run{};

  CaptureStartupState(run);
  CaptureInstanceDispatches(run);
  CaptureClassDispatches(run);
  CaptureStrictErrorDispatches(run);
  CaptureCacheEntries(run);

  return run;
}

} // namespace method_cache_slow_path_probe
} // namespace tooling
} // namespace objc3c
