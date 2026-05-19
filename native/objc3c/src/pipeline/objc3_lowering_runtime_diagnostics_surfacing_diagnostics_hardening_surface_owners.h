#pragma once

#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface.h"

namespace objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening {

void PopulateEvidence(
    Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result);

void PublishReadiness(
    Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result);

void PublishFailureReason(
    Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface &surface);

}  // namespace objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening
