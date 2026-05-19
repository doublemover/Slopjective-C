#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface_owners.h"

namespace objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening {

void PopulateEvidence(
    Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result) {
  const Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface
      &edge_case_surface =
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface;
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;
  const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurface
      &semantic_surface =
          pipeline_result
              .semantic_diagnostic_taxonomy_and_fixit_diagnostics_hardening_surface;
  const Objc3LoweringPipelinePassGraphCoreFeatureSurface
      &pass_graph_core_feature_surface =
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface;

  surface.edge_case_robustness_consistent =
      edge_case_surface.edge_case_expansion_consistent;
  surface.edge_case_robustness_ready = edge_case_surface.edge_case_robustness_ready;
  surface.semantic_diagnostics_hardening_consistent =
      semantic_surface.diagnostics_hardening_consistent;
  surface.semantic_diagnostics_hardening_ready =
      semantic_surface.diagnostics_hardening_ready;
  surface.lowering_pipeline_diagnostics_hardening_ready =
      pass_graph_core_feature_surface.diagnostics_hardening_ready;

  surface.edge_case_robustness_key = edge_case_surface.edge_case_robustness_key;
  surface.parse_artifact_diagnostics_hardening_key =
      parse_surface.parse_artifact_diagnostics_hardening_key;
  surface.long_tail_grammar_diagnostics_hardening_key =
      parse_surface.long_tail_grammar_diagnostics_hardening_key;
  surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_key =
      parse_surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_key;
  surface.semantic_diagnostics_hardening_key =
      semantic_surface.diagnostics_hardening_key;
  surface.lowering_pipeline_diagnostics_hardening_key =
      pass_graph_core_feature_surface.diagnostics_hardening_key;
}

}  // namespace objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening
