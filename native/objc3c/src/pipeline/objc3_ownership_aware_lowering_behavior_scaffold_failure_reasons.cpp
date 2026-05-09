#include "pipeline/objc3_ownership_aware_lowering_behavior_scaffold_owners.h"

namespace objc3_ownership_aware_lowering_behavior_scaffold {

void PublishFailureReason(Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold) {
  if (scaffold.modular_split_ready &&
      scaffold.expansion_ready &&
      scaffold.edge_case_compatibility_ready &&
      scaffold.edge_case_expansion_consistent &&
      scaffold.edge_case_robustness_ready &&
      !scaffold.edge_case_robustness_key.empty() &&
      scaffold.diagnostics_hardening_ready &&
      !scaffold.diagnostics_hardening_key.empty() &&
      scaffold.recovery_determinism_ready &&
      !scaffold.recovery_determinism_key.empty() &&
      scaffold.conformance_matrix_ready &&
      !scaffold.conformance_matrix_key.empty() &&
      scaffold.conformance_corpus_ready &&
      !scaffold.conformance_corpus_key.empty() &&
      scaffold.performance_quality_guardrails_ready &&
      !scaffold.performance_quality_guardrails_key.empty() &&
      scaffold.cross_lane_integration_ready &&
      !scaffold.cross_lane_integration_key.empty()) {
    return;
  }

  if (!scaffold.ownership_qualifier_contract_ready) {
    scaffold.failure_reason = "ownership qualifier lowering contract is not ready";
  } else if (!scaffold.retain_release_contract_ready) {
    scaffold.failure_reason = "retain/release lowering contract is not ready";
  } else if (!scaffold.autoreleasepool_scope_contract_ready) {
    scaffold.failure_reason = "autoreleasepool scope lowering contract is not ready";
  } else if (!scaffold.arc_diagnostics_fixit_contract_ready) {
    scaffold.failure_reason = "ARC diagnostics/fix-it lowering contract is not ready";
  } else if (!scaffold.replay_keys_ready) {
    scaffold.failure_reason = "ownership-aware lowering replay keys are incomplete";
  } else if (!scaffold.deterministic_replay_surface) {
    scaffold.failure_reason = "ownership-aware lowering replay keys are not lane-contract deterministic";
  } else if (!scaffold.weak_unowned_semantics_contract_ready) {
    scaffold.failure_reason = "weak/unowned semantics lowering contract is not ready";
  } else if (!scaffold.ownership_profile_accounting_consistent) {
    scaffold.failure_reason = "ownership-aware lowering expansion accounting is inconsistent";
  } else if (!scaffold.expansion_replay_keys_ready) {
    scaffold.failure_reason = "ownership-aware lowering expansion replay keys are incomplete";
  } else if (!scaffold.expansion_deterministic_replay_surface) {
    scaffold.failure_reason = "ownership-aware lowering expansion replay keys are not lane-contract deterministic";
  } else if (!scaffold.expansion_ready) {
    scaffold.failure_reason = "ownership-aware lowering core feature expansion is not ready";
  } else if (!scaffold.compatibility_handoff_consistent) {
    scaffold.failure_reason = "ownership-aware lowering compatibility handoff is inconsistent";
  } else if (!scaffold.language_version_pragma_coordinate_order_consistent) {
    scaffold.failure_reason = "ownership-aware lowering language version pragma coordinate order is inconsistent";
  } else if (!scaffold.parse_artifact_edge_case_robustness_consistent) {
    scaffold.failure_reason = "ownership-aware lowering parse artifact edge-case robustness is inconsistent";
  } else if (!scaffold.parse_artifact_replay_key_deterministic) {
    scaffold.failure_reason = "ownership-aware lowering parse artifact replay key is not deterministic";
  } else if (scaffold.compatibility_handoff_key.empty()) {
    scaffold.failure_reason = "ownership-aware lowering compatibility handoff key is empty";
  } else if (scaffold.parse_artifact_edge_robustness_key.empty()) {
    scaffold.failure_reason = "ownership-aware lowering parse artifact edge robustness key is empty";
  } else if (!scaffold.edge_case_compatibility_ready) {
    scaffold.failure_reason = "ownership-aware lowering edge-case compatibility is not ready";
  } else if (!scaffold.edge_case_expansion_consistent) {
    scaffold.failure_reason = "ownership-aware lowering edge-case expansion is inconsistent";
  } else if (!scaffold.edge_case_robustness_ready) {
    scaffold.failure_reason = "ownership-aware lowering edge-case robustness is not ready";
  } else if (scaffold.edge_case_robustness_key.empty()) {
    scaffold.failure_reason = "ownership-aware lowering edge-case robustness key is empty";
  } else if (!scaffold.diagnostics_hardening_consistent) {
    scaffold.failure_reason = "ownership-aware lowering diagnostics hardening is inconsistent";
  } else if (scaffold.diagnostics_hardening_key.empty()) {
    scaffold.failure_reason = "ownership-aware lowering diagnostics hardening key is empty";
  } else if (!scaffold.diagnostics_hardening_ready) {
    scaffold.failure_reason = "ownership-aware lowering diagnostics hardening is not ready";
  } else if (!scaffold.parse_recovery_determinism_hardening_consistent) {
    scaffold.failure_reason = "ownership-aware lowering parse recovery determinism hardening is inconsistent";
  } else if (scaffold.parse_recovery_determinism_hardening_key.empty()) {
    scaffold.failure_reason = "ownership-aware lowering parse recovery determinism hardening key is empty";
  } else if (!scaffold.recovery_determinism_consistent) {
    scaffold.failure_reason = "ownership-aware lowering recovery determinism hardening is inconsistent";
  } else if (scaffold.recovery_determinism_key.empty()) {
    scaffold.failure_reason = "ownership-aware lowering recovery determinism hardening key is empty";
  } else if (!scaffold.recovery_determinism_ready) {
    scaffold.failure_reason = "ownership-aware lowering recovery determinism hardening is not ready";
  } else if (!scaffold.parse_lowering_conformance_matrix_consistent) {
    scaffold.failure_reason = "ownership-aware lowering parse conformance matrix is inconsistent";
  } else if (scaffold.parse_lowering_conformance_matrix_key.empty()) {
    scaffold.failure_reason = "ownership-aware lowering parse conformance matrix key is empty";
  } else if (!scaffold.conformance_matrix_consistent) {
    scaffold.failure_reason = "ownership-aware lowering conformance matrix is inconsistent";
  } else if (scaffold.conformance_matrix_key.empty()) {
    scaffold.failure_reason = "ownership-aware lowering conformance matrix key is empty";
  } else if (!scaffold.conformance_matrix_ready) {
    scaffold.failure_reason = "ownership-aware lowering conformance matrix is not ready";
  } else if (!scaffold.parse_lowering_conformance_corpus_consistent) {
    scaffold.failure_reason = "ownership-aware lowering parse conformance corpus is inconsistent";
  } else if (scaffold.parse_lowering_conformance_corpus_case_count == 0) {
    scaffold.failure_reason = "ownership-aware lowering parse conformance corpus case count is zero";
  } else if (scaffold.parse_lowering_conformance_corpus_key.empty()) {
    scaffold.failure_reason = "ownership-aware lowering parse conformance corpus key is empty";
  } else if (!scaffold.conformance_corpus_consistent) {
    scaffold.failure_reason = "ownership-aware lowering conformance corpus is inconsistent";
  } else if (scaffold.conformance_corpus_key.empty()) {
    scaffold.failure_reason = "ownership-aware lowering conformance corpus key is empty";
  } else if (!scaffold.conformance_corpus_ready) {
    scaffold.failure_reason = "ownership-aware lowering conformance corpus is not ready";
  } else if (!scaffold.parse_lowering_performance_quality_guardrails_consistent) {
    scaffold.failure_reason = "ownership-aware lowering parse performance quality guardrails are inconsistent";
  } else if (scaffold.parse_lowering_performance_quality_guardrails_case_count == 0) {
    scaffold.failure_reason = "ownership-aware lowering parse performance quality guardrails case count is zero";
  } else if (scaffold.parse_lowering_performance_quality_guardrails_case_count !=
             scaffold.parse_lowering_performance_quality_guardrails_passed_case_count +
                 scaffold.parse_lowering_performance_quality_guardrails_failed_case_count) {
    scaffold.failure_reason = "ownership-aware lowering parse performance quality guardrails case accounting is inconsistent";
  } else if (scaffold.parse_lowering_performance_quality_guardrails_failed_case_count != 0) {
    scaffold.failure_reason = "ownership-aware lowering parse performance quality guardrails include failing cases";
  } else if (scaffold.parse_lowering_performance_quality_guardrails_key.empty()) {
    scaffold.failure_reason = "ownership-aware lowering parse performance quality guardrails key is empty";
  } else if (!scaffold.performance_quality_guardrails_consistent) {
    scaffold.failure_reason = "ownership-aware lowering performance quality guardrails are inconsistent";
  } else if (scaffold.performance_quality_guardrails_key.empty()) {
    scaffold.failure_reason = "ownership-aware lowering performance quality guardrails key is empty";
  } else if (!scaffold.performance_quality_guardrails_ready) {
    scaffold.failure_reason = "ownership-aware lowering performance quality guardrails are not ready";
  } else if (!scaffold.lowering_pass_graph_conformance_corpus_ready) {
    scaffold.failure_reason = "ownership-aware lowering lane-A pass-graph conformance corpus is not ready";
  } else if (scaffold.lowering_pass_graph_conformance_corpus_key.empty()) {
    scaffold.failure_reason = "ownership-aware lowering lane-A pass-graph conformance corpus key is empty";
  } else if (!scaffold.lowering_pass_graph_performance_quality_guardrails_ready) {
    scaffold.failure_reason =
        "ownership-aware lowering lane-A pass-graph performance quality guardrails are not ready";
  } else if (scaffold.lowering_pass_graph_performance_quality_guardrails_key.empty()) {
    scaffold.failure_reason =
        "ownership-aware lowering lane-A pass-graph performance quality guardrails key is empty";
  } else if (!scaffold.cross_lane_integration_consistent) {
    scaffold.failure_reason = "ownership-aware lowering cross-lane integration is inconsistent";
  } else if (!scaffold.cross_lane_integration_ready) {
    scaffold.failure_reason = "ownership-aware lowering cross-lane integration is not ready";
  } else if (scaffold.cross_lane_integration_key.empty()) {
    scaffold.failure_reason = "ownership-aware lowering cross-lane integration key is empty";
  } else {
    scaffold.failure_reason = "ownership-aware lowering modular split scaffold not ready";
  }
}

}  // namespace objc3_ownership_aware_lowering_behavior_scaffold
