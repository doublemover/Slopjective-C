#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface_owners.h"

namespace objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening {

void PopulateEvidence(
    Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
        &surface,
    const Objc3FrontendPipelineResult &pipeline_result) {
  const Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface
      &diagnostics_surface =
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface;
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;
  const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningSurface
      &semantic_surface =
          pipeline_result
              .semantic_diagnostic_taxonomy_and_fixit_recovery_determinism_hardening_surface;
  const Objc3LoweringPipelinePassGraphCoreFeatureSurface &pass_graph_surface =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface;

  surface.diagnostics_hardening_consistent =
      diagnostics_surface.diagnostics_hardening_consistent;
  surface.diagnostics_hardening_ready = diagnostics_surface.diagnostics_hardening_ready;
  surface.semantic_recovery_determinism_consistent =
      semantic_surface.recovery_determinism_consistent;
  surface.semantic_recovery_determinism_ready =
      semantic_surface.recovery_determinism_ready;
  surface.lowering_pipeline_recovery_determinism_ready =
      pass_graph_surface.recovery_determinism_ready;

  surface.diagnostics_hardening_key = diagnostics_surface.diagnostics_hardening_key;
  surface.parse_recovery_determinism_hardening_key =
      parse_surface.parse_recovery_determinism_hardening_key;
  surface.long_tail_grammar_recovery_determinism_key =
      parse_surface.long_tail_grammar_recovery_determinism_key;
  surface.parser_diagnostic_grammar_hooks_recovery_determinism_key =
      parse_surface.parser_diagnostic_grammar_hooks_recovery_determinism_key;
  surface.semantic_recovery_determinism_key =
      semantic_surface.recovery_determinism_key;
  surface.lowering_pipeline_recovery_determinism_key =
      pass_graph_surface.recovery_determinism_key;
}

}  // namespace objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening
