#pragma once

#include "binding_actions.h"
#include "binding_invariant_assertions.h"
#include "report_helpers.h"

namespace objc3c {
namespace tooling {
namespace method_binding_probe {

inline int RunMethodBindingProbe() {
  MethodBindingProbeRun run{};
  CaptureMethodBindingProbeRun(run);
  (void)MethodBindingProbeInvariantsSatisfied(run);
  PrintMethodBindingProbeReport(run);
  return 0;
}

} // namespace method_binding_probe
} // namespace tooling
} // namespace objc3c
