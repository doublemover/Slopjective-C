#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface_owners.h"

Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface
BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface surface;
  objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion::
      PopulateEvidence(surface, pipeline_result);
  objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion::
      PublishReadiness(surface, pipeline_result);
  objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion::
      PublishFailureReason(surface);
  return surface;
}
