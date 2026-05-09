#include "pipeline/objc3_ownership_aware_lowering_behavior_scaffold_owners.h"

namespace objc3_ownership_aware_lowering_behavior_scaffold {

void PublishReadiness(
    Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    const Objc3OwnershipQualifierLoweringContract &ownership_qualifier_contract,
    const Objc3RetainReleaseOperationLoweringContract &retain_release_contract,
    const Objc3WeakUnownedSemanticsLoweringContract &weak_unowned_semantics_contract,
    const Objc3ArcDiagnosticsFixitLoweringContract &arc_diagnostics_fixit_contract) {
  scaffold.ownership_profile_accounting_consistent =
      ownership_qualifier_contract.object_pointer_type_annotation_sites >=
          ownership_qualifier_contract.ownership_qualifier_sites &&
      retain_release_contract.retain_insertion_sites <=
          retain_release_contract.ownership_qualified_sites +
              retain_release_contract.contract_violation_sites &&
      retain_release_contract.release_insertion_sites <=
          retain_release_contract.ownership_qualified_sites +
              retain_release_contract.contract_violation_sites &&
      retain_release_contract.autorelease_insertion_sites <=
          retain_release_contract.ownership_qualified_sites +
              retain_release_contract.contract_violation_sites &&
      weak_unowned_semantics_contract.contract_violation_sites <=
          weak_unowned_semantics_contract.ownership_candidate_sites +
              weak_unowned_semantics_contract.weak_unowned_conflict_sites &&
      arc_diagnostics_fixit_contract.ownership_arc_fixit_available_sites <=
          arc_diagnostics_fixit_contract.ownership_arc_diagnostic_candidate_sites +
              arc_diagnostics_fixit_contract.contract_violation_sites;
  scaffold.replay_keys_ready =
      !scaffold.ownership_qualifier_replay_key.empty() &&
      !scaffold.retain_release_replay_key.empty() &&
      !scaffold.autoreleasepool_scope_replay_key.empty() &&
      !scaffold.arc_diagnostics_fixit_replay_key.empty();
  scaffold.deterministic_replay_surface =
      HasOwnershipLaneContractReplaySuffix(scaffold.ownership_qualifier_replay_key,
                                           kObjc3OwnershipQualifierLoweringLaneContract) &&
      HasOwnershipLaneContractReplaySuffix(scaffold.retain_release_replay_key,
                                           kObjc3RetainReleaseOperationLoweringLaneContract) &&
      HasOwnershipLaneContractReplaySuffix(scaffold.autoreleasepool_scope_replay_key,
                                           kObjc3AutoreleasePoolScopeLoweringLaneContract) &&
      HasOwnershipLaneContractReplaySuffix(scaffold.arc_diagnostics_fixit_replay_key,
                                           kObjc3ArcDiagnosticsFixitLoweringLaneContract);
  scaffold.modular_split_ready =
      scaffold.ownership_qualifier_contract_ready &&
      scaffold.retain_release_contract_ready &&
      scaffold.autoreleasepool_scope_contract_ready &&
      scaffold.arc_diagnostics_fixit_contract_ready &&
      scaffold.replay_keys_ready &&
      scaffold.deterministic_replay_surface;
  scaffold.scaffold_key = BuildObjc3OwnershipAwareLoweringBehaviorScaffoldKey(scaffold);
  scaffold.expansion_replay_keys_ready =
      scaffold.replay_keys_ready &&
      !scaffold.weak_unowned_semantics_replay_key.empty();
  scaffold.expansion_deterministic_replay_surface =
      scaffold.deterministic_replay_surface &&
      HasOwnershipLaneContractReplaySuffix(
          scaffold.weak_unowned_semantics_replay_key,
          kObjc3WeakUnownedSemanticsLoweringLaneContract);
  scaffold.expansion_ready =
      scaffold.modular_split_ready &&
      scaffold.weak_unowned_semantics_contract_ready &&
      scaffold.ownership_profile_accounting_consistent &&
      scaffold.expansion_replay_keys_ready &&
      scaffold.expansion_deterministic_replay_surface;
  scaffold.expansion_key =
      BuildObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionKey(scaffold);
  scaffold.edge_case_compatibility_ready =
      scaffold.expansion_ready &&
      scaffold.compatibility_handoff_consistent &&
      scaffold.language_version_pragma_coordinate_order_consistent &&
      scaffold.parse_artifact_edge_case_robustness_consistent &&
      scaffold.parse_artifact_replay_key_deterministic &&
      !scaffold.compatibility_handoff_key.empty() &&
      !scaffold.parse_artifact_edge_robustness_key.empty();
  scaffold.edge_case_compatibility_key =
      BuildObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityKey(scaffold);
  scaffold.edge_case_expansion_consistent =
      scaffold.edge_case_compatibility_ready &&
      scaffold.compatibility_handoff_consistent &&
      scaffold.parse_artifact_edge_case_robustness_consistent &&
      scaffold.parse_artifact_replay_key_deterministic;
  scaffold.edge_case_robustness_ready =
      scaffold.edge_case_expansion_consistent &&
      !scaffold.edge_case_compatibility_key.empty() &&
      !scaffold.compatibility_handoff_key.empty() &&
      !scaffold.parse_artifact_edge_robustness_key.empty();
  scaffold.edge_case_robustness_key =
      BuildObjc3OwnershipAwareLoweringBehaviorEdgeCaseRobustnessKey(scaffold);
  scaffold.diagnostics_hardening_consistent =
      scaffold.edge_case_robustness_ready &&
      scaffold.parse_artifact_edge_case_robustness_consistent &&
      scaffold.parse_artifact_replay_key_deterministic &&
      scaffold.arc_diagnostics_fixit_contract_ready &&
      scaffold.ownership_profile_accounting_consistent &&
      !scaffold.arc_diagnostics_fixit_replay_key.empty();
  scaffold.diagnostics_hardening_key =
      BuildObjc3OwnershipAwareLoweringBehaviorDiagnosticsHardeningKey(scaffold);
  scaffold.diagnostics_hardening_ready =
      scaffold.diagnostics_hardening_consistent &&
      !scaffold.diagnostics_hardening_key.empty() &&
      !scaffold.edge_case_robustness_key.empty();
  scaffold.recovery_determinism_consistent =
      scaffold.diagnostics_hardening_ready &&
      scaffold.parse_recovery_determinism_hardening_consistent &&
      scaffold.parse_artifact_replay_key_deterministic &&
      !scaffold.parse_recovery_determinism_hardening_key.empty();
  scaffold.recovery_determinism_key =
      BuildObjc3OwnershipAwareLoweringBehaviorRecoveryDeterminismKey(scaffold);
  scaffold.recovery_determinism_ready =
      scaffold.recovery_determinism_consistent &&
      !scaffold.recovery_determinism_key.empty() &&
      !scaffold.diagnostics_hardening_key.empty();
  scaffold.conformance_matrix_consistent =
      scaffold.recovery_determinism_ready &&
      scaffold.parse_lowering_conformance_matrix_consistent &&
      scaffold.parse_artifact_replay_key_deterministic;
  scaffold.conformance_matrix_key =
      BuildObjc3OwnershipAwareLoweringBehaviorConformanceMatrixKey(scaffold);
  scaffold.conformance_matrix_ready =
      scaffold.conformance_matrix_consistent &&
      !scaffold.conformance_matrix_key.empty() &&
      !scaffold.parse_lowering_conformance_matrix_key.empty() &&
      !scaffold.recovery_determinism_key.empty();
  scaffold.conformance_corpus_consistent =
      scaffold.conformance_matrix_ready &&
      scaffold.parse_lowering_conformance_corpus_consistent &&
      scaffold.parse_lowering_conformance_corpus_case_count > 0 &&
      scaffold.parse_artifact_replay_key_deterministic;
  scaffold.conformance_corpus_key =
      BuildObjc3OwnershipAwareLoweringBehaviorConformanceCorpusKey(scaffold);
  scaffold.conformance_corpus_ready =
      scaffold.conformance_corpus_consistent &&
      !scaffold.conformance_corpus_key.empty() &&
      !scaffold.parse_lowering_conformance_corpus_key.empty();
  const bool parse_lowering_performance_quality_guardrails_case_accounting_consistent =
      scaffold.parse_lowering_performance_quality_guardrails_case_count ==
          scaffold.parse_lowering_performance_quality_guardrails_passed_case_count +
              scaffold.parse_lowering_performance_quality_guardrails_failed_case_count &&
      scaffold.parse_lowering_performance_quality_guardrails_case_count > 0 &&
      scaffold.parse_lowering_performance_quality_guardrails_passed_case_count <=
          scaffold.parse_lowering_performance_quality_guardrails_case_count &&
      scaffold.parse_lowering_performance_quality_guardrails_failed_case_count ==
          (scaffold.parse_lowering_performance_quality_guardrails_case_count -
           scaffold.parse_lowering_performance_quality_guardrails_passed_case_count);
  scaffold.performance_quality_guardrails_consistent =
      scaffold.conformance_corpus_ready &&
      scaffold.parse_lowering_performance_quality_guardrails_consistent &&
      parse_lowering_performance_quality_guardrails_case_accounting_consistent &&
      scaffold.parse_artifact_replay_key_deterministic;
  scaffold.performance_quality_guardrails_key =
      BuildObjc3OwnershipAwareLoweringBehaviorPerformanceQualityGuardrailsKey(scaffold);
  scaffold.performance_quality_guardrails_ready =
      scaffold.performance_quality_guardrails_consistent &&
      scaffold.parse_lowering_performance_quality_guardrails_failed_case_count == 0 &&
      !scaffold.performance_quality_guardrails_key.empty() &&
      !scaffold.parse_lowering_performance_quality_guardrails_key.empty() &&
      !scaffold.conformance_corpus_key.empty();
  scaffold.cross_lane_integration_consistent =
      scaffold.performance_quality_guardrails_ready &&
      scaffold.lowering_pass_graph_performance_quality_guardrails_ready &&
      scaffold.conformance_corpus_ready &&
      scaffold.lowering_pass_graph_conformance_corpus_ready;
  scaffold.cross_lane_integration_ready =
      scaffold.cross_lane_integration_consistent &&
      !scaffold.performance_quality_guardrails_key.empty() &&
      !scaffold.lowering_pass_graph_performance_quality_guardrails_key.empty() &&
      !scaffold.conformance_corpus_key.empty() &&
      !scaffold.lowering_pass_graph_conformance_corpus_key.empty();
  scaffold.cross_lane_integration_key =
      BuildObjc3OwnershipAwareLoweringBehaviorCrossLaneIntegrationKey(scaffold);
}

}  // namespace objc3_ownership_aware_lowering_behavior_scaffold
