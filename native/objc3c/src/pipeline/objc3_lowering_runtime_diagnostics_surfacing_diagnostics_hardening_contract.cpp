#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface.h"

bool IsObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurfaceReady(
    const Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface
        &surface,
    std::string &reason) {
  if (surface.diagnostics_hardening_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "lowering/runtime diagnostics hardening is not ready"
               : surface.failure_reason;
  return false;
}
