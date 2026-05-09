#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface_owners.h"

#include <cstddef>

namespace objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation {

void PublishReadiness(
    Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
        &surface,
    const Objc3FrontendPipelineResult &pipeline_result) {
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;
  const Objc3LoweringRuntimeDiagnosticsSurfacingScaffold &scaffold =
      pipeline_result.lowering_runtime_diagnostics_surfacing_scaffold;

  const std::size_t stage_diagnostics_total =
      surface.lexer_diagnostic_count + surface.parser_diagnostic_count +
      surface.semantic_diagnostic_count;
  surface.stage_diagnostics_bus_consistent =
      stage_diagnostics_total == pipeline_result.stage_diagnostics.size();
  surface.parse_readiness_surface_ready =
      parse_surface.ready_for_lowering && parse_surface.lowering_boundary_ready;

  surface.diagnostics_hardening_consistent =
      scaffold.parse_diagnostics_hardening_consistent &&
      parse_surface.parse_artifact_diagnostics_hardening_consistent &&
      parse_surface.long_tail_grammar_diagnostics_hardening_consistent &&
      surface.parser_diagnostic_surface_consistent &&
      surface.parser_diagnostic_code_surface_deterministic &&
      surface.semantic_diagnostics_deterministic;
  surface.diagnostics_hardening_ready =
      surface.diagnostics_hardening_consistent &&
      parse_surface.long_tail_grammar_diagnostics_hardening_ready &&
      !parse_surface.parse_artifact_diagnostics_hardening_key.empty() &&
      !parse_surface.long_tail_grammar_diagnostics_hardening_key.empty();

  surface.replay_keys_ready =
      !surface.parse_artifact_replay_key.empty() &&
      !surface.lowering_boundary_replay_key.empty() &&
      !surface.diagnostics_hardening_key.empty() &&
      !parse_surface.parser_diagnostic_source_precision_scaffold_key.empty();
  surface.lowering_pipeline_ready =
      pipeline_result.lowering_pipeline_pass_graph_scaffold.pass_graph_ready &&
      pipeline_result.ir_emission_completeness_scaffold.modular_split_ready &&
      pipeline_result.ir_emission_completeness_scaffold.core_feature_ready;

  surface.core_feature_impl_ready =
      surface.stage_diagnostics_bus_consistent &&
      surface.parse_readiness_surface_ready &&
      surface.diagnostics_surfacing_scaffold_ready &&
      surface.parser_diagnostic_surface_consistent &&
      surface.parser_diagnostic_code_surface_deterministic &&
      surface.semantic_diagnostics_deterministic &&
      surface.diagnostics_hardening_consistent &&
      surface.diagnostics_hardening_ready &&
      surface.replay_keys_ready &&
      surface.lowering_pipeline_ready;
  surface.core_feature_key =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationKey(
          surface);
}

}  // namespace objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation
