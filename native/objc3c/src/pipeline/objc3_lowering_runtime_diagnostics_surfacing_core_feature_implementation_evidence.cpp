#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface_owners.h"

namespace objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation {

void PopulateEvidence(
    Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
        &surface,
    const Objc3FrontendPipelineResult &pipeline_result) {
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;
  const Objc3LoweringRuntimeDiagnosticsSurfacingScaffold &scaffold =
      pipeline_result.lowering_runtime_diagnostics_surfacing_scaffold;

  surface.lexer_diagnostic_count = pipeline_result.stage_diagnostics.lexer.size();
  surface.parser_diagnostic_count =
      pipeline_result.stage_diagnostics.parser.size();
  surface.semantic_diagnostic_count =
      pipeline_result.stage_diagnostics.semantic.size();
  surface.diagnostics_surfacing_scaffold_ready = scaffold.modular_split_ready;
  surface.parser_diagnostic_surface_consistent =
      parse_surface.parser_diagnostic_surface_consistent;
  surface.parser_diagnostic_code_surface_deterministic =
      parse_surface.parser_diagnostic_code_surface_deterministic;
  surface.semantic_diagnostics_deterministic =
      parse_surface.semantic_diagnostics_deterministic;
  surface.parse_artifact_replay_key = scaffold.parse_artifact_replay_key;
  surface.lowering_boundary_replay_key = scaffold.lowering_boundary_replay_key;
  surface.diagnostics_hardening_key =
      parse_surface.parse_artifact_diagnostics_hardening_key;
}

}  // namespace objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation
