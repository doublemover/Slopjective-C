#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface_owners.h"

namespace objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening {

void PublishReadiness(
    Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result) {
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;
  const Objc3LoweringPipelinePassGraphCoreFeatureSurface
      &pass_graph_core_feature_surface =
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface;

  surface.parse_diagnostics_hardening_consistent =
      parse_surface.parse_artifact_diagnostics_hardening_consistent &&
      parse_surface.long_tail_grammar_diagnostics_hardening_consistent &&
      parse_surface
          .parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent;
  surface.parse_diagnostics_hardening_ready =
      parse_surface.long_tail_grammar_diagnostics_hardening_ready &&
      parse_surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready;

  surface.diagnostics_hardening_consistent =
      surface.edge_case_robustness_consistent &&
      surface.parse_diagnostics_hardening_consistent &&
      surface.semantic_diagnostics_hardening_consistent &&
      pass_graph_core_feature_surface.diagnostics_hardening_consistent;
  const bool diagnostics_hardening_replay_keys_ready =
      !surface.edge_case_robustness_key.empty() &&
      !surface.parse_artifact_diagnostics_hardening_key.empty() &&
      !surface.long_tail_grammar_diagnostics_hardening_key.empty() &&
      !surface
           .parser_diagnostic_grammar_hooks_diagnostics_hardening_key.empty() &&
      !surface.semantic_diagnostics_hardening_key.empty() &&
      !surface.lowering_pipeline_diagnostics_hardening_key.empty();
  surface.diagnostics_hardening_key =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningKey(
          surface);
  surface.diagnostics_hardening_ready =
      surface.edge_case_robustness_ready &&
      surface.parse_diagnostics_hardening_ready &&
      surface.semantic_diagnostics_hardening_ready &&
      surface.lowering_pipeline_diagnostics_hardening_ready &&
      surface.diagnostics_hardening_consistent &&
      diagnostics_hardening_replay_keys_ready &&
      !surface.diagnostics_hardening_key.empty();
}

}  // namespace objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening
