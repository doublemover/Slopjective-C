#pragma once

#include <cstddef>
#include <string>

#include "pipeline/parse_lowering_diagnostic_keys.h"
#include "pipeline/objc3_frontend_types.h"
#include "pipeline/readiness/objc3_long_tail_grammar_readiness_keys.h"
#include "pipeline/readiness/objc3_parse_lowering_conformance_keys.h"
#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_readiness_keys.h"

struct Objc3ParseLoweringConformancePerformanceReadinessRecord {
  bool toolchain_runtime_ga_operations_conformance_matrix_consistent = false;
  bool toolchain_runtime_ga_operations_conformance_matrix_ready = false;
  bool parse_lowering_conformance_matrix_ready = false;
  bool toolchain_runtime_ga_operations_conformance_corpus_consistent = false;
  bool toolchain_runtime_ga_operations_conformance_corpus_ready = false;
  bool parse_lowering_conformance_corpus_ready = false;
  bool toolchain_runtime_ga_operations_performance_quality_guardrails_consistent = false;
  bool toolchain_runtime_ga_operations_performance_quality_guardrails_ready = false;
  bool parse_lowering_performance_quality_guardrails_ready = false;
  bool parse_lowering_performance_quality_guardrails_ready_gate = false;
  bool toolchain_runtime_ga_operations_cross_lane_integration_consistent = false;
  bool toolchain_runtime_ga_operations_cross_lane_integration_ready = false;
};

inline Objc3ParseLoweringConformancePerformanceReadinessRecord
ApplyObjc3ParseLoweringConformancePerformanceReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    bool toolchain_runtime_ga_operations_recovery_determinism_consistent,
    bool toolchain_runtime_ga_operations_recovery_determinism_ready,
    bool parse_snapshot_replay_ready,
    bool sema_handoff_ready,
    bool semantic_handoff_deterministic,
    bool typed_core_feature_ready) {
  Objc3ParseLoweringConformancePerformanceReadinessRecord record;

  surface.parse_lowering_conformance_matrix_consistent =
      surface.parse_lowering_conformance_matrix_case_count ==
          kObjc3ParseLoweringConformanceMatrixCaseCount &&
      surface.parse_lowering_conformance_matrix_case_count > 0 &&
      surface.parser_contract_snapshot_present &&
      surface.parse_artifact_handoff_deterministic &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.parse_artifact_diagnostics_hardening_consistent &&
      surface.parse_artifact_edge_case_robustness_consistent &&
      surface.parse_recovery_determinism_hardening_consistent &&
      surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent &&
      surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready &&
      surface.long_tail_grammar_expansion_ready &&
      surface.long_tail_grammar_edge_case_compatibility_ready &&
      surface.long_tail_grammar_edge_case_expansion_consistent &&
      surface.long_tail_grammar_edge_case_robustness_ready &&
      surface.long_tail_grammar_diagnostics_hardening_ready &&
      surface.long_tail_grammar_recovery_determinism_ready &&
      surface.semantic_integration_surface_built &&
      semantic_handoff_deterministic &&
      typed_core_feature_ready &&
      sema_handoff_ready &&
      surface.lowering_boundary_ready &&
      !surface.parse_artifact_handoff_key.empty() &&
      !surface.parse_artifact_replay_key.empty() &&
      !surface.parse_artifact_diagnostics_hardening_key.empty() &&
      !surface.parse_artifact_edge_robustness_key.empty() &&
      !surface.parse_recovery_determinism_hardening_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_conformance_matrix_key.empty() &&
      !surface.long_tail_grammar_expansion_key.empty() &&
      !surface.long_tail_grammar_edge_case_compatibility_key.empty() &&
      !surface.long_tail_grammar_edge_case_robustness_key.empty() &&
      !surface.long_tail_grammar_diagnostics_hardening_key.empty() &&
      !surface.long_tail_grammar_recovery_determinism_key.empty() &&
      !surface.typed_sema_core_feature_key.empty() &&
      !surface.typed_sema_core_feature_expansion_key.empty() &&
      !surface.lowering_boundary_replay_key.empty();
  surface.long_tail_grammar_conformance_matrix_consistent =
      surface.long_tail_grammar_recovery_determinism_ready &&
      surface.parse_lowering_conformance_matrix_consistent &&
      surface.parse_artifact_replay_key_deterministic &&
      !surface.long_tail_grammar_recovery_determinism_key.empty() &&
      !surface.parse_artifact_replay_key.empty();
  surface.long_tail_grammar_conformance_matrix_ready =
      surface.long_tail_grammar_conformance_matrix_consistent &&
      surface.parse_lowering_conformance_corpus_case_count > 0 &&
      surface.parse_lowering_performance_quality_guardrails_case_count > 0;
  surface.long_tail_grammar_conformance_matrix_key =
      BuildObjc3LongTailGrammarConformanceMatrixKey(
          surface.parse_lowering_conformance_matrix_case_count,
          surface.parse_lowering_conformance_corpus_case_count,
          surface.parse_lowering_performance_quality_guardrails_case_count,
          surface.parse_artifact_replay_key_deterministic,
          surface.long_tail_grammar_recovery_determinism_ready,
          surface.long_tail_grammar_conformance_matrix_consistent,
          surface.long_tail_grammar_conformance_matrix_ready);
  surface.parse_lowering_conformance_matrix_key =
      BuildObjc3ParseLoweringConformanceMatrixKey(
          surface.parse_lowering_conformance_matrix_case_count,
          surface.parser_contract_snapshot_present,
          surface.parse_artifact_handoff_deterministic,
          surface.parse_artifact_replay_key_deterministic,
          surface.parse_artifact_diagnostics_hardening_consistent,
          surface.parse_artifact_edge_case_robustness_consistent,
          surface.parse_recovery_determinism_hardening_consistent,
          surface.semantic_integration_surface_built,
          semantic_handoff_deterministic,
          surface.lowering_boundary_ready,
          surface.parse_lowering_conformance_matrix_consistent);
  record.toolchain_runtime_ga_operations_conformance_matrix_consistent =
      IsObjc3ToolchainRuntimeGaOperationsConformanceMatrixConsistent(
          toolchain_runtime_ga_operations_recovery_determinism_consistent,
          toolchain_runtime_ga_operations_recovery_determinism_ready,
          surface.parse_lowering_conformance_matrix_consistent,
          surface.long_tail_grammar_conformance_matrix_consistent,
          surface.long_tail_grammar_conformance_matrix_ready,
          surface.parse_recovery_determinism_hardening_key,
          surface.long_tail_grammar_recovery_determinism_key);
  record.toolchain_runtime_ga_operations_conformance_matrix_ready =
      IsObjc3ToolchainRuntimeGaOperationsConformanceMatrixReady(
          record.toolchain_runtime_ga_operations_conformance_matrix_consistent,
          surface.parse_lowering_conformance_matrix_key,
          surface.long_tail_grammar_conformance_matrix_key);
  const std::string toolchain_runtime_ga_operations_conformance_matrix_key =
      BuildObjc3ToolchainRuntimeGaOperationsConformanceMatrixKey(
          surface.parse_lowering_conformance_matrix_consistent,
          surface.long_tail_grammar_conformance_matrix_consistent,
          surface.long_tail_grammar_conformance_matrix_ready,
          surface.parse_lowering_conformance_matrix_key,
          surface.long_tail_grammar_conformance_matrix_key,
          record.toolchain_runtime_ga_operations_conformance_matrix_consistent,
          record.toolchain_runtime_ga_operations_conformance_matrix_ready);
  surface.long_tail_grammar_conformance_matrix_consistent =
      surface.long_tail_grammar_conformance_matrix_consistent &&
      record.toolchain_runtime_ga_operations_conformance_matrix_consistent;
  surface.long_tail_grammar_conformance_matrix_ready =
      surface.long_tail_grammar_conformance_matrix_ready &&
      record.toolchain_runtime_ga_operations_conformance_matrix_ready;
  surface.long_tail_grammar_conformance_matrix_key +=
      ";toolchain_runtime_ga_operations_conformance_matrix_key=" +
      toolchain_runtime_ga_operations_conformance_matrix_key;
  surface.parse_lowering_conformance_matrix_key +=
      ";toolchain_runtime_ga_operations_conformance_matrix_key=" +
      toolchain_runtime_ga_operations_conformance_matrix_key;
  record.parse_lowering_conformance_matrix_ready =
      surface.parse_lowering_conformance_matrix_consistent &&
      record.toolchain_runtime_ga_operations_conformance_matrix_ready;

  const bool parser_contract_snapshot_case_passed =
      surface.parser_contract_snapshot_present;
  const bool parse_artifact_handoff_case_passed =
      (surface.parse_artifact_handoff_deterministic);
  const bool parse_artifact_replay_case_passed =
      (surface.parse_artifact_replay_key_deterministic);
  const bool parse_artifact_diagnostics_hardening_case_passed =
      surface.parse_artifact_diagnostics_hardening_consistent;
  const bool parse_artifact_edge_robustness_case_passed =
      surface.parse_artifact_edge_case_robustness_consistent;
  const bool parse_recovery_determinism_hardening_case_passed =
      surface.parse_recovery_determinism_hardening_consistent;
  const bool semantic_handoff_case_passed =
      surface.semantic_integration_surface_built &&
      semantic_handoff_deterministic &&
      sema_handoff_ready;
  const bool lowering_boundary_case_passed =
      surface.lowering_boundary_ready &&
      !surface.lowering_boundary_replay_key.empty();
  surface.parse_lowering_conformance_corpus_passed_case_count =
      static_cast<std::size_t>(parser_contract_snapshot_case_passed) +
      static_cast<std::size_t>(parse_artifact_handoff_case_passed) +
      static_cast<std::size_t>(parse_artifact_replay_case_passed) +
      static_cast<std::size_t>(parse_artifact_diagnostics_hardening_case_passed) +
      static_cast<std::size_t>(parse_artifact_edge_robustness_case_passed) +
      static_cast<std::size_t>(parse_recovery_determinism_hardening_case_passed) +
      static_cast<std::size_t>(semantic_handoff_case_passed) +
      static_cast<std::size_t>(lowering_boundary_case_passed);
  surface.parse_lowering_conformance_corpus_failed_case_count =
      surface.parse_lowering_conformance_corpus_case_count >=
              surface.parse_lowering_conformance_corpus_passed_case_count
          ? (surface.parse_lowering_conformance_corpus_case_count -
             surface.parse_lowering_conformance_corpus_passed_case_count)
          : surface.parse_lowering_conformance_corpus_case_count;
  surface.parse_lowering_conformance_corpus_consistent =
      surface.parse_lowering_conformance_matrix_consistent &&
      surface.long_tail_grammar_conformance_matrix_ready &&
      surface.parse_lowering_conformance_corpus_case_count ==
          kObjc3ParseLoweringConformanceCorpusCaseCount &&
      surface.parse_lowering_conformance_corpus_case_count > 0 &&
      surface.parse_lowering_conformance_corpus_passed_case_count ==
          surface.parse_lowering_conformance_corpus_case_count &&
      surface.parse_lowering_conformance_corpus_failed_case_count == 0 &&
      parser_contract_snapshot_case_passed &&
      parse_artifact_handoff_case_passed &&
      parse_artifact_replay_case_passed &&
      parse_artifact_diagnostics_hardening_case_passed &&
      parse_artifact_edge_robustness_case_passed &&
      parse_recovery_determinism_hardening_case_passed &&
      semantic_handoff_case_passed &&
      lowering_boundary_case_passed;
  surface.parse_lowering_conformance_corpus_key =
      BuildObjc3ParseLoweringConformanceCorpusKey(
          surface.parse_lowering_conformance_corpus_case_count,
          surface.parse_lowering_conformance_corpus_passed_case_count,
          surface.parse_lowering_conformance_corpus_failed_case_count,
          parser_contract_snapshot_case_passed,
          parse_artifact_handoff_case_passed,
          parse_artifact_replay_case_passed,
          parse_artifact_diagnostics_hardening_case_passed,
          parse_artifact_edge_robustness_case_passed,
          parse_recovery_determinism_hardening_case_passed,
          semantic_handoff_case_passed,
          lowering_boundary_case_passed,
          surface.parse_lowering_conformance_matrix_consistent,
          surface.parse_lowering_conformance_corpus_consistent);
  record.toolchain_runtime_ga_operations_conformance_corpus_consistent =
      IsObjc3ToolchainRuntimeGaOperationsConformanceCorpusConsistent(
          record.toolchain_runtime_ga_operations_conformance_matrix_consistent,
          record.toolchain_runtime_ga_operations_conformance_matrix_ready,
          surface.parse_lowering_conformance_matrix_consistent,
          surface.parse_lowering_conformance_corpus_consistent,
          surface.parse_lowering_conformance_matrix_key,
          surface.parse_lowering_conformance_corpus_key,
          surface.long_tail_grammar_conformance_matrix_key);
  record.toolchain_runtime_ga_operations_conformance_corpus_ready =
      IsObjc3ToolchainRuntimeGaOperationsConformanceCorpusReady(
          record.toolchain_runtime_ga_operations_conformance_corpus_consistent,
          surface.parse_lowering_conformance_corpus_key);
  const std::string toolchain_runtime_ga_operations_conformance_corpus_key =
      BuildObjc3ToolchainRuntimeGaOperationsConformanceCorpusKey(
          surface.parse_lowering_conformance_matrix_consistent,
          surface.parse_lowering_conformance_corpus_consistent,
          surface.parse_lowering_conformance_matrix_key,
          surface.parse_lowering_conformance_corpus_key,
          surface.long_tail_grammar_conformance_matrix_key,
          record.toolchain_runtime_ga_operations_conformance_corpus_consistent,
          record.toolchain_runtime_ga_operations_conformance_corpus_ready);
  surface.parse_lowering_conformance_corpus_consistent =
      surface.parse_lowering_conformance_corpus_consistent &&
      record.toolchain_runtime_ga_operations_conformance_corpus_consistent;
  surface.parse_lowering_conformance_corpus_key +=
      ";toolchain_runtime_ga_operations_conformance_corpus_key=" +
      toolchain_runtime_ga_operations_conformance_corpus_key;
  surface.long_tail_grammar_conformance_matrix_key +=
      ";toolchain_runtime_ga_operations_conformance_corpus_key=" +
      toolchain_runtime_ga_operations_conformance_corpus_key;
  record.parse_lowering_conformance_corpus_ready =
      surface.parse_lowering_conformance_corpus_consistent &&
      record.toolchain_runtime_ga_operations_conformance_corpus_ready;
  surface.parser_diagnostic_grammar_hooks_conformance_corpus_consistent =
      surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent &&
      surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready &&
      surface.parse_lowering_conformance_corpus_consistent &&
      surface.parse_lowering_conformance_corpus_case_count ==
          kObjc3ParseLoweringConformanceCorpusCaseCount &&
      surface.parse_lowering_conformance_corpus_case_count > 0 &&
      !surface.parser_diagnostic_grammar_hooks_conformance_matrix_key.empty() &&
      !surface.parse_lowering_conformance_corpus_key.empty();
  const bool parser_diagnostic_grammar_hooks_conformance_corpus_ready_gate =
      surface.parser_diagnostic_grammar_hooks_conformance_corpus_consistent &&
      record.parse_lowering_conformance_corpus_ready &&
      surface.parse_lowering_performance_quality_guardrails_case_count > 0;
  surface.parser_diagnostic_grammar_hooks_conformance_corpus_key =
      BuildObjc3DiagnosticGrammarHooksConformanceCorpusKey(
          surface.parse_lowering_conformance_corpus_case_count,
          surface.parse_lowering_conformance_corpus_passed_case_count,
          surface.parse_lowering_conformance_corpus_failed_case_count,
          surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready,
          surface.parse_artifact_replay_key_deterministic,
          surface.parser_diagnostic_grammar_hooks_conformance_corpus_consistent,
          parser_diagnostic_grammar_hooks_conformance_corpus_ready_gate);
  surface.parser_diagnostic_grammar_hooks_conformance_corpus_ready =
      parser_diagnostic_grammar_hooks_conformance_corpus_ready_gate &&
      !surface.parser_diagnostic_grammar_hooks_conformance_corpus_key.empty();

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

  record.toolchain_runtime_ga_operations_cross_lane_integration_consistent =
      IsObjc3ToolchainRuntimeGaOperationsCrossLaneIntegrationConsistent(
          record.toolchain_runtime_ga_operations_performance_quality_guardrails_consistent,
          record.toolchain_runtime_ga_operations_performance_quality_guardrails_ready,
          parse_snapshot_replay_ready,
          sema_handoff_ready,
          surface.lowering_boundary_ready,
          surface.parse_lowering_conformance_corpus_consistent,
          surface.parse_lowering_performance_quality_guardrails_consistent,
          surface.parse_artifact_replay_key,
          surface.lowering_boundary_replay_key,
          surface.parse_lowering_conformance_corpus_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  record.toolchain_runtime_ga_operations_cross_lane_integration_ready =
      IsObjc3ToolchainRuntimeGaOperationsCrossLaneIntegrationReady(
          record.toolchain_runtime_ga_operations_cross_lane_integration_consistent,
          surface.parse_artifact_replay_key,
          surface.lowering_boundary_replay_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  const std::string toolchain_runtime_ga_operations_cross_lane_integration_key =
      BuildObjc3ToolchainRuntimeGaOperationsCrossLaneIntegrationKey(
          parse_snapshot_replay_ready,
          sema_handoff_ready,
          surface.lowering_boundary_ready,
          surface.parse_lowering_conformance_corpus_consistent,
          surface.parse_lowering_performance_quality_guardrails_consistent,
          surface.parse_artifact_replay_key,
          surface.lowering_boundary_replay_key,
          surface.parse_lowering_conformance_corpus_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          record.toolchain_runtime_ga_operations_cross_lane_integration_consistent,
          record.toolchain_runtime_ga_operations_cross_lane_integration_ready);
  surface.toolchain_runtime_ga_operations_cross_lane_integration_consistent =
      record.toolchain_runtime_ga_operations_cross_lane_integration_consistent;
  surface.toolchain_runtime_ga_operations_cross_lane_integration_ready =
      record.toolchain_runtime_ga_operations_cross_lane_integration_ready;
  surface.toolchain_runtime_ga_operations_cross_lane_integration_key =
      toolchain_runtime_ga_operations_cross_lane_integration_key;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_cross_lane_integration_key=" +
      toolchain_runtime_ga_operations_cross_lane_integration_key;
  surface.long_tail_grammar_conformance_matrix_key +=
      ";toolchain_runtime_ga_operations_cross_lane_integration_key=" +
      toolchain_runtime_ga_operations_cross_lane_integration_key;

  return record;
}
