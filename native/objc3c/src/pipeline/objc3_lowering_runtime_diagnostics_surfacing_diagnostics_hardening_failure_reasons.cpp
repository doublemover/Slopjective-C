#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface_owners.h"

namespace objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening {

void PublishFailureReason(
    Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface &surface) {
  if (surface.diagnostics_hardening_ready) {
    return;
  }

  const bool diagnostics_hardening_replay_keys_ready =
      !surface.edge_case_robustness_key.empty() &&
      !surface.parse_artifact_diagnostics_hardening_key.empty() &&
      !surface.long_tail_grammar_diagnostics_hardening_key.empty() &&
      !surface
           .parser_diagnostic_grammar_hooks_diagnostics_hardening_key.empty() &&
      !surface.semantic_diagnostics_hardening_key.empty() &&
      !surface.lowering_pipeline_diagnostics_hardening_key.empty();

  if (!surface.edge_case_robustness_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics edge-case robustness is not ready";
  } else if (!surface.parse_diagnostics_hardening_consistent) {
    surface.failure_reason =
        "parse diagnostics hardening surfaces are inconsistent";
  } else if (!surface.parse_diagnostics_hardening_ready) {
    surface.failure_reason = "parse diagnostics hardening surfaces are not ready";
  } else if (!surface.semantic_diagnostics_hardening_consistent) {
    surface.failure_reason =
        "semantic diagnostics hardening surfaces are inconsistent";
  } else if (!surface.semantic_diagnostics_hardening_ready) {
    surface.failure_reason =
        "semantic diagnostics hardening surfaces are not ready";
  } else if (!surface.lowering_pipeline_diagnostics_hardening_ready) {
    surface.failure_reason =
        "lowering pipeline diagnostics hardening prerequisites are not ready";
  } else if (!surface.diagnostics_hardening_consistent) {
    surface.failure_reason =
        "lowering/runtime diagnostics hardening is inconsistent";
  } else if (!diagnostics_hardening_replay_keys_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics hardening replay keys are not ready";
  } else {
    surface.failure_reason =
        "lowering/runtime diagnostics hardening is not ready";
  }
}

}  // namespace objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening
