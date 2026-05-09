#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface_owners.h"

namespace objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening {

void PublishReadiness(
    Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
        &surface,
    const Objc3FrontendPipelineResult &pipeline_result) {
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;
  const Objc3LoweringPipelinePassGraphCoreFeatureSurface &pass_graph_surface =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface;

  surface.parse_recovery_determinism_consistent =
      parse_surface.parse_recovery_determinism_hardening_consistent &&
      parse_surface.long_tail_grammar_recovery_determinism_consistent &&
      parse_surface
          .parser_diagnostic_grammar_hooks_recovery_determinism_consistent;
  surface.parse_recovery_determinism_ready =
      parse_surface.long_tail_grammar_recovery_determinism_ready &&
      parse_surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready;

  surface.recovery_determinism_consistent =
      surface.diagnostics_hardening_consistent &&
      surface.parse_recovery_determinism_consistent &&
      surface.semantic_recovery_determinism_consistent &&
      pass_graph_surface.recovery_determinism_consistent;
  const bool recovery_determinism_replay_keys_ready =
      !surface.diagnostics_hardening_key.empty() &&
      !surface.parse_recovery_determinism_hardening_key.empty() &&
      !surface.long_tail_grammar_recovery_determinism_key.empty() &&
      !surface
           .parser_diagnostic_grammar_hooks_recovery_determinism_key.empty() &&
      !surface.semantic_recovery_determinism_key.empty() &&
      !surface.lowering_pipeline_recovery_determinism_key.empty();
  surface.recovery_determinism_key =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningKey(
          surface);
  surface.recovery_determinism_ready =
      surface.diagnostics_hardening_ready &&
      surface.parse_recovery_determinism_ready &&
      surface.semantic_recovery_determinism_ready &&
      surface.lowering_pipeline_recovery_determinism_ready &&
      surface.recovery_determinism_consistent &&
      recovery_determinism_replay_keys_ready &&
      !surface.recovery_determinism_key.empty();
}

}  // namespace objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening
