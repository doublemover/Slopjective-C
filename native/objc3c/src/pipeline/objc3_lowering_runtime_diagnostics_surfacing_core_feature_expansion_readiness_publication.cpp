#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface_owners.h"

namespace objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion {

void PublishReadiness(
    Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result) {
  const Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
      &core_surface =
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface;
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;

  const bool stage_diagnostic_count_consistent =
      surface.lexer_diagnostic_count == parse_surface.lexer_diagnostic_count &&
      surface.parser_diagnostic_count == parse_surface.parser_diagnostic_count &&
      surface.semantic_diagnostic_count ==
          parse_surface.semantic_diagnostic_count;
  const bool parser_code_accounting_consistent =
      surface.parser_diagnostic_code_count <= surface.parser_diagnostic_count &&
      parse_surface.parser_diagnostic_code_surface_deterministic;
  surface.diagnostics_hardening_key_consistent =
      !surface.diagnostics_hardening_key.empty() &&
      surface.diagnostics_hardening_key ==
          parse_surface.parse_artifact_diagnostics_hardening_key &&
      !parse_surface.long_tail_grammar_diagnostics_hardening_key.empty();
  surface.diagnostics_payload_accounting_consistent =
      stage_diagnostic_count_consistent &&
      parser_code_accounting_consistent &&
      core_surface.stage_diagnostics_bus_consistent &&
      core_surface.parser_diagnostic_surface_consistent &&
      core_surface.parser_diagnostic_code_surface_deterministic &&
      core_surface.semantic_diagnostics_deterministic;
  surface.expansion_replay_keys_ready =
      !surface.parse_artifact_replay_key.empty() &&
      !surface.lowering_boundary_replay_key.empty() &&
      !surface.diagnostics_hardening_key.empty() &&
      !surface.core_feature_key.empty() &&
      !parse_surface.parser_diagnostic_source_precision_scaffold_key.empty();
  surface.lowering_pipeline_expansion_ready =
      pipeline_result.ir_emission_completeness_scaffold.expansion_ready &&
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .expansion_ready;
  surface.core_feature_expansion_ready =
      surface.core_feature_impl_ready &&
      surface.diagnostics_surfacing_scaffold_ready &&
      surface.diagnostics_hardening_key_consistent &&
      surface.diagnostics_payload_accounting_consistent &&
      surface.expansion_replay_keys_ready &&
      surface.lowering_pipeline_expansion_ready;
  surface.expansion_key =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionKey(
          surface);
}

}  // namespace objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion
