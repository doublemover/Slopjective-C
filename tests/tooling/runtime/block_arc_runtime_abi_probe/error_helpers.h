#pragma once

#include "arc_abi_assertions.h"
#include "probe_state.h"

namespace objc3c {
namespace runtime {
namespace probe {
namespace block_arc_runtime_abi {

constexpr int kProbeSuccessExitCode = 0;
constexpr int kProbeFailureExitCode = 1;

inline int ExitCodeForProbeResult(const ProbeResult &result) {
  return BlockArcRuntimeAbiAssertionsPassed(result) ? kProbeSuccessExitCode
                                                   : kProbeFailureExitCode;
}

} // namespace block_arc_runtime_abi
} // namespace probe
} // namespace runtime
} // namespace objc3c
