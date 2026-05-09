#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface_owners.h"

namespace objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion {

void PopulateEvidence(
    Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result) {
  const Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
      &core_surface =
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface;
  const Objc3LoweringRuntimeDiagnosticsSurfacingScaffold &scaffold =
      pipeline_result.lowering_runtime_diagnostics_surfacing_scaffold;
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;

  surface.core_feature_impl_ready = core_surface.core_feature_impl_ready;
  surface.diagnostics_surfacing_scaffold_ready = scaffold.modular_split_ready;
  surface.lexer_diagnostic_count = core_surface.lexer_diagnostic_count;
  surface.parser_diagnostic_count = core_surface.parser_diagnostic_count;
  surface.semantic_diagnostic_count = core_surface.semantic_diagnostic_count;
  surface.parser_diagnostic_code_count =
      parse_surface.parser_diagnostic_code_count;
  surface.parse_artifact_replay_key = core_surface.parse_artifact_replay_key;
  surface.lowering_boundary_replay_key =
      core_surface.lowering_boundary_replay_key;
  surface.diagnostics_hardening_key = core_surface.diagnostics_hardening_key;
  surface.core_feature_key = core_surface.core_feature_key;
}

}  // namespace objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion
