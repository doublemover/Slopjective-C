#include "pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h"

#include <sstream>

bool HasOwnershipLaneContractReplaySuffix(const std::string &replay_key,
                                          const char *lane_contract) {
  const std::string expected_suffix = std::string(";lane_contract=") + lane_contract;
  return replay_key.find(expected_suffix) != std::string::npos;
}

std::string BuildObjc3OwnershipAwareLoweringBehaviorScaffoldKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold) {
  std::ostringstream key;
  key << "ownership-aware-lowering-modular-split-scaffold:v1:"
      << "ownership_qualifier_contract_ready="
      << (scaffold.ownership_qualifier_contract_ready ? "true" : "false")
      << ";retain_release_contract_ready="
      << (scaffold.retain_release_contract_ready ? "true" : "false")
      << ";autoreleasepool_scope_contract_ready="
      << (scaffold.autoreleasepool_scope_contract_ready ? "true" : "false")
      << ";arc_diagnostics_fixit_contract_ready="
      << (scaffold.arc_diagnostics_fixit_contract_ready ? "true" : "false")
      << ";replay_keys_ready=" << (scaffold.replay_keys_ready ? "true" : "false")
      << ";deterministic_replay_surface="
      << (scaffold.deterministic_replay_surface ? "true" : "false")
      << ";modular_split_ready=" << (scaffold.modular_split_ready ? "true" : "false");
  return key.str();
}

std::string BuildObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold) {
  std::ostringstream key;
  key << "ownership-aware-lowering-core-feature-expansion:v1:"
      << "modular_split_ready=" << (scaffold.modular_split_ready ? "true" : "false")
      << ";weak_unowned_semantics_contract_ready="
      << (scaffold.weak_unowned_semantics_contract_ready ? "true" : "false")
      << ";ownership_profile_accounting_consistent="
      << (scaffold.ownership_profile_accounting_consistent ? "true" : "false")
      << ";expansion_replay_keys_ready="
      << (scaffold.expansion_replay_keys_ready ? "true" : "false")
      << ";expansion_deterministic_replay_surface="
      << (scaffold.expansion_deterministic_replay_surface ? "true" : "false")
      << ";expansion_ready=" << (scaffold.expansion_ready ? "true" : "false")
      << ";scaffold_key=" << scaffold.scaffold_key
      << ";weak_unowned_semantics_replay_key="
      << scaffold.weak_unowned_semantics_replay_key;
  return key.str();
}

std::string BuildObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold) {
  std::ostringstream key;
  key << "ownership-aware-lowering-edge-case-compatibility:v1:"
      << "expansion-ready=" << (scaffold.expansion_ready ? "true" : "false")
      << ";compatibility-handoff-consistent="
      << (scaffold.compatibility_handoff_consistent ? "true" : "false")
      << ";language-version-pragma-coordinate-order-consistent="
      << (scaffold.language_version_pragma_coordinate_order_consistent ? "true"
                                                                       : "false")
      << ";parse-artifact-edge-case-robustness-consistent="
      << (scaffold.parse_artifact_edge_case_robustness_consistent ? "true"
                                                                  : "false")
      << ";parse-artifact-replay-key-deterministic="
      << (scaffold.parse_artifact_replay_key_deterministic ? "true" : "false")
      << ";edge-case-compatibility-ready="
      << (scaffold.edge_case_compatibility_ready ? "true" : "false")
      << ";compatibility-handoff-key=" << scaffold.compatibility_handoff_key
      << ";parse-artifact-edge-robustness-key="
      << scaffold.parse_artifact_edge_robustness_key
      << ";weak-unowned-semantics-replay-key="
      << scaffold.weak_unowned_semantics_replay_key;
  return key.str();
}

