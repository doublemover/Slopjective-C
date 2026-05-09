#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface_owners.h"

namespace objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening {

void PublishFailureReason(
    Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
        &surface) {
  if (surface.recovery_determinism_ready) {
    return;
  }

  const bool recovery_determinism_replay_keys_ready =
      !surface.diagnostics_hardening_key.empty() &&
      !surface.parse_recovery_determinism_hardening_key.empty() &&
      !surface.long_tail_grammar_recovery_determinism_key.empty() &&
      !surface
           .parser_diagnostic_grammar_hooks_recovery_determinism_key.empty() &&
      !surface.semantic_recovery_determinism_key.empty() &&
      !surface.lowering_pipeline_recovery_determinism_key.empty();

  if (!surface.diagnostics_hardening_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics hardening is not ready";
  } else if (!surface.parse_recovery_determinism_consistent) {
    surface.failure_reason = "parse recovery/determinism surfaces are inconsistent";
  } else if (!surface.parse_recovery_determinism_ready) {
    surface.failure_reason = "parse recovery/determinism surfaces are not ready";
  } else if (!surface.semantic_recovery_determinism_consistent) {
    surface.failure_reason =
        "semantic recovery/determinism surfaces are inconsistent";
  } else if (!surface.semantic_recovery_determinism_ready) {
    surface.failure_reason = "semantic recovery/determinism surfaces are not ready";
  } else if (!surface.lowering_pipeline_recovery_determinism_ready) {
    surface.failure_reason =
        "lowering pipeline recovery/determinism prerequisites are not ready";
  } else if (!surface.recovery_determinism_consistent) {
    surface.failure_reason =
        "lowering/runtime diagnostics recovery/determinism hardening is inconsistent";
  } else if (!recovery_determinism_replay_keys_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics recovery/determinism replay keys are not ready";
  } else {
    surface.failure_reason =
        "lowering/runtime diagnostics recovery/determinism hardening is not ready";
  }
}

}  // namespace objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening
