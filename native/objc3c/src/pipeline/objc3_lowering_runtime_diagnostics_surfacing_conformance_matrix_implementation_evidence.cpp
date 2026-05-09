#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface_owners.h"

namespace objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation {

void PopulateEvidence(
    Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface
        &surface,
    const Objc3FrontendPipelineResult &pipeline_result) {
  const Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
      &recovery_surface =
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface;
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;
  const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurface
      &semantic_surface =
          pipeline_result
              .semantic_diagnostic_taxonomy_and_fixit_conformance_matrix_implementation_surface;
  const Objc3LoweringPipelinePassGraphCoreFeatureSurface &pass_graph_surface =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface;

  surface.recovery_determinism_consistent =
      recovery_surface.recovery_determinism_consistent;
  surface.recovery_determinism_ready = recovery_surface.recovery_determinism_ready;
  surface.semantic_conformance_matrix_consistent =
      semantic_surface.conformance_matrix_consistent;
  surface.semantic_conformance_matrix_ready =
      semantic_surface.conformance_matrix_ready;
  surface.lowering_pipeline_conformance_matrix_ready =
      pass_graph_surface.conformance_matrix_ready;

  surface.parse_lowering_conformance_matrix_case_count =
      parse_surface.parse_lowering_conformance_matrix_case_count;

  surface.recovery_determinism_key = recovery_surface.recovery_determinism_key;
  surface.parse_lowering_conformance_matrix_key =
      parse_surface.parse_lowering_conformance_matrix_key;
  surface.long_tail_grammar_conformance_matrix_key =
      parse_surface.long_tail_grammar_conformance_matrix_key;
  surface.parser_diagnostic_grammar_hooks_conformance_matrix_key =
      parse_surface.parser_diagnostic_grammar_hooks_conformance_matrix_key;
  surface.semantic_conformance_matrix_key = semantic_surface.conformance_matrix_key;
  surface.lowering_pipeline_conformance_matrix_key =
      pass_graph_surface.conformance_matrix_key;
}

}  // namespace objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation
