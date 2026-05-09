#pragma once

#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface.h"

namespace objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion {

void PopulateEvidence(
    Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result);

void PublishReadiness(
    Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result);

void PublishFailureReason(
    Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface &surface);

}  // namespace objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion
