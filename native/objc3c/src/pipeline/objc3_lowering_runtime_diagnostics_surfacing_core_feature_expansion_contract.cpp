#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface.h"

bool
IsObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurfaceReady(
    const Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface
        &surface,
    std::string &reason) {
  if (surface.core_feature_expansion_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "lowering/runtime diagnostics core feature expansion is not ready"
               : surface.failure_reason;
  return false;
}
