#pragma once

#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface.h"

namespace objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation {

void PopulateEvidence(
    Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
        &surface,
    const Objc3FrontendPipelineResult &pipeline_result);

void PublishReadiness(
    Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
        &surface,
    const Objc3FrontendPipelineResult &pipeline_result);

void PublishFailureReason(
    Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
        &surface);

}  // namespace objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation
