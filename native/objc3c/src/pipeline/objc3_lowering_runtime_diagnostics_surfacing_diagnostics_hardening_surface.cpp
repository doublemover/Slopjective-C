#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface_owners.h"

Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface
BuildObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface surface;
  objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening::
      PopulateEvidence(surface, pipeline_result);
  objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening::
      PublishReadiness(surface, pipeline_result);
  objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening::
      PublishFailureReason(surface);
  return surface;
}
