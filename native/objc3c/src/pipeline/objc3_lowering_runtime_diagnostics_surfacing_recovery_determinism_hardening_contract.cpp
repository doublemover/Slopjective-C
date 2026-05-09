#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface.h"

bool
IsObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurfaceReady(
    const Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
        &surface,
    std::string &reason) {
  if (surface.recovery_determinism_ready) {
    reason.clear();
    return true;
  }

  reason =
      surface.failure_reason.empty()
          ? "lowering/runtime diagnostics recovery/determinism hardening is not ready"
          : surface.failure_reason;
  return false;
}
