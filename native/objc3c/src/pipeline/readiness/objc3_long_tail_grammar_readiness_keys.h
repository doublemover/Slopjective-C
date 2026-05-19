#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

std::string BuildObjc3LongTailGrammarExpansionKey(
    std::size_t construct_count,
    std::size_t covered_construct_count,
    std::uint64_t fingerprint,
    bool core_feature_consistent,
    bool handoff_key_deterministic,
    bool expansion_accounting_consistent,
    bool replay_keys_ready,
    bool expansion_ready);

std::string BuildObjc3LongTailGrammarEdgeCaseCompatibilityKey(
    bool compatibility_handoff_consistent,
    bool compatibility_handoff_ready,
    bool language_version_pragma_coordinate_order_consistent,
    bool parse_artifact_edge_case_robustness_consistent,
    bool edge_case_compatibility_consistent,
    bool edge_case_compatibility_ready);

std::string BuildObjc3LongTailGrammarEdgeCaseRobustnessKey(
    std::size_t construct_count,
    std::size_t covered_construct_count,
    std::uint64_t fingerprint,
    bool edge_case_compatibility_ready,
    bool edge_case_expansion_consistent,
    bool parse_artifact_edge_case_robustness_consistent,
    bool edge_case_robustness_ready);

std::string BuildObjc3LongTailGrammarDiagnosticsHardeningKey(
    std::size_t parser_diagnostic_count,
    std::size_t parser_diagnostic_code_count,
    std::uint64_t parser_diagnostic_code_fingerprint,
    bool parser_diagnostic_surface_consistent,
    bool parser_diagnostic_code_surface_deterministic,
    bool parse_artifact_diagnostics_hardening_consistent,
    bool edge_case_robustness_ready,
    bool diagnostics_hardening_consistent,
    bool diagnostics_hardening_ready);

std::string BuildObjc3LongTailGrammarRecoveryDeterminismKey(
    bool parser_recovery_replay_ready,
    bool parse_artifact_replay_key_deterministic,
    bool parse_recovery_determinism_hardening_consistent,
    bool diagnostics_hardening_ready,
    bool edge_case_robustness_ready,
    bool expansion_ready,
    bool recovery_determinism_consistent,
    bool recovery_determinism_ready);

std::string BuildObjc3LongTailGrammarConformanceMatrixKey(
    std::size_t conformance_matrix_case_count,
    std::size_t conformance_corpus_case_count,
    std::size_t performance_guardrail_case_count,
    bool parse_replay_key_deterministic,
    bool recovery_determinism_ready,
    bool conformance_matrix_consistent,
    bool conformance_matrix_ready);

std::string BuildObjc3LongTailGrammarIntegrationCloseoutKey(
    bool conformance_matrix_ready,
    bool conformance_corpus_consistent,
    bool performance_guardrails_consistent,
    bool cross_lane_integration_consistent,
    bool cross_lane_integration_ready,
    bool recovery_determinism_ready,
    bool semantic_handoff_ready,
    bool lowering_boundary_ready,
    bool integration_closeout_consistent,
    bool gate_signoff_ready);
