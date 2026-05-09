#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface_owners.h"

namespace objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation {

void PublishFailureReason(
    Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface
        &surface) {
  if (surface.conformance_matrix_ready) {
    return;
  }

  const bool conformance_matrix_replay_keys_ready =
      !surface.recovery_determinism_key.empty() &&
      !surface.parse_lowering_conformance_matrix_key.empty() &&
      !surface.long_tail_grammar_conformance_matrix_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_conformance_matrix_key.empty() &&
      !surface.semantic_conformance_matrix_key.empty() &&
      !surface.lowering_pipeline_conformance_matrix_key.empty();

  if (!surface.recovery_determinism_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics recovery/determinism hardening is not ready";
  } else if (!surface.parse_conformance_matrix_consistent) {
    surface.failure_reason = "parse conformance-matrix surfaces are inconsistent";
  } else if (!surface.parse_conformance_matrix_ready) {
    surface.failure_reason = "parse conformance-matrix surfaces are not ready";
  } else if (!surface.semantic_conformance_matrix_consistent) {
    surface.failure_reason = "semantic conformance-matrix surface is inconsistent";
  } else if (!surface.semantic_conformance_matrix_ready) {
    surface.failure_reason = "semantic conformance-matrix surface is not ready";
  } else if (!surface.lowering_pipeline_conformance_matrix_ready) {
    surface.failure_reason =
        "lowering pipeline conformance-matrix prerequisite is not ready";
  } else if (!surface.conformance_matrix_consistent) {
    surface.failure_reason =
        "lowering/runtime diagnostics surfacing conformance matrix is inconsistent";
  } else if (!conformance_matrix_replay_keys_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics surfacing conformance matrix replay keys are not ready";
  } else {
    surface.failure_reason =
        "lowering/runtime diagnostics surfacing conformance matrix is not ready";
  }
}

}  // namespace objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation
