#include "pipeline/readiness/objc3_parse_lowering_conformance_performance_readiness_private.h"

#include <cstddef>
#include <string>

#include "pipeline/readiness/objc3_parse_lowering_conformance_keys.h"
#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_readiness_keys.h"

void ApplyObjc3ParseLoweringPerformanceQualityGuardrailsReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    Objc3ParseLoweringConformancePerformanceReadinessRecord &record) {
  const bool parser_token_budget_guardrail_case_passed =
      surface.parser_token_count_budget_consistent;
  const bool parser_diagnostic_code_surface_guardrail_case_passed =
      surface.parser_diagnostic_code_surface_deterministic;
  const bool parse_artifact_diagnostics_hardening_guardrail_case_passed =
      surface.parse_artifact_diagnostics_hardening_consistent;
  const bool parse_artifact_edge_robustness_guardrail_case_passed =
      surface.parse_artifact_edge_case_robustness_consistent;
  const bool parse_recovery_determinism_hardening_guardrail_case_passed =
      surface.parse_recovery_determinism_hardening_consistent;
  const bool parse_lowering_conformance_corpus_guardrail_case_passed =
      surface.parse_lowering_conformance_corpus_consistent;
  surface.parse_lowering_performance_quality_guardrails_passed_case_count =
      static_cast<std::size_t>(parser_token_budget_guardrail_case_passed) +
      static_cast<std::size_t>(parser_diagnostic_code_surface_guardrail_case_passed) +
      static_cast<std::size_t>(parse_artifact_diagnostics_hardening_guardrail_case_passed) +
      static_cast<std::size_t>(parse_artifact_edge_robustness_guardrail_case_passed) +
      static_cast<std::size_t>(parse_recovery_determinism_hardening_guardrail_case_passed) +
      static_cast<std::size_t>(parse_lowering_conformance_corpus_guardrail_case_passed);
  surface.parse_lowering_performance_quality_guardrails_failed_case_count =
      surface.parse_lowering_performance_quality_guardrails_case_count >=
              surface.parse_lowering_performance_quality_guardrails_passed_case_count
          ? (surface.parse_lowering_performance_quality_guardrails_case_count -
             surface.parse_lowering_performance_quality_guardrails_passed_case_count)
          : surface.parse_lowering_performance_quality_guardrails_case_count;
  surface.parse_lowering_performance_quality_guardrails_consistent =
      surface.parse_lowering_performance_quality_guardrails_case_count ==
          kObjc3ParseLoweringPerformanceQualityGuardrailsCaseCount &&
      surface.parse_lowering_performance_quality_guardrails_case_count > 0 &&
      surface.parse_lowering_performance_quality_guardrails_passed_case_count ==
          surface.parse_lowering_performance_quality_guardrails_case_count &&
      surface.parse_lowering_performance_quality_guardrails_failed_case_count == 0 &&
      parser_token_budget_guardrail_case_passed &&
      parser_diagnostic_code_surface_guardrail_case_passed &&
      parse_artifact_diagnostics_hardening_guardrail_case_passed &&
      parse_artifact_edge_robustness_guardrail_case_passed &&
      parse_recovery_determinism_hardening_guardrail_case_passed &&
      parse_lowering_conformance_corpus_guardrail_case_passed &&
      surface.long_tail_grammar_diagnostics_hardening_ready &&
      surface.long_tail_grammar_recovery_determinism_ready &&
      surface.long_tail_grammar_conformance_matrix_ready &&
      !surface.parse_artifact_diagnostics_hardening_key.empty() &&
      !surface.parse_artifact_edge_robustness_key.empty() &&
      !surface.parse_recovery_determinism_hardening_key.empty() &&
      !surface.long_tail_grammar_diagnostics_hardening_key.empty() &&
      !surface.long_tail_grammar_recovery_determinism_key.empty() &&
      !surface.long_tail_grammar_conformance_matrix_key.empty() &&
      !surface.parse_lowering_conformance_corpus_key.empty();
  surface.parse_lowering_performance_quality_guardrails_key =
      BuildObjc3ParseLoweringPerformanceQualityGuardrailsKey(
          surface.parse_lowering_performance_quality_guardrails_case_count,
          surface.parse_lowering_performance_quality_guardrails_passed_case_count,
          surface.parse_lowering_performance_quality_guardrails_failed_case_count,
          surface.parser_token_count_budget_consistent,
          surface.parser_diagnostic_code_surface_deterministic,
          surface.parse_artifact_diagnostics_hardening_consistent,
          surface.parse_artifact_edge_case_robustness_consistent,
          surface.parse_recovery_determinism_hardening_consistent,
          surface.parse_lowering_conformance_corpus_consistent,
          surface.parse_lowering_performance_quality_guardrails_consistent);
  record.toolchain_runtime_ga_operations_performance_quality_guardrails_consistent =
      IsObjc3ToolchainRuntimeGaOperationsPerformanceQualityGuardrailsConsistent(
          record.toolchain_runtime_ga_operations_conformance_corpus_consistent,
          record.toolchain_runtime_ga_operations_conformance_corpus_ready,
          surface.parse_lowering_conformance_corpus_consistent,
          surface.parse_lowering_performance_quality_guardrails_consistent,
          surface.parse_lowering_conformance_corpus_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          surface.long_tail_grammar_conformance_matrix_key);
  record.toolchain_runtime_ga_operations_performance_quality_guardrails_ready =
      IsObjc3ToolchainRuntimeGaOperationsPerformanceQualityGuardrailsReady(
          record.toolchain_runtime_ga_operations_performance_quality_guardrails_consistent,
          surface.parse_lowering_performance_quality_guardrails_key);
  const std::string toolchain_runtime_ga_operations_performance_quality_guardrails_key =
      BuildObjc3ToolchainRuntimeGaOperationsPerformanceQualityGuardrailsKey(
          surface.parse_lowering_conformance_corpus_consistent,
          surface.parse_lowering_performance_quality_guardrails_consistent,
          surface.parse_lowering_conformance_corpus_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          surface.long_tail_grammar_conformance_matrix_key,
          record.toolchain_runtime_ga_operations_performance_quality_guardrails_consistent,
          record.toolchain_runtime_ga_operations_performance_quality_guardrails_ready);
  surface.parse_lowering_performance_quality_guardrails_consistent &=
      record.toolchain_runtime_ga_operations_performance_quality_guardrails_consistent;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_performance_quality_guardrails_key=" +
      toolchain_runtime_ga_operations_performance_quality_guardrails_key;
  surface.long_tail_grammar_conformance_matrix_key +=
      ";toolchain_runtime_ga_operations_performance_quality_guardrails_key=" +
      toolchain_runtime_ga_operations_performance_quality_guardrails_key;
  record.parse_lowering_performance_quality_guardrails_ready =
      surface.parse_lowering_performance_quality_guardrails_consistent &&
      record.toolchain_runtime_ga_operations_performance_quality_guardrails_ready;
  record.parse_lowering_performance_quality_guardrails_ready_gate =
      record.parse_lowering_performance_quality_guardrails_ready;
}
