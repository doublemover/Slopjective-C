#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface_owners.h"

Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
      surface;
  objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation::
      PopulateEvidence(surface, pipeline_result);
  objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation::
      PublishReadiness(surface, pipeline_result);
  objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation::
      PublishFailureReason(surface);
  return surface;
}
