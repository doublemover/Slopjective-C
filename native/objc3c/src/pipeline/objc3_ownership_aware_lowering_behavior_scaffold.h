#pragma once

#include <cstddef>
#include <sstream>
#include <string>

#include "lower/objc3_lowering_contract.h"

struct Objc3OwnershipAwareLoweringBehaviorScaffold {
  // M262-A002 ARC mode-handling anchor: this scaffold remains the truthful
  // source-side inventory for ownership qualifiers, weak/unowned semantics,
  // autoreleasepool profiling, and ARC fix-it summaries while explicit
  // -fobjc-arc mode only widens the executable signature boundary rather than
  // claiming full ARC automation.
  // M262-B001 ARC semantic-rule freeze anchor: this scaffold remains explicit
  // about legality/inference boundaries so later ARC lifetime work cannot
  // silently treat property conflicts or inferred ownership as already live.
  // M262-B002 ARC inference/lifetime implementation anchor: this scaffold now
  // truthfully carries inferred strong-owned retain/release activity for the
  // supported ARC slice, without widening into the still-deferred cleanup,
  // weak, autorelease-return, or property-synthesis ARC behaviors.
  // M262-B003 ARC interaction-semantics expansion anchor: this scaffold now
  // also truthfully carries the supported weak/non-owning, autorelease-return,
  // synthesized-accessor, and block-interaction semantic packets that sit on
  // top of the retained ARC inference baseline.
  // M262-C001 ARC lowering ABI/cleanup freeze anchor: this scaffold remains a
  // semantic/source packet only and does not by itself schedule ARC cleanup
  // scopes or claim helper-call placement; later lane-C lowering must consume
  // it explicitly.
  // M262-C002 ARC automatic-insertion implementation anchor: this scaffold now
  // explicitly feeds the supported ARC helper-placement path, but remains a
  // source-side replay packet rather than the place where retain/release/
  // autorelease calls are emitted.
  // M262-C003 ARC cleanup/weak/lifetime implementation anchor: this scaffold
  // still only carries the source-side ownership and block-interaction packets
  // that lane-C lowering consumes for scope cleanup, weak current-property
  // helper paths, and deterministic lifetime cleanup on scope and implicit
  // exits.
  // M262-C004 ARC/block autorelease-return implementation anchor: this
  // scaffold still carries only source-side return-autorelease and block-escape
  // intent while lane-C owns the actual branch-stable cleanup ordering when
  // escaping blocks and autoreleasing returns compose.
  // M262-E001 runnable-arc-runtime gate anchor: lane-E consumes the
  // already-proven ARC source, semantic, lowering, and runtime summaries
  // without widening this scaffold beyond its truthful source-side packet role.
  // M262-E002 runnable-arc-closeout anchor: lane-E closes the current ARC
  // tranche by combining those preserved summaries with integrated execution
  // smoke and operator docs rather than widening lowering behavior here.
  bool ownership_qualifier_contract_ready = false;
  bool retain_release_contract_ready = false;
  bool autoreleasepool_scope_contract_ready = false;
  bool arc_diagnostics_fixit_contract_ready = false;
  bool weak_unowned_semantics_contract_ready = false;
  bool ownership_profile_accounting_consistent = false;
  bool replay_keys_ready = false;
  bool deterministic_replay_surface = false;
  bool modular_split_ready = false;
  bool expansion_replay_keys_ready = false;
  bool expansion_deterministic_replay_surface = false;
  bool expansion_ready = false;
  bool compatibility_handoff_consistent = false;
  bool language_version_pragma_coordinate_order_consistent = false;
  bool parse_artifact_edge_case_robustness_consistent = false;
  bool parse_artifact_replay_key_deterministic = false;
  bool edge_case_compatibility_ready = false;
  bool edge_case_expansion_consistent = false;
  bool edge_case_robustness_ready = false;
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  bool recovery_determinism_consistent = false;
  bool recovery_determinism_ready = false;
  bool conformance_matrix_consistent = false;
  bool conformance_matrix_ready = false;
  bool conformance_corpus_consistent = false;
  bool conformance_corpus_ready = false;
  bool performance_quality_guardrails_consistent = false;
  bool performance_quality_guardrails_ready = false;
  bool lowering_pass_graph_conformance_corpus_ready = false;
  bool lowering_pass_graph_performance_quality_guardrails_ready = false;
  bool cross_lane_integration_consistent = false;
  bool cross_lane_integration_ready = false;
  bool parse_recovery_determinism_hardening_consistent = false;
  bool parse_lowering_conformance_matrix_consistent = false;
  bool parse_lowering_conformance_corpus_consistent = false;
  bool parse_lowering_performance_quality_guardrails_consistent = false;
  std::size_t parse_lowering_conformance_corpus_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_passed_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_failed_case_count = 0;
  std::string ownership_qualifier_replay_key;
  std::string retain_release_replay_key;
  std::string autoreleasepool_scope_replay_key;
  std::string weak_unowned_semantics_replay_key;
  std::string arc_diagnostics_fixit_replay_key;
  std::string compatibility_handoff_key;
  std::string parse_artifact_edge_robustness_key;
  std::string scaffold_key;
  std::string expansion_key;
  std::string edge_case_compatibility_key;
  std::string edge_case_robustness_key;
  std::string diagnostics_hardening_key;
  std::string recovery_determinism_key;
  std::string conformance_matrix_key;
  std::string conformance_corpus_key;
  std::string performance_quality_guardrails_key;
  std::string lowering_pass_graph_conformance_corpus_key;
  std::string lowering_pass_graph_performance_quality_guardrails_key;
  std::string cross_lane_integration_key;
  std::string parse_recovery_determinism_hardening_key;
  std::string parse_lowering_conformance_matrix_key;
  std::string parse_lowering_conformance_corpus_key;
  std::string parse_lowering_performance_quality_guardrails_key;
  std::string failure_reason;
};

