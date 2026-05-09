#include "pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface_owners.h"

#include <cstddef>
#include <string>

namespace objc3_lowering_pipeline_pass_graph_core_feature {

void PublishReadiness(
    Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  surface.lowering_boundary_replay_key_consistent =
      !surface.lowering_boundary_replay_key.empty() &&
      !pipeline_result.typed_sema_to_lowering_contract_surface
           .lowering_boundary_replay_key.empty() &&
      !pipeline_result.parse_lowering_readiness_surface.lowering_boundary_replay_key
           .empty() &&
      surface.lowering_boundary_replay_key ==
          pipeline_result.typed_sema_to_lowering_contract_surface
              .lowering_boundary_replay_key &&
      surface.lowering_boundary_replay_key ==
          pipeline_result.parse_lowering_readiness_surface
              .lowering_boundary_replay_key;
  surface.runtime_dispatch_declaration_consistent =
      !surface.runtime_dispatch_declaration_replay_key.empty() &&
      surface.runtime_dispatch_declaration_replay_key.find("declare i32 @") == 0 &&
      surface.runtime_dispatch_declaration_replay_key.find(
          options.lowering.runtime_dispatch_symbol) != std::string::npos;
  surface.dispatch_shape_sharding_ready =
      pipeline_result.typed_sema_to_lowering_contract_surface
          .runtime_dispatch_contract_consistent &&
      pipeline_result.typed_sema_to_lowering_contract_surface
          .typed_core_feature_consistent;
  surface.llc_object_emission_route_deterministic =
      options.lowering.max_message_send_args <= kObjc3RuntimeDispatchMaxArgs &&
      IsValidRuntimeDispatchSymbol(options.lowering.runtime_dispatch_symbol);
  surface.replay_proof_artifact_key_ready =
      pipeline_result.parser_contract_snapshot.parser_recovery_replay_ready &&
      !pipeline_result.parse_lowering_readiness_surface
           .parse_artifact_replay_key.empty() &&
      !surface.lowering_boundary_replay_key.empty() &&
      !surface.runtime_dispatch_declaration_replay_key.empty();
  surface.edge_case_dispatch_shape_coverage_ready =
      pipeline_result.typed_sema_to_lowering_contract_surface
          .typed_core_feature_expansion_consistent &&
      pipeline_result.parse_lowering_readiness_surface
          .long_tail_grammar_expansion_ready &&
      options.lowering.max_message_send_args >= kObjc3RuntimeDispatchDefaultArgs &&
      options.lowering.max_message_send_args <= kObjc3RuntimeDispatchMaxArgs;
  surface.replay_proof_expansion_ready =
      surface.replay_proof_artifact_key_ready &&
      !pipeline_result.typed_sema_to_lowering_contract_surface
           .typed_core_feature_expansion_key.empty() &&
      !pipeline_result.parse_lowering_readiness_surface
           .long_tail_grammar_expansion_key.empty();
  surface.compatibility_handoff_consistent =
      pipeline_result.parse_lowering_readiness_surface
          .compatibility_handoff_consistent &&
      !pipeline_result.parse_lowering_readiness_surface.compatibility_handoff_key
           .empty();
  surface.language_version_pragma_coordinate_order_consistent =
      pipeline_result.parse_lowering_readiness_surface
          .language_version_pragma_coordinate_order_consistent;
  surface.core_feature_ready =
      surface.scaffold_ready &&
      surface.lowering_boundary_replay_key_consistent &&
      surface.runtime_dispatch_declaration_consistent &&
      surface.direct_ir_entrypoint_enabled &&
      surface.dispatch_shape_sharding_ready &&
      surface.llc_object_emission_route_deterministic &&
      surface.replay_proof_artifact_key_ready;
  surface.expansion_ready = surface.core_feature_ready &&
                            surface.edge_case_dispatch_shape_coverage_ready &&
                            surface.replay_proof_expansion_ready;
  surface.edge_case_compatibility_ready =
      surface.expansion_ready &&
      surface.compatibility_handoff_consistent &&
      surface.language_version_pragma_coordinate_order_consistent &&
      pipeline_result.parse_lowering_readiness_surface
          .long_tail_grammar_edge_case_compatibility_ready &&
      pipeline_result.typed_sema_to_lowering_contract_surface
          .typed_core_feature_expansion_consistent;
  surface.edge_case_compatibility_key =
      BuildObjc3LoweringPipelinePassGraphEdgeCaseCompatibilityKey(surface);
  surface.edge_case_expansion_consistent =
      surface.edge_case_compatibility_ready &&
      pipeline_result.parse_lowering_readiness_surface
          .long_tail_grammar_edge_case_expansion_consistent &&
      pipeline_result.parse_lowering_readiness_surface
          .parse_artifact_edge_case_robustness_consistent;
  surface.edge_case_robustness_ready =
      surface.edge_case_expansion_consistent &&
      pipeline_result.parse_lowering_readiness_surface
          .long_tail_grammar_edge_case_robustness_ready &&
      !pipeline_result.parse_lowering_readiness_surface
           .long_tail_grammar_edge_case_robustness_key.empty() &&
      !surface.edge_case_compatibility_key.empty();
  surface.edge_case_robustness_key =
      BuildObjc3LoweringPipelinePassGraphEdgeCaseRobustnessKey(surface);
  surface.diagnostics_hardening_consistent =
      surface.edge_case_robustness_ready &&
      pipeline_result.parse_lowering_readiness_surface
          .long_tail_grammar_diagnostics_hardening_consistent &&
      pipeline_result.parse_lowering_readiness_surface
          .parse_artifact_diagnostics_hardening_consistent;
  surface.diagnostics_hardening_ready =
      surface.diagnostics_hardening_consistent &&
      pipeline_result.parse_lowering_readiness_surface
          .long_tail_grammar_diagnostics_hardening_ready &&
      !pipeline_result.parse_lowering_readiness_surface
           .long_tail_grammar_diagnostics_hardening_key.empty() &&
      !pipeline_result.parse_lowering_readiness_surface
           .parse_artifact_diagnostics_hardening_key.empty() &&
      !surface.edge_case_robustness_key.empty();
  surface.diagnostics_hardening_key =
      BuildObjc3LoweringPipelinePassGraphDiagnosticsHardeningKey(surface);
  surface.recovery_determinism_consistent =
      surface.diagnostics_hardening_ready &&
      pipeline_result.parse_lowering_readiness_surface
          .long_tail_grammar_recovery_determinism_consistent &&
      pipeline_result.parse_lowering_readiness_surface
          .parse_recovery_determinism_hardening_consistent;
  surface.recovery_determinism_ready =
      surface.recovery_determinism_consistent &&
      pipeline_result.parse_lowering_readiness_surface
          .long_tail_grammar_recovery_determinism_ready &&
      !pipeline_result.parse_lowering_readiness_surface
           .long_tail_grammar_recovery_determinism_key.empty() &&
      !pipeline_result.parse_lowering_readiness_surface
           .parse_recovery_determinism_hardening_key.empty() &&
      !surface.diagnostics_hardening_key.empty();
  surface.recovery_determinism_key =
      BuildObjc3LoweringPipelinePassGraphRecoveryDeterminismKey(surface);
  surface.conformance_matrix_consistent =
      surface.recovery_determinism_ready &&
      pipeline_result.parse_lowering_readiness_surface
          .long_tail_grammar_conformance_matrix_consistent &&
      pipeline_result.parse_lowering_readiness_surface
          .parse_lowering_conformance_matrix_consistent;
  surface.conformance_matrix_ready =
      surface.conformance_matrix_consistent &&
      pipeline_result.parse_lowering_readiness_surface
          .long_tail_grammar_conformance_matrix_ready &&
      !pipeline_result.parse_lowering_readiness_surface
           .long_tail_grammar_conformance_matrix_key.empty() &&
      !pipeline_result.parse_lowering_readiness_surface
           .parse_lowering_conformance_matrix_key.empty() &&
      !surface.recovery_determinism_key.empty();
  surface.conformance_matrix_key =
      BuildObjc3LoweringPipelinePassGraphConformanceMatrixKey(surface);
  const std::size_t parse_lowering_conformance_corpus_case_count =
      pipeline_result.parse_lowering_readiness_surface
          .parse_lowering_conformance_corpus_case_count;
  const std::size_t parse_lowering_conformance_corpus_passed_case_count =
      pipeline_result.parse_lowering_readiness_surface
          .parse_lowering_conformance_corpus_passed_case_count;
  const std::size_t parse_lowering_conformance_corpus_failed_case_count =
      pipeline_result.parse_lowering_readiness_surface
          .parse_lowering_conformance_corpus_failed_case_count;
  const bool parse_lowering_conformance_corpus_case_accounting_consistent =
      parse_lowering_conformance_corpus_case_count > 0 &&
      parse_lowering_conformance_corpus_case_count ==
          parse_lowering_conformance_corpus_passed_case_count +
              parse_lowering_conformance_corpus_failed_case_count;
  surface.conformance_corpus_consistent =
      surface.conformance_matrix_consistent &&
      pipeline_result.parse_lowering_readiness_surface
          .parse_lowering_conformance_corpus_consistent;
  surface.conformance_corpus_ready =
      surface.conformance_corpus_consistent &&
      surface.conformance_matrix_ready &&
      parse_lowering_conformance_corpus_case_accounting_consistent &&
      !pipeline_result.parse_lowering_readiness_surface
           .parse_lowering_conformance_corpus_key.empty();
  surface.conformance_corpus_key =
      BuildObjc3LoweringPipelinePassGraphConformanceCorpusKey(surface);
  surface.parse_lowering_performance_quality_guardrails_case_count =
      pipeline_result.parse_lowering_readiness_surface
          .parse_lowering_performance_quality_guardrails_case_count;
  surface.parse_lowering_performance_quality_guardrails_passed_case_count =
      pipeline_result.parse_lowering_readiness_surface
          .parse_lowering_performance_quality_guardrails_passed_case_count;
  surface.parse_lowering_performance_quality_guardrails_failed_case_count =
      pipeline_result.parse_lowering_readiness_surface
          .parse_lowering_performance_quality_guardrails_failed_case_count;
  const bool parse_lowering_performance_quality_guardrails_case_accounting_consistent =
      surface.parse_lowering_performance_quality_guardrails_case_count > 0 &&
      surface.parse_lowering_performance_quality_guardrails_case_count ==
          surface.parse_lowering_performance_quality_guardrails_passed_case_count +
              surface.parse_lowering_performance_quality_guardrails_failed_case_count;
  surface.performance_quality_guardrails_consistent =
      surface.conformance_corpus_consistent &&
      pipeline_result.parse_lowering_readiness_surface
          .parse_lowering_performance_quality_guardrails_consistent;
  surface.performance_quality_guardrails_ready =
      surface.performance_quality_guardrails_consistent &&
      surface.conformance_corpus_ready &&
      parse_lowering_performance_quality_guardrails_case_accounting_consistent &&
      !pipeline_result.parse_lowering_readiness_surface
           .parse_lowering_performance_quality_guardrails_key.empty() &&
      !surface.conformance_corpus_key.empty();
  surface.performance_quality_guardrails_key =
      BuildObjc3LoweringPipelinePassGraphPerformanceQualityGuardrailsKey(
          surface);
  surface.expansion_key =
      BuildObjc3LoweringPipelinePassGraphCoreFeatureExpansionKey(surface);
  surface.core_feature_key =
      BuildObjc3LoweringPipelinePassGraphCoreFeatureKey(surface);
}

}  // namespace objc3_lowering_pipeline_pass_graph_core_feature
