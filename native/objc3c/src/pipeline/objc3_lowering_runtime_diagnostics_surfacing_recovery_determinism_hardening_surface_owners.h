#pragma once

#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface.h"

namespace objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening {

void PopulateEvidence(
    Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
        &surface,
    const Objc3FrontendPipelineResult &pipeline_result);

void PublishReadiness(
    Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
        &surface,
    const Objc3FrontendPipelineResult &pipeline_result);

void PublishFailureReason(
    Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
        &surface);

}  // namespace objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening
