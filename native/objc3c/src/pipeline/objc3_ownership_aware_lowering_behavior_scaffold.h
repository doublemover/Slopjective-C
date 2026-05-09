#pragma once

#include <cstddef>
#include <string>

#include "lower/objc3_lowering_contract.h"

struct Objc3OwnershipAwareLoweringBehaviorScaffold {
  // ARC mode-handling anchor: this scaffold remains the truthful
  // source-side inventory for ownership qualifiers, weak/unowned semantics,
  // autoreleasepool profiling, and ARC fix-it summaries while explicit
  // -fobjc-arc mode only widens the executable signature boundary rather than
  // claiming full ARC automation.
  // ARC semantic-rule freeze anchor: this scaffold remains explicit
  // about legality/inference boundaries so later ARC lifetime work cannot
  // silently treat property conflicts or inferred ownership as already live.
  // ARC inference/lifetime implementation anchor: this scaffold now
  // truthfully carries inferred strong-owned retain/release activity for the
  // supported ARC slice, without widening into the still-deferred cleanup,
  // weak, autorelease-return, or property-synthesis ARC behaviors.
  // ARC interaction-semantics expansion anchor: this scaffold now
  // also truthfully carries the supported weak/non-owning, autorelease-return,
  // synthesized-accessor, and block-interaction semantic packets that sit on
  // top of the retained ARC inference baseline.
  // ARC lowering ABI/cleanup freeze anchor: this scaffold remains a
  // semantic/source packet only and does not by itself schedule ARC cleanup
  // scopes or claim helper-call placement; later lane-C lowering must consume
  // it explicitly.
  // ARC automatic-insertion implementation anchor: this scaffold now
  // explicitly feeds the supported ARC helper-placement path, but remains a
  // source-side replay packet rather than the place where retain/release/
  // autorelease calls are emitted.
  // ARC cleanup/weak/lifetime implementation anchor: this scaffold
  // still only carries the source-side ownership and block-interaction packets
  // that lane-C lowering consumes for scope cleanup, weak current-property
  // helper paths, and deterministic lifetime cleanup on scope and implicit
  // exits.
  // ARC/block autorelease-return implementation anchor: this
  // scaffold still carries only source-side return-autorelease and block-escape
  // intent while lane-C owns the actual branch-stable cleanup ordering when
  // escaping blocks and autoreleasing returns compose.
  // runnable-arc-runtime gate anchor: lane-E consumes the
  // already-proven ARC source, semantic, lowering, and runtime summaries
  // without widening this scaffold beyond its truthful source-side packet role.
  // runnable-arc-closeout anchor: lane-E closes the current ARC
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

bool HasOwnershipLaneContractReplaySuffix(const std::string &replay_key,
                                          const char *lane_contract);

std::string BuildObjc3OwnershipAwareLoweringBehaviorScaffoldKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold);

std::string BuildObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold);

std::string BuildObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold);

std::string BuildObjc3OwnershipAwareLoweringBehaviorEdgeCaseRobustnessKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold);

std::string BuildObjc3OwnershipAwareLoweringBehaviorDiagnosticsHardeningKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold);

std::string BuildObjc3OwnershipAwareLoweringBehaviorRecoveryDeterminismKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold);

std::string BuildObjc3OwnershipAwareLoweringBehaviorConformanceMatrixKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold);

std::string BuildObjc3OwnershipAwareLoweringBehaviorConformanceCorpusKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold);

std::string BuildObjc3OwnershipAwareLoweringBehaviorPerformanceQualityGuardrailsKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold);

std::string BuildObjc3OwnershipAwareLoweringBehaviorCrossLaneIntegrationKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold);

Objc3OwnershipAwareLoweringBehaviorScaffold BuildObjc3OwnershipAwareLoweringBehaviorScaffold(
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
    const std::string &lowering_pass_graph_performance_quality_guardrails_key);

bool IsObjc3OwnershipAwareLoweringBehaviorScaffoldReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason);

bool IsObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason);

bool IsObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason);

bool IsObjc3OwnershipAwareLoweringBehaviorEdgeCaseRobustnessReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason);

bool IsObjc3OwnershipAwareLoweringBehaviorDiagnosticsHardeningReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason);

bool IsObjc3OwnershipAwareLoweringBehaviorRecoveryDeterminismReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason);

bool IsObjc3OwnershipAwareLoweringBehaviorConformanceMatrixReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason);

bool IsObjc3OwnershipAwareLoweringBehaviorConformanceCorpusReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason);

bool IsObjc3OwnershipAwareLoweringBehaviorPerformanceQualityGuardrailsReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason);

bool IsObjc3OwnershipAwareLoweringBehaviorCrossLaneIntegrationReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason);
