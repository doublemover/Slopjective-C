#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface.h"

bool
IsObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurfaceReady(
    const Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface
        &surface,
    std::string &reason) {
  if (surface.conformance_matrix_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "lowering/runtime diagnostics surfacing conformance matrix is not ready"
               : surface.failure_reason;
  return false;
}
