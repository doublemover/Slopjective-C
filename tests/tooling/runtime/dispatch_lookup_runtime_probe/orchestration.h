#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_ORCHESTRATION_H_

#include "dispatch_capture.h"
#include "dispatch_fixture_setup.h"
#include "lookup_capture.h"
#include "probe_state.h"
#include "registration_snapshot_capture.h"

namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe {

inline DispatchLookupProbeResult CaptureDispatchLookupProbeResult() {
  ResetDispatchLookupRuntime();

  DispatchLookupProbeResult result;
  result.register_status = RegisterDispatchLookupImage();
  result.selectors = CaptureSelectorLookups();
  result.dispatch = CaptureDispatchResults();
  result.registration = CaptureRegistrationSnapshot();

  ResetDispatchLookupRuntime();
  result.reset_lookup = CaptureSelectorLookupAfterReset();
  return result;
}

}  // namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_ORCHESTRATION_H_
