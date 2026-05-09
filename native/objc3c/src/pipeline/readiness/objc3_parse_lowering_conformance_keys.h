#pragma once

#include <cstddef>
#include <string>

inline constexpr std::size_t kObjc3ParseLoweringConformanceMatrixCaseCount = 8u;
inline constexpr std::size_t kObjc3ParseLoweringConformanceCorpusCaseCount = 8u;
inline constexpr std::size_t kObjc3ParseLoweringPerformanceQualityGuardrailsCaseCount = 6u;

std::string BuildObjc3ParseLoweringConformanceMatrixKey(
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
    bool parse_lowering_conformance_matrix_consistent);

std::string BuildObjc3ParseLoweringConformanceCorpusKey(
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
    bool parse_lowering_conformance_corpus_consistent);

std::string BuildObjc3ParseLoweringPerformanceQualityGuardrailsKey(
    std::size_t case_count,
    std::size_t passed_case_count,
    std::size_t failed_case_count,
    bool parser_token_count_budget_consistent,
    bool parser_diagnostic_code_surface_deterministic,
    bool parse_artifact_diagnostics_hardening_consistent,
    bool parse_artifact_edge_case_robustness_consistent,
    bool parse_recovery_determinism_hardening_consistent,
    bool parse_lowering_conformance_corpus_consistent,
    bool parse_lowering_performance_quality_guardrails_consistent);
