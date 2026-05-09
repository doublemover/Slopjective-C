#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface_owners.h"

Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
BuildObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
      surface;
  objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening::
      PopulateEvidence(surface, pipeline_result);
  objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening::
      PublishReadiness(surface, pipeline_result);
  objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening::
      PublishFailureReason(surface);
  return surface;
}
