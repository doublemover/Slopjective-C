#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface.h"

bool
IsObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurfaceReady(
    const Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
        &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "lowering/runtime diagnostics core feature implementation is not ready"
               : surface.failure_reason;
  return false;
}