std::string BuildObjc3OwnershipAwareLoweringBehaviorEdgeCaseRobustnessKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold) {
  std::ostringstream key;
  key << "ownership-aware-lowering-edge-case-expansion-robustness:v1:"
      << "edge-case-compatibility-ready="
      << (scaffold.edge_case_compatibility_ready ? "true" : "false")
      << ";edge-case-expansion-consistent="
      << (scaffold.edge_case_expansion_consistent ? "true" : "false")
      << ";edge-case-robustness-ready="
      << (scaffold.edge_case_robustness_ready ? "true" : "false")
      << ";parse-artifact-edge-case-robustness-consistent="
      << (scaffold.parse_artifact_edge_case_robustness_consistent ? "true"
                                                                  : "false")
      << ";parse-artifact-replay-key-deterministic="
      << (scaffold.parse_artifact_replay_key_deterministic ? "true" : "false")
      << ";edge-case-compatibility-key-ready="
      << (!scaffold.edge_case_compatibility_key.empty() ? "true" : "false")
      << ";parse-artifact-edge-robustness-key="
      << scaffold.parse_artifact_edge_robustness_key
      << ";compatibility-handoff-key=" << scaffold.compatibility_handoff_key
      << ";expansion-key=" << scaffold.expansion_key;
  return key.str();
}

std::string BuildObjc3OwnershipAwareLoweringBehaviorDiagnosticsHardeningKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold) {
  std::ostringstream key;
  key << "ownership-aware-lowering-diagnostics-hardening:v1:"
      << "edge-case-robustness-ready="
      << (scaffold.edge_case_robustness_ready ? "true" : "false")
      << ";parse-artifact-edge-case-robustness-consistent="
      << (scaffold.parse_artifact_edge_case_robustness_consistent ? "true"
                                                                  : "false")
      << ";parse-artifact-replay-key-deterministic="
      << (scaffold.parse_artifact_replay_key_deterministic ? "true" : "false")
      << ";arc-diagnostics-fixit-contract-ready="
      << (scaffold.arc_diagnostics_fixit_contract_ready ? "true" : "false")
      << ";ownership-profile-accounting-consistent="
      << (scaffold.ownership_profile_accounting_consistent ? "true" : "false")
      << ";arc-diagnostics-fixit-replay-key="
      << scaffold.arc_diagnostics_fixit_replay_key
      << ";edge-case-robustness-key=" << scaffold.edge_case_robustness_key
      << ";parse-artifact-edge-robustness-key="
      << scaffold.parse_artifact_edge_robustness_key;
  return key.str();
}

std::string BuildObjc3OwnershipAwareLoweringBehaviorRecoveryDeterminismKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold) {
  std::ostringstream key;
  key << "ownership-aware-lowering-recovery-determinism-hardening:v1:"
      << "diagnostics-hardening-ready="
      << (scaffold.diagnostics_hardening_ready ? "true" : "false")
      << ";parse-recovery-determinism-hardening-consistent="
      << (scaffold.parse_recovery_determinism_hardening_consistent ? "true"
                                                                   : "false")
      << ";parse-artifact-replay-key-deterministic="
      << (scaffold.parse_artifact_replay_key_deterministic ? "true" : "false")
      << ";diagnostics-hardening-key=" << scaffold.diagnostics_hardening_key
      << ";parse-recovery-determinism-hardening-key="
      << scaffold.parse_recovery_determinism_hardening_key
      << ";edge-case-robustness-key=" << scaffold.edge_case_robustness_key;
  return key.str();
}

std::string BuildObjc3OwnershipAwareLoweringBehaviorConformanceMatrixKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold) {
  std::ostringstream key;
  key << "ownership-aware-lowering-conformance-matrix:v1:"
      << "recovery-determinism-ready="
      << (scaffold.recovery_determinism_ready ? "true" : "false")
      << ";parse-lowering-conformance-matrix-consistent="
      << (scaffold.parse_lowering_conformance_matrix_consistent ? "true"
                                                                : "false")
      << ";parse-artifact-replay-key-deterministic="
      << (scaffold.parse_artifact_replay_key_deterministic ? "true" : "false")
      << ";recovery-determinism-key=" << scaffold.recovery_determinism_key
      << ";parse-lowering-conformance-matrix-key="
      << scaffold.parse_lowering_conformance_matrix_key;
  return key.str();
}