inline bool HasOwnershipLaneContractReplaySuffix(const std::string &replay_key, const char *lane_contract) {
  const std::string expected_suffix = std::string(";lane_contract=") + lane_contract;
  return replay_key.find(expected_suffix) != std::string::npos;
}

inline std::string BuildObjc3OwnershipAwareLoweringBehaviorScaffoldKey(
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

inline std::string BuildObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionKey(
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

inline std::string BuildObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityKey(
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

inline std::string BuildObjc3OwnershipAwareLoweringBehaviorEdgeCaseRobustnessKey(
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

inline std::string BuildObjc3OwnershipAwareLoweringBehaviorDiagnosticsHardeningKey(
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

inline std::string BuildObjc3OwnershipAwareLoweringBehaviorRecoveryDeterminismKey(
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

inline std::string BuildObjc3OwnershipAwareLoweringBehaviorConformanceMatrixKey(
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

inline std::string BuildObjc3OwnershipAwareLoweringBehaviorConformanceCorpusKey(
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

inline std::string BuildObjc3OwnershipAwareLoweringBehaviorPerformanceQualityGuardrailsKey(
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

inline std::string BuildObjc3OwnershipAwareLoweringBehaviorCrossLaneIntegrationKey(
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

inline Objc3OwnershipAwareLoweringBehaviorScaffold BuildObjc3OwnershipAwareLoweringBehaviorScaffold(
    const Objc3OwnershipQualifierLoweringContract &ownership_qualifier_contract,
    const std::string &ownership_qualifier_replay_key,
    const Objc3RetainReleaseOperationLoweringContract &retain_release_contract,
    const std::string &retain_release_replay_key,
    const Objc3AutoreleasePoolScopeLoweringContract &autoreleasepool_scope_contract,
    const std::string &autoreleasepool_scope_replay_key,
    const Objc3WeakUnownedSemanticsLoweringContract &weak_unowned_semantics_contract,
    const std::string &weak_unowned_semantics_replay_key,
    const Objc3ArcDiagnosticsFixitLoweringContract &arc_diagnostics_fixit_contract,
    const std::string &arc_diagnostics_fixit_replay_key,
    bool compatibility_handoff_consistent,
    bool language_version_pragma_coordinate_order_consistent,
    bool parse_artifact_edge_case_robustness_consistent,
    bool parse_artifact_replay_key_deterministic,
    const std::string &compatibility_handoff_key,
    const std::string &parse_artifact_edge_robustness_key,
    bool parse_recovery_determinism_hardening_consistent,
    const std::string &parse_recovery_determinism_hardening_key,
    bool parse_lowering_conformance_matrix_consistent,
    const std::string &parse_lowering_conformance_matrix_key,
    bool parse_lowering_conformance_corpus_consistent,
    std::size_t parse_lowering_conformance_corpus_case_count,
    const std::string &parse_lowering_conformance_corpus_key,
    bool parse_lowering_performance_quality_guardrails_consistent,
    std::size_t parse_lowering_performance_quality_guardrails_case_count,
    std::size_t parse_lowering_performance_quality_guardrails_passed_case_count,
    std::size_t parse_lowering_performance_quality_guardrails_failed_case_count,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    bool lowering_pass_graph_conformance_corpus_ready,
    const std::string &lowering_pass_graph_conformance_corpus_key,
    bool lowering_pass_graph_performance_quality_guardrails_ready,
    const std::string &lowering_pass_graph_performance_quality_guardrails_key) {
  Objc3OwnershipAwareLoweringBehaviorScaffold scaffold;
  scaffold.ownership_qualifier_contract_ready =
      IsValidObjc3OwnershipQualifierLoweringContract(ownership_qualifier_contract) &&
      ownership_qualifier_contract.deterministic;
  scaffold.retain_release_contract_ready =
      IsValidObjc3RetainReleaseOperationLoweringContract(retain_release_contract) &&
      retain_release_contract.deterministic;
  scaffold.autoreleasepool_scope_contract_ready =
      IsValidObjc3AutoreleasePoolScopeLoweringContract(autoreleasepool_scope_contract) &&
      autoreleasepool_scope_contract.deterministic;
  scaffold.weak_unowned_semantics_contract_ready =
      IsValidObjc3WeakUnownedSemanticsLoweringContract(weak_unowned_semantics_contract) &&
      weak_unowned_semantics_contract.deterministic;
  scaffold.arc_diagnostics_fixit_contract_ready =
      IsValidObjc3ArcDiagnosticsFixitLoweringContract(arc_diagnostics_fixit_contract) &&
      arc_diagnostics_fixit_contract.deterministic;
  scaffold.ownership_qualifier_replay_key = ownership_qualifier_replay_key;
  scaffold.retain_release_replay_key = retain_release_replay_key;
  scaffold.autoreleasepool_scope_replay_key = autoreleasepool_scope_replay_key;
  scaffold.weak_unowned_semantics_replay_key = weak_unowned_semantics_replay_key;
  scaffold.arc_diagnostics_fixit_replay_key = arc_diagnostics_fixit_replay_key;
  scaffold.compatibility_handoff_consistent = compatibility_handoff_consistent;
  scaffold.language_version_pragma_coordinate_order_consistent =
      language_version_pragma_coordinate_order_consistent;
  scaffold.parse_artifact_edge_case_robustness_consistent =
      parse_artifact_edge_case_robustness_consistent;
  scaffold.parse_artifact_replay_key_deterministic =
      parse_artifact_replay_key_deterministic;
  scaffold.parse_recovery_determinism_hardening_consistent =
      parse_recovery_determinism_hardening_consistent;
  scaffold.parse_lowering_conformance_matrix_consistent =
      parse_lowering_conformance_matrix_consistent;
  scaffold.parse_lowering_conformance_corpus_consistent =
      parse_lowering_conformance_corpus_consistent;
  scaffold.parse_lowering_conformance_corpus_case_count =
      parse_lowering_conformance_corpus_case_count;
  scaffold.parse_lowering_performance_quality_guardrails_consistent =
      parse_lowering_performance_quality_guardrails_consistent;
  scaffold.parse_lowering_performance_quality_guardrails_case_count =
      parse_lowering_performance_quality_guardrails_case_count;
  scaffold.parse_lowering_performance_quality_guardrails_passed_case_count =
      parse_lowering_performance_quality_guardrails_passed_case_count;
  scaffold.parse_lowering_performance_quality_guardrails_failed_case_count =
      parse_lowering_performance_quality_guardrails_failed_case_count;
  scaffold.compatibility_handoff_key = compatibility_handoff_key;
  scaffold.parse_artifact_edge_robustness_key =
      parse_artifact_edge_robustness_key;
  scaffold.parse_recovery_determinism_hardening_key =
      parse_recovery_determinism_hardening_key;
  scaffold.parse_lowering_conformance_matrix_key =
      parse_lowering_conformance_matrix_key;
  scaffold.parse_lowering_conformance_corpus_key =
      parse_lowering_conformance_corpus_key;
  scaffold.parse_lowering_performance_quality_guardrails_key =
      parse_lowering_performance_quality_guardrails_key;
  scaffold.lowering_pass_graph_conformance_corpus_ready =
      lowering_pass_graph_conformance_corpus_ready;
  scaffold.lowering_pass_graph_conformance_corpus_key =
      lowering_pass_graph_conformance_corpus_key;
  scaffold.lowering_pass_graph_performance_quality_guardrails_ready =
      lowering_pass_graph_performance_quality_guardrails_ready;
  scaffold.lowering_pass_graph_performance_quality_guardrails_key =
      lowering_pass_graph_performance_quality_guardrails_key;
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
    return scaffold;
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

  return scaffold;
}

inline bool IsObjc3OwnershipAwareLoweringBehaviorScaffoldReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.modular_split_ready) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering modular split scaffold not ready"
               : scaffold.failure_reason;
  return false;
}

inline bool IsObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.expansion_ready) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering core feature expansion is not ready"
               : scaffold.failure_reason;
  return false;
}

inline bool IsObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.edge_case_compatibility_ready &&
      scaffold.edge_case_robustness_ready &&
      !scaffold.edge_case_robustness_key.empty() &&
      scaffold.diagnostics_hardening_ready &&
      !scaffold.diagnostics_hardening_key.empty()) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering edge-case compatibility is not ready"
               : scaffold.failure_reason;
  return false;
}

inline bool IsObjc3OwnershipAwareLoweringBehaviorEdgeCaseRobustnessReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.edge_case_expansion_consistent &&
      scaffold.edge_case_robustness_ready &&
      !scaffold.edge_case_robustness_key.empty()) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering edge-case robustness is not ready"
               : scaffold.failure_reason;
  return false;
}

inline bool IsObjc3OwnershipAwareLoweringBehaviorDiagnosticsHardeningReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.diagnostics_hardening_consistent &&
      scaffold.diagnostics_hardening_ready &&
      !scaffold.diagnostics_hardening_key.empty()) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering diagnostics hardening is not ready"
               : scaffold.failure_reason;
  return false;
}

