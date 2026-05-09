#pragma once

#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface.h"

namespace objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation {

void PopulateEvidence(
    Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface
        &surface,
    const Objc3FrontendPipelineResult &pipeline_result);

void PublishReadiness(
    Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface
        &surface,
    const Objc3FrontendPipelineResult &pipeline_result);

void PublishFailureReason(
    Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface
        &surface);

}  // namespace objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation
