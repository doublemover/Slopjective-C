#pragma once

#include "pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h"

namespace objc3_ownership_aware_lowering_behavior_scaffold {

void PopulateEvidence(
    Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
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

void PublishReadiness(
    Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    const Objc3OwnershipQualifierLoweringContract &ownership_qualifier_contract,
    const Objc3RetainReleaseOperationLoweringContract &retain_release_contract,
    const Objc3WeakUnownedSemanticsLoweringContract &weak_unowned_semantics_contract,
    const Objc3ArcDiagnosticsFixitLoweringContract &arc_diagnostics_fixit_contract);

void PublishFailureReason(Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold);

}  // namespace objc3_ownership_aware_lowering_behavior_scaffold
