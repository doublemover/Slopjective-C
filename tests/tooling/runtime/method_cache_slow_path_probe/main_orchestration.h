#pragma once

#include "report_helpers.h"
#include "slow_path_cache_setup.h"

namespace objc3c {
namespace tooling {
namespace method_cache_slow_path_probe {

inline int RunMethodCacheSlowPathProbe() {
  const SlowPathProbeRun run = CaptureSlowPathProbeRun();
  PrintMethodCacheSlowPathProbeReport(run);
  return 0;
}

} // namespace method_cache_slow_path_probe
} // namespace tooling
} // namespace objc3c