inline bool IsObjc3OwnershipAwareLoweringBehaviorRecoveryDeterminismReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.recovery_determinism_consistent &&
      scaffold.recovery_determinism_ready &&
      !scaffold.recovery_determinism_key.empty()) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering recovery determinism hardening is not ready"
               : scaffold.failure_reason;
  return false;
}

inline bool IsObjc3OwnershipAwareLoweringBehaviorConformanceMatrixReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.conformance_matrix_consistent &&
      scaffold.conformance_matrix_ready &&
      !scaffold.conformance_matrix_key.empty()) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering conformance matrix is not ready"
               : scaffold.failure_reason;
  return false;
}

inline bool IsObjc3OwnershipAwareLoweringBehaviorConformanceCorpusReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.conformance_corpus_consistent &&
      scaffold.conformance_corpus_ready &&
      !scaffold.conformance_corpus_key.empty()) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering conformance corpus is not ready"
               : scaffold.failure_reason;
  return false;
}

inline bool IsObjc3OwnershipAwareLoweringBehaviorPerformanceQualityGuardrailsReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.performance_quality_guardrails_consistent &&
      scaffold.performance_quality_guardrails_ready &&
      !scaffold.performance_quality_guardrails_key.empty()) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering performance quality guardrails are not ready"
               : scaffold.failure_reason;
  return false;
}

inline bool IsObjc3OwnershipAwareLoweringBehaviorCrossLaneIntegrationReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.cross_lane_integration_consistent &&
      scaffold.cross_lane_integration_ready &&
      !scaffold.cross_lane_integration_key.empty()) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering cross-lane integration is not ready"
               : scaffold.failure_reason;
  return false;
}