std::string BuildObjc3OwnershipAwareLoweringBehaviorConformanceCorpusKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold) {
  std::ostringstream key;
  key << "ownership-aware-lowering-conformance-corpus-expansion:v1:"
      << "conformance-matrix-ready="
      << (scaffold.conformance_matrix_ready ? "true" : "false")
      << ";parse-lowering-conformance-corpus-consistent="
      << (scaffold.parse_lowering_conformance_corpus_consistent ? "true" : "false")
      << ";parse-lowering-conformance-corpus-case-count="
      << scaffold.parse_lowering_conformance_corpus_case_count
      << ";parse-artifact-replay-key-deterministic="
      << (scaffold.parse_artifact_replay_key_deterministic ? "true" : "false")
      << ";conformance-matrix-key=" << scaffold.conformance_matrix_key
      << ";parse-lowering-conformance-corpus-key="
      << scaffold.parse_lowering_conformance_corpus_key;
  return key.str();
}

std::string BuildObjc3OwnershipAwareLoweringBehaviorPerformanceQualityGuardrailsKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold) {
  std::ostringstream key;
  key << "ownership-aware-lowering-performance-quality-guardrails:v1:"
      << "conformance-corpus-ready="
      << (scaffold.conformance_corpus_ready ? "true" : "false")
      << ";parse-lowering-performance-quality-guardrails-consistent="
      << (scaffold.parse_lowering_performance_quality_guardrails_consistent ? "true"
                                                                            : "false")
      << ";parse-lowering-performance-quality-guardrails-case-count="
      << scaffold.parse_lowering_performance_quality_guardrails_case_count
      << ";parse-lowering-performance-quality-guardrails-passed-case-count="
      << scaffold.parse_lowering_performance_quality_guardrails_passed_case_count
      << ";parse-lowering-performance-quality-guardrails-failed-case-count="
      << scaffold.parse_lowering_performance_quality_guardrails_failed_case_count
      << ";parse-artifact-replay-key-deterministic="
      << (scaffold.parse_artifact_replay_key_deterministic ? "true" : "false")
      << ";conformance-corpus-key=" << scaffold.conformance_corpus_key
      << ";parse-lowering-performance-quality-guardrails-key="
      << scaffold.parse_lowering_performance_quality_guardrails_key;
  return key.str();
}

std::string BuildObjc3OwnershipAwareLoweringBehaviorCrossLaneIntegrationKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold) {
  std::ostringstream key;
  key << "ownership-aware-lowering-cross-lane-integration-sync:v1:"
      << "performance-quality-guardrails-ready="
      << (scaffold.performance_quality_guardrails_ready ? "true" : "false")
      << ";lowering-pass-graph-performance-quality-guardrails-ready="
      << (scaffold.lowering_pass_graph_performance_quality_guardrails_ready ? "true"
                                                                            : "false")
      << ";conformance-corpus-ready="
      << (scaffold.conformance_corpus_ready ? "true" : "false")
      << ";lowering-pass-graph-conformance-corpus-ready="
      << (scaffold.lowering_pass_graph_conformance_corpus_ready ? "true" : "false")
      << ";cross-lane-integration-consistent="
      << (scaffold.cross_lane_integration_consistent ? "true" : "false")
      << ";cross-lane-integration-ready="
      << (scaffold.cross_lane_integration_ready ? "true" : "false")
      << ";performance-quality-guardrails-key="
      << scaffold.performance_quality_guardrails_key
      << ";lowering-pass-graph-performance-quality-guardrails-key="
      << scaffold.lowering_pass_graph_performance_quality_guardrails_key
      << ";conformance-corpus-key=" << scaffold.conformance_corpus_key
      << ";lowering-pass-graph-conformance-corpus-key="
      << scaffold.lowering_pass_graph_conformance_corpus_key;
  return key.str();
}
