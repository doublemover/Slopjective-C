#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_MAIN_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_MAIN_ORCHESTRATION_H_

#include "orchestration.h"
#include "probe_state.h"
#include "report_helpers.h"

namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe {

inline int RunProbeMain() {
  const DispatchLookupProbeResult result = CaptureDispatchLookupProbeResult();
  PrintDispatchLookupProbeReport(result);
  return 0;
}

}  // namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_MAIN_ORCHESTRATION_H_
