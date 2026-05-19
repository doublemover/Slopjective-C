#pragma once

#include "error_helpers.h"
#include "report_helpers.h"
#include "runtime_invocation_helpers.h"

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c {
namespace runtime {
namespace probe {
namespace block_arc_runtime_abi {

inline ProbeResult CaptureBlockArcRuntimeAbiProbe() {
  ::objc3_runtime_reset_for_testing();

  ProbeResult result;
  result.runtime = InvokeRuntimeHelpersForProbe();
  CaptureRuntimeAbiSnapshots(&result);
  return result;
}

inline int RunBlockArcRuntimeAbiProbe() {
  const ProbeResult result = CaptureBlockArcRuntimeAbiProbe();
  PrintBlockArcRuntimeAbiProbeReport(result);
  return ExitCodeForProbeResult(result);
}

} // namespace block_arc_runtime_abi
} // namespace probe
} // namespace runtime
} // namespace objc3c
