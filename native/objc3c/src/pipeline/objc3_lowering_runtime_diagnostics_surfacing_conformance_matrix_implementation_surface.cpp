#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface_owners.h"

Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface
BuildObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface
      surface;
  objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation::
      PopulateEvidence(surface, pipeline_result);
  objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation::
      PublishReadiness(surface, pipeline_result);
  objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation::
      PublishFailureReason(surface);
  return surface;
}
