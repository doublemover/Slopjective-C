#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface_owners.h"

namespace objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation {

void PublishFailureReason(
    Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
        &surface) {
  if (surface.core_feature_impl_ready) {
    return;
  }

  if (!surface.stage_diagnostics_bus_consistent) {
    surface.failure_reason = "stage diagnostics bus accounting is inconsistent";
  } else if (!surface.parse_readiness_surface_ready) {
    surface.failure_reason = "parse-lowering readiness surface is not ready";
  } else if (!surface.diagnostics_surfacing_scaffold_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics surfacing scaffold is not ready";
  } else if (!surface.parser_diagnostic_surface_consistent) {
    surface.failure_reason = "parser diagnostic surface is inconsistent";
  } else if (!surface.parser_diagnostic_code_surface_deterministic) {
    surface.failure_reason =
        "parser diagnostic code surface is not deterministic";
  } else if (!surface.semantic_diagnostics_deterministic) {
    surface.failure_reason = "semantic diagnostics are not deterministic";
  } else if (!surface.diagnostics_hardening_consistent) {
    surface.failure_reason =
        "lowering/runtime diagnostics hardening is inconsistent";
  } else if (!surface.diagnostics_hardening_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics hardening is not ready";
  } else if (!surface.replay_keys_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics replay keys are not ready";
  } else if (!surface.lowering_pipeline_ready) {
    surface.failure_reason = "lowering pipeline prerequisites are not ready";
  } else {
    surface.failure_reason =
        "lowering/runtime diagnostics core feature implementation is not ready";
  }
}

}  // namespace objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation
