#pragma once

#include <cstddef>
#include <string>

inline constexpr std::size_t kObjc3ParseLoweringConformanceMatrixCaseCount = 8u;
inline constexpr std::size_t kObjc3ParseLoweringConformanceCorpusCaseCount = 8u;
inline constexpr std::size_t kObjc3ParseLoweringPerformanceQualityGuardrailsCaseCount = 6u;

inline std::string BuildObjc3ParseLoweringConformanceMatrixKey(
    std::size_t case_count,
    bool parser_contract_snapshot_present,
    bool parse_artifact_handoff_deterministic,
    bool parse_artifact_replay_key_deterministic,
    bool parse_artifact_diagnostics_hardening_consistent,
    bool parse_artifact_edge_case_robustness_consistent,
    bool parse_recovery_determinism_hardening_consistent,
    bool semantic_integration_surface_built,
    bool semantic_handoff_deterministic,
    bool lowering_boundary_ready,
    bool parse_lowering_conformance_matrix_consistent) {
  return "case_count=" + std::to_string(case_count) +
         ";parser_contract_snapshot_present=" + (parser_contract_snapshot_present ? "true" : "false") +
         ";parse_artifact_handoff_deterministic=" + (parse_artifact_handoff_deterministic ? "true" : "false") +
         ";parse_artifact_replay_key_deterministic=" + (parse_artifact_replay_key_deterministic ? "true" : "false") +
         ";parse_artifact_diagnostics_hardening_consistent=" +
         (parse_artifact_diagnostics_hardening_consistent ? "true" : "false") +
         ";parse_artifact_edge_case_robustness_consistent=" +
         (parse_artifact_edge_case_robustness_consistent ? "true" : "false") +
         ";parse_recovery_determinism_hardening_consistent=" +
         (parse_recovery_determinism_hardening_consistent ? "true" : "false") +
         ";semantic_integration_surface_built=" + (semantic_integration_surface_built ? "true" : "false") +
         ";semantic_handoff_deterministic=" + (semantic_handoff_deterministic ? "true" : "false") +
         ";lowering_boundary_ready=" + (lowering_boundary_ready ? "true" : "false") +
         ";consistent=" + (parse_lowering_conformance_matrix_consistent ? "true" : "false");
}

inline std::string BuildObjc3ParseLoweringConformanceCorpusKey(
    std::size_t case_count,
    std::size_t passed_case_count,
    std::size_t failed_case_count,
    bool parser_contract_snapshot_case_passed,
    bool parse_artifact_handoff_case_passed,
    bool parse_artifact_replay_case_passed,
    bool parse_artifact_diagnostics_hardening_case_passed,
    bool parse_artifact_edge_robustness_case_passed,
    bool parse_recovery_determinism_hardening_case_passed,
    bool semantic_handoff_case_passed,
    bool lowering_boundary_case_passed,
    bool parse_lowering_conformance_matrix_consistent,
    bool parse_lowering_conformance_corpus_consistent) {
  return "case_count=" + std::to_string(case_count) +
         ";passed_case_count=" + std::to_string(passed_case_count) +
         ";failed_case_count=" + std::to_string(failed_case_count) +
         ";parser_contract_snapshot_case_passed=" + (parser_contract_snapshot_case_passed ? "true" : "false") +
         ";parse_artifact_handoff_case_passed=" + (parse_artifact_handoff_case_passed ? "true" : "false") +
         ";parse_artifact_replay_case_passed=" + (parse_artifact_replay_case_passed ? "true" : "false") +
         ";parse_artifact_diagnostics_hardening_case_passed=" +
         (parse_artifact_diagnostics_hardening_case_passed ? "true" : "false") +
         ";parse_artifact_edge_robustness_case_passed=" +
         (parse_artifact_edge_robustness_case_passed ? "true" : "false") +
         ";parse_recovery_determinism_hardening_case_passed=" +
         (parse_recovery_determinism_hardening_case_passed ? "true" : "false") +
         ";semantic_handoff_case_passed=" + (semantic_handoff_case_passed ? "true" : "false") +
         ";lowering_boundary_case_passed=" + (lowering_boundary_case_passed ? "true" : "false") +
         ";parse_lowering_conformance_matrix_consistent=" +
         (parse_lowering_conformance_matrix_consistent ? "true" : "false") +
         ";consistent=" + (parse_lowering_conformance_corpus_consistent ? "true" : "false");
}

inline std::string BuildObjc3ParseLoweringPerformanceQualityGuardrailsKey(
    std::size_t case_count,
    std::size_t passed_case_count,
    std::size_t failed_case_count,
    bool parser_token_count_budget_consistent,
    bool parser_diagnostic_code_surface_deterministic,
    bool parse_artifact_diagnostics_hardening_consistent,
    bool parse_artifact_edge_case_robustness_consistent,
    bool parse_recovery_determinism_hardening_consistent,
    bool parse_lowering_conformance_corpus_consistent,
    bool parse_lowering_performance_quality_guardrails_consistent) {
  return "case_count=" + std::to_string(case_count) +
         ";passed_case_count=" + std::to_string(passed_case_count) +
         ";failed_case_count=" + std::to_string(failed_case_count) +
         ";parser_token_count_budget_consistent=" +
         (parser_token_count_budget_consistent ? "true" : "false") +
         ";parser_diagnostic_code_surface_deterministic=" +
         (parser_diagnostic_code_surface_deterministic ? "true" : "false") +
         ";parse_artifact_diagnostics_hardening_consistent=" +
         (parse_artifact_diagnostics_hardening_consistent ? "true" : "false") +
         ";parse_artifact_edge_case_robustness_consistent=" +
         (parse_artifact_edge_case_robustness_consistent ? "true" : "false") +
         ";parse_recovery_determinism_hardening_consistent=" +
         (parse_recovery_determinism_hardening_consistent ? "true" : "false") +
         ";parse_lowering_conformance_corpus_consistent=" +
         (parse_lowering_conformance_corpus_consistent ? "true" : "false") +
         ";consistent=" +
         (parse_lowering_performance_quality_guardrails_consistent ? "true" : "false");
}
