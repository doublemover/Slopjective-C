#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface_owners.h"

namespace objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation {

void PublishReadiness(
    Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface
        &surface,
    const Objc3FrontendPipelineResult &pipeline_result) {
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;

  surface.parse_conformance_matrix_consistent =
      parse_surface.parse_lowering_conformance_matrix_consistent &&
      parse_surface.long_tail_grammar_conformance_matrix_consistent &&
      parse_surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent;
  surface.parse_conformance_matrix_ready =
      surface.parse_conformance_matrix_consistent &&
      parse_surface.long_tail_grammar_conformance_matrix_ready &&
      parse_surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready &&
      parse_surface.parse_lowering_conformance_matrix_case_count > 0;

  surface.conformance_matrix_consistent =
      surface.recovery_determinism_consistent &&
      surface.parse_conformance_matrix_consistent &&
      surface.semantic_conformance_matrix_consistent;
  const bool conformance_matrix_replay_keys_ready =
      !surface.recovery_determinism_key.empty() &&
      !surface.parse_lowering_conformance_matrix_key.empty() &&
      !surface.long_tail_grammar_conformance_matrix_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_conformance_matrix_key.empty() &&
      !surface.semantic_conformance_matrix_key.empty() &&
      !surface.lowering_pipeline_conformance_matrix_key.empty();

  surface.conformance_matrix_key =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationKey(
          surface);
  surface.conformance_matrix_ready =
      surface.recovery_determinism_ready &&
      surface.parse_conformance_matrix_ready &&
      surface.semantic_conformance_matrix_ready &&
      surface.lowering_pipeline_conformance_matrix_ready &&
      surface.conformance_matrix_consistent &&
      conformance_matrix_replay_keys_ready &&
      !surface.conformance_matrix_key.empty();
}

}  // namespace objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation
