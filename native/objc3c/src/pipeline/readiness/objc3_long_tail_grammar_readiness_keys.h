#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

inline std::string BuildObjc3LongTailGrammarExpansionKey(
    std::size_t construct_count,
    std::size_t covered_construct_count,
    std::uint64_t fingerprint,
    bool core_feature_consistent,
    bool handoff_key_deterministic,
    bool expansion_accounting_consistent,
    bool replay_keys_ready,
    bool expansion_ready) {
  return "construct_count=" + std::to_string(construct_count) +
         ";covered_construct_count=" + std::to_string(covered_construct_count) +
         ";fingerprint=" + std::to_string(fingerprint) +
         ";core_feature_consistent=" + (core_feature_consistent ? "true" : "false") +
         ";handoff_key_deterministic=" + (handoff_key_deterministic ? "true" : "false") +
         ";expansion_accounting_consistent=" +
         (expansion_accounting_consistent ? "true" : "false") +
         ";replay_keys_ready=" + (replay_keys_ready ? "true" : "false") +
         ";expansion_ready=" + (expansion_ready ? "true" : "false");
}

inline std::string BuildObjc3LongTailGrammarEdgeCaseCompatibilityKey(
    bool compatibility_handoff_consistent,
    bool compatibility_handoff_ready,
    bool language_version_pragma_coordinate_order_consistent,
    bool parse_artifact_edge_case_robustness_consistent,
    bool edge_case_compatibility_consistent,
    bool edge_case_compatibility_ready) {
  return "compatibility_handoff_consistent=" +
         std::string(compatibility_handoff_consistent ? "true" : "false") +
         ";compatibility_handoff_ready=" +
         std::string(compatibility_handoff_ready ? "true" : "false") +
         ";pragma_coordinate_order_consistent=" +
         std::string(language_version_pragma_coordinate_order_consistent ? "true" : "false") +
         ";parse_edge_case_robustness_consistent=" +
         std::string(parse_artifact_edge_case_robustness_consistent ? "true" : "false") +
         ";edge_case_compatibility_consistent=" +
         std::string(edge_case_compatibility_consistent ? "true" : "false") +
         ";edge_case_compatibility_ready=" +
         std::string(edge_case_compatibility_ready ? "true" : "false");
}

inline std::string BuildObjc3LongTailGrammarEdgeCaseRobustnessKey(
    std::size_t construct_count,
    std::size_t covered_construct_count,
    std::uint64_t fingerprint,
    bool edge_case_compatibility_ready,
    bool edge_case_expansion_consistent,
    bool parse_artifact_edge_case_robustness_consistent,
    bool edge_case_robustness_ready) {
  return "construct_count=" + std::to_string(construct_count) +
         ";covered_construct_count=" + std::to_string(covered_construct_count) +
         ";fingerprint=" + std::to_string(fingerprint) +
         ";edge_case_compatibility_ready=" +
         std::string(edge_case_compatibility_ready ? "true" : "false") +
         ";edge_case_expansion_consistent=" +
         std::string(edge_case_expansion_consistent ? "true" : "false") +
         ";parse_edge_case_robustness_consistent=" +
         std::string(parse_artifact_edge_case_robustness_consistent ? "true" : "false") +
         ";edge_case_robustness_ready=" +
         std::string(edge_case_robustness_ready ? "true" : "false");
}

inline std::string BuildObjc3LongTailGrammarDiagnosticsHardeningKey(
    std::size_t parser_diagnostic_count,
    std::size_t parser_diagnostic_code_count,
    std::uint64_t parser_diagnostic_code_fingerprint,
    bool parser_diagnostic_surface_consistent,
    bool parser_diagnostic_code_surface_deterministic,
    bool parse_artifact_diagnostics_hardening_consistent,
    bool edge_case_robustness_ready,
    bool diagnostics_hardening_consistent,
    bool diagnostics_hardening_ready) {
  return "parser_diagnostic_count=" + std::to_string(parser_diagnostic_count) +
         ";parser_diagnostic_code_count=" + std::to_string(parser_diagnostic_code_count) +
         ";parser_diagnostic_code_fingerprint=" + std::to_string(parser_diagnostic_code_fingerprint) +
         ";parser_diagnostic_surface_consistent=" +
         std::string(parser_diagnostic_surface_consistent ? "true" : "false") +
         ";parser_diagnostic_code_surface_deterministic=" +
         std::string(parser_diagnostic_code_surface_deterministic ? "true" : "false") +
         ";parse_artifact_diagnostics_hardening_consistent=" +
         std::string(parse_artifact_diagnostics_hardening_consistent ? "true" : "false") +
         ";edge_case_robustness_ready=" +
         std::string(edge_case_robustness_ready ? "true" : "false") +
         ";diagnostics_hardening_consistent=" +
         std::string(diagnostics_hardening_consistent ? "true" : "false") +
         ";diagnostics_hardening_ready=" +
         std::string(diagnostics_hardening_ready ? "true" : "false");
}

inline std::string BuildObjc3LongTailGrammarRecoveryDeterminismKey(
    bool parser_recovery_replay_ready,
    bool parse_artifact_replay_key_deterministic,
    bool parse_recovery_determinism_hardening_consistent,
    bool diagnostics_hardening_ready,
    bool edge_case_robustness_ready,
    bool expansion_ready,
    bool recovery_determinism_consistent,
    bool recovery_determinism_ready) {
  return std::string("parser_recovery_replay_ready=") +
         (parser_recovery_replay_ready ? "true" : "false") +
         ";parse_artifact_replay_key_deterministic=" +
         (parse_artifact_replay_key_deterministic ? "true" : "false") +
         ";parse_recovery_determinism_hardening_consistent=" +
         (parse_recovery_determinism_hardening_consistent ? "true" : "false") +
         ";diagnostics_hardening_ready=" +
         (diagnostics_hardening_ready ? "true" : "false") +
         ";edge_case_robustness_ready=" +
         (edge_case_robustness_ready ? "true" : "false") +
         ";expansion_ready=" + (expansion_ready ? "true" : "false") +
         ";recovery_determinism_consistent=" +
         (recovery_determinism_consistent ? "true" : "false") +
         ";recovery_determinism_ready=" +
         (recovery_determinism_ready ? "true" : "false");
}

inline std::string BuildObjc3LongTailGrammarConformanceMatrixKey(
    std::size_t conformance_matrix_case_count,
    std::size_t conformance_corpus_case_count,
    std::size_t performance_guardrail_case_count,
    bool parse_replay_key_deterministic,
    bool recovery_determinism_ready,
    bool conformance_matrix_consistent,
    bool conformance_matrix_ready) {
  return "conformance_matrix_case_count=" + std::to_string(conformance_matrix_case_count) +
         ";conformance_corpus_case_count=" + std::to_string(conformance_corpus_case_count) +
         ";performance_guardrail_case_count=" + std::to_string(performance_guardrail_case_count) +
         ";parse_replay_key_deterministic=" +
         std::string(parse_replay_key_deterministic ? "true" : "false") +
         ";recovery_determinism_ready=" +
         std::string(recovery_determinism_ready ? "true" : "false") +
         ";conformance_matrix_consistent=" +
         std::string(conformance_matrix_consistent ? "true" : "false") +
         ";conformance_matrix_ready=" +
         std::string(conformance_matrix_ready ? "true" : "false");
}

inline std::string BuildObjc3LongTailGrammarIntegrationCloseoutKey(
    bool conformance_matrix_ready,
    bool conformance_corpus_consistent,
    bool performance_guardrails_consistent,
    bool cross_lane_integration_consistent,
    bool cross_lane_integration_ready,
    bool recovery_determinism_ready,
    bool semantic_handoff_ready,
    bool lowering_boundary_ready,
    bool integration_closeout_consistent,
    bool gate_signoff_ready) {
  return std::string("conformance_matrix_ready=") +
         (conformance_matrix_ready ? "true" : "false") +
         ";conformance_corpus_consistent=" +
         (conformance_corpus_consistent ? "true" : "false") +
         ";performance_guardrails_consistent=" +
         (performance_guardrails_consistent ? "true" : "false") +
         ";cross_lane_integration_consistent=" +
         (cross_lane_integration_consistent ? "true" : "false") +
         ";cross_lane_integration_ready=" +
         (cross_lane_integration_ready ? "true" : "false") +
         ";recovery_determinism_ready=" +
         (recovery_determinism_ready ? "true" : "false") +
         ";semantic_handoff_ready=" +
         (semantic_handoff_ready ? "true" : "false") +
         ";lowering_boundary_ready=" +
         (lowering_boundary_ready ? "true" : "false") +
         ";integration_closeout_consistent=" +
         (integration_closeout_consistent ? "true" : "false") +
         ";gate_signoff_ready=" + (gate_signoff_ready ? "true" : "false");
}
