#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

#include "parse/objc3_diagnostic_grammar_hooks_core_feature.h"
#include "parse/objc3_diagnostic_grammar_hooks_core_feature_expansion_surface.h"
#include "parse/objc3_diagnostic_grammar_hooks_edge_case_compatibility_surface.h"
#include "parse/objc3_diagnostic_source_precision_scaffold.h"
#include "pipeline/parse_lowering_artifact_keys.h"
#include "pipeline/parse_lowering_diagnostic_keys.h"
#include "pipeline/objc3_frontend_types.h"
#include "pipeline/readiness/objc3_long_tail_grammar_readiness_keys.h"
#include "pipeline/readiness/objc3_parse_lowering_conformance_performance_readiness.h"
#include "pipeline/readiness/objc3_parse_lowering_conformance_keys.h"
#include "pipeline/readiness/objc3_parse_lowering_parser_behavior_readiness.h"
#include "pipeline/readiness/objc3_typed_sema_lowering_readiness.h"
#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_closeout_readiness.h"
#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_readiness_keys.h"

inline Objc3ParseLoweringReadinessSurface BuildObjc3ParseLoweringReadinessSurface(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  Objc3ParseLoweringReadinessSurface surface;
  BuildObjc3ParseLoweringParserBehaviorReadiness(surface, pipeline_result, options);
  surface.parse_recovery_determinism_hardening_consistent =
      surface.parser_contract_snapshot_present &&
      surface.parser_contract_deterministic &&
      surface.parser_recovery_replay_ready &&
      surface.long_tail_grammar_core_feature_consistent &&
      surface.long_tail_grammar_handoff_key_deterministic &&
      surface.long_tail_grammar_expansion_accounting_consistent &&
      surface.long_tail_grammar_replay_keys_ready &&
      surface.long_tail_grammar_expansion_ready &&
      surface.long_tail_grammar_compatibility_handoff_ready &&
      surface.long_tail_grammar_edge_case_compatibility_consistent &&
      surface.long_tail_grammar_edge_case_compatibility_ready &&
      surface.long_tail_grammar_edge_case_expansion_consistent &&
      surface.long_tail_grammar_edge_case_robustness_ready &&
      surface.long_tail_grammar_diagnostics_hardening_ready &&
      surface.parse_artifact_handoff_deterministic &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.parse_artifact_diagnostics_hardening_consistent &&
      surface.parse_artifact_edge_case_robustness_consistent &&
      surface.parser_diagnostic_grammar_hooks_edge_case_expansion_consistent &&
      surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready &&
      surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent &&
      surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready &&
      !surface.long_tail_grammar_handoff_key.empty() &&
      !surface.long_tail_grammar_expansion_key.empty() &&
      !surface.long_tail_grammar_edge_case_compatibility_key.empty() &&
      !surface.long_tail_grammar_edge_case_robustness_key.empty() &&
      !surface.long_tail_grammar_diagnostics_hardening_key.empty() &&
      !surface.parse_artifact_handoff_key.empty() &&
      !surface.parse_artifact_replay_key.empty() &&
      !surface.parse_artifact_diagnostics_hardening_key.empty() &&
      !surface.parse_artifact_edge_robustness_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_edge_case_robustness_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_key.empty();
  surface.parse_recovery_determinism_hardening_key = BuildObjc3ParseRecoveryDeterminismHardeningKey(
      surface.parser_contract_snapshot_present,
      surface.parser_contract_deterministic,
      surface.parser_recovery_replay_ready,
      surface.long_tail_grammar_core_feature_consistent,
      surface.long_tail_grammar_handoff_key_deterministic,
      surface.long_tail_grammar_expansion_accounting_consistent,
      surface.long_tail_grammar_replay_keys_ready,
      surface.long_tail_grammar_expansion_ready,
      surface.long_tail_grammar_compatibility_handoff_ready,
      surface.long_tail_grammar_edge_case_compatibility_consistent,
      surface.long_tail_grammar_edge_case_compatibility_ready,
      surface.long_tail_grammar_edge_case_expansion_consistent,
      surface.long_tail_grammar_edge_case_robustness_ready,
      surface.long_tail_grammar_diagnostics_hardening_ready,
      surface.parse_artifact_handoff_deterministic,
      surface.parse_artifact_replay_key_deterministic,
      surface.parse_artifact_diagnostics_hardening_consistent,
      surface.parse_artifact_edge_case_robustness_consistent,
      surface.parse_recovery_determinism_hardening_consistent);
  surface.parser_diagnostic_grammar_hooks_recovery_determinism_consistent =
      surface.parse_recovery_determinism_hardening_consistent &&
      surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready &&
      surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready &&
      surface.parser_recovery_replay_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      !surface.parse_recovery_determinism_hardening_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_edge_case_robustness_key.empty();
  surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready =
      surface.parser_diagnostic_grammar_hooks_recovery_determinism_consistent &&
      surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready;
  surface.parser_diagnostic_grammar_hooks_recovery_determinism_key =
      BuildObjc3DiagnosticGrammarHooksRecoveryDeterminismKey(
          surface.parser_recovery_replay_ready,
          surface.parse_artifact_replay_key_deterministic,
          surface.parse_recovery_determinism_hardening_consistent,
          surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready,
          surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready,
          surface.parser_diagnostic_grammar_hooks_recovery_determinism_consistent,
          surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready);
  surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent =
      surface.parser_diagnostic_grammar_hooks_recovery_determinism_consistent &&
      surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready &&
      surface.parse_lowering_conformance_matrix_case_count ==
          kObjc3ParseLoweringConformanceMatrixCaseCount &&
      surface.parse_lowering_conformance_matrix_case_count > 0 &&
      surface.parse_artifact_replay_key_deterministic &&
      !surface.parser_diagnostic_grammar_hooks_recovery_determinism_key.empty() &&
      !surface.parse_recovery_determinism_hardening_key.empty() &&
      !surface.parse_artifact_replay_key.empty();
  surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready =
      surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent &&
      surface.parse_lowering_conformance_corpus_case_count > 0 &&
      surface.parse_lowering_performance_quality_guardrails_case_count > 0;
  surface.parser_diagnostic_grammar_hooks_conformance_matrix_key =
      BuildObjc3DiagnosticGrammarHooksConformanceMatrixKey(
          surface.parse_lowering_conformance_matrix_case_count,
          surface.parse_lowering_conformance_corpus_case_count,
          surface.parse_lowering_performance_quality_guardrails_case_count,
          surface.parse_artifact_replay_key_deterministic,
          surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready,
          surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent,
          surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready);
  surface.long_tail_grammar_recovery_determinism_consistent =
      surface.parse_recovery_determinism_hardening_consistent &&
      surface.parser_diagnostic_grammar_hooks_recovery_determinism_consistent &&
      surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready &&
      surface.long_tail_grammar_diagnostics_hardening_ready &&
      surface.parser_recovery_replay_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      !surface.parse_recovery_determinism_hardening_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_recovery_determinism_key.empty() &&
      !surface.long_tail_grammar_diagnostics_hardening_key.empty();
  surface.long_tail_grammar_recovery_determinism_ready =
      surface.long_tail_grammar_recovery_determinism_consistent &&
      surface.long_tail_grammar_expansion_ready &&
      surface.long_tail_grammar_edge_case_robustness_ready;
  surface.long_tail_grammar_recovery_determinism_key =
      BuildObjc3LongTailGrammarRecoveryDeterminismKey(
          surface.parser_recovery_replay_ready,
          surface.parse_artifact_replay_key_deterministic,
          surface.parse_recovery_determinism_hardening_consistent,
          surface.long_tail_grammar_diagnostics_hardening_ready,
          surface.long_tail_grammar_edge_case_robustness_ready,
          surface.long_tail_grammar_expansion_ready,
          surface.long_tail_grammar_recovery_determinism_consistent,
          surface.long_tail_grammar_recovery_determinism_ready);
  const bool toolchain_runtime_ga_operations_recovery_determinism_consistent =
      IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningConsistent(
          surface.parser_recovery_replay_ready,
          surface.parse_artifact_replay_key_deterministic,
          surface.long_tail_grammar_replay_keys_ready,
          surface.long_tail_grammar_diagnostics_hardening_ready,
          surface.parse_recovery_determinism_hardening_consistent,
          surface.parse_artifact_handoff_key,
          surface.parse_artifact_replay_key,
          surface.parse_artifact_diagnostics_hardening_key,
          surface.parse_artifact_edge_robustness_key,
          surface.long_tail_grammar_handoff_key,
          surface.long_tail_grammar_diagnostics_hardening_key,
          surface.parse_recovery_determinism_hardening_key);
  const bool toolchain_runtime_ga_operations_recovery_determinism_ready =
      IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningReady(
          toolchain_runtime_ga_operations_recovery_determinism_consistent,
          surface.long_tail_grammar_recovery_determinism_consistent,
          surface.long_tail_grammar_recovery_determinism_ready,
          surface.long_tail_grammar_recovery_determinism_key);
  surface.parse_recovery_determinism_hardening_consistent &=
      toolchain_runtime_ga_operations_recovery_determinism_consistent;
  surface.long_tail_grammar_recovery_determinism_consistent =
      surface.long_tail_grammar_recovery_determinism_consistent &&
      toolchain_runtime_ga_operations_recovery_determinism_consistent;
  surface.long_tail_grammar_recovery_determinism_ready =
      surface.long_tail_grammar_recovery_determinism_ready &&
      toolchain_runtime_ga_operations_recovery_determinism_ready;
  surface.parse_recovery_determinism_hardening_key =
      BuildObjc3ParseRecoveryDeterminismHardeningKey(
          surface.parser_contract_snapshot_present,
          surface.parser_contract_deterministic,
          surface.parser_recovery_replay_ready,
          surface.long_tail_grammar_core_feature_consistent,
          surface.long_tail_grammar_handoff_key_deterministic,
          surface.long_tail_grammar_expansion_accounting_consistent,
          surface.long_tail_grammar_replay_keys_ready,
          surface.long_tail_grammar_expansion_ready,
          surface.long_tail_grammar_compatibility_handoff_ready,
          surface.long_tail_grammar_edge_case_compatibility_consistent,
          surface.long_tail_grammar_edge_case_compatibility_ready,
          surface.long_tail_grammar_edge_case_expansion_consistent,
          surface.long_tail_grammar_edge_case_robustness_ready,
          surface.long_tail_grammar_diagnostics_hardening_ready,
          surface.parse_artifact_handoff_deterministic,
          surface.parse_artifact_replay_key_deterministic,
          surface.parse_artifact_diagnostics_hardening_consistent,
          surface.parse_artifact_edge_case_robustness_consistent,
          surface.parse_recovery_determinism_hardening_consistent);
  surface.long_tail_grammar_recovery_determinism_key =
      BuildObjc3LongTailGrammarRecoveryDeterminismKey(
          surface.parser_recovery_replay_ready,
          surface.parse_artifact_replay_key_deterministic,
          surface.parse_recovery_determinism_hardening_consistent,
          surface.long_tail_grammar_diagnostics_hardening_ready,
          surface.long_tail_grammar_edge_case_robustness_ready,
          surface.long_tail_grammar_expansion_ready,
          surface.long_tail_grammar_recovery_determinism_consistent,
          surface.long_tail_grammar_recovery_determinism_ready);
  const std::string toolchain_runtime_ga_operations_recovery_determinism_key =
      BuildObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningKey(
          surface.parser_recovery_replay_ready,
          surface.parse_artifact_replay_key_deterministic,
          surface.long_tail_grammar_replay_keys_ready,
          surface.long_tail_grammar_diagnostics_hardening_ready,
          surface.parse_artifact_handoff_key,
          surface.parse_artifact_replay_key,
          surface.parse_artifact_diagnostics_hardening_key,
          surface.parse_artifact_edge_robustness_key,
          surface.long_tail_grammar_handoff_key,
          surface.long_tail_grammar_diagnostics_hardening_key,
          surface.parse_recovery_determinism_hardening_key,
          surface.long_tail_grammar_recovery_determinism_key,
          toolchain_runtime_ga_operations_recovery_determinism_consistent,
          toolchain_runtime_ga_operations_recovery_determinism_ready);
  surface.parse_recovery_determinism_hardening_key +=
      ";toolchain_runtime_ga_operations_recovery_determinism_key=" +
      toolchain_runtime_ga_operations_recovery_determinism_key;
  surface.long_tail_grammar_recovery_determinism_key +=
      ";toolchain_runtime_ga_operations_recovery_determinism_key=" +
      toolchain_runtime_ga_operations_recovery_determinism_key;
  const Objc3TypedSemaLoweringReadinessRecord typed_sema_lowering_readiness =
      BuildObjc3TypedSemaLoweringReadiness(surface, pipeline_result, options);

  const bool diagnostics_clear =
      surface.lexer_diagnostic_count == 0 &&
      surface.parser_diagnostic_count == 0 &&
      surface.semantic_diagnostic_count == 0;
  const bool parse_snapshot_ready =
      surface.parser_contract_snapshot_present &&
      surface.parser_contract_deterministic &&
      surface.parser_recovery_replay_ready &&
      surface.long_tail_grammar_core_feature_consistent &&
      surface.long_tail_grammar_handoff_key_deterministic &&
      surface.long_tail_grammar_expansion_ready &&
      surface.long_tail_grammar_compatibility_handoff_ready &&
      surface.long_tail_grammar_edge_case_compatibility_ready &&
      surface.long_tail_grammar_edge_case_expansion_consistent &&
      surface.long_tail_grammar_edge_case_robustness_ready &&
      surface.long_tail_grammar_diagnostics_hardening_ready &&
      surface.long_tail_grammar_recovery_determinism_ready &&
      surface.parse_artifact_handoff_deterministic;
  const bool parse_artifact_replay_key_ready =
      surface.parse_artifact_replay_key_deterministic;
  const bool parse_artifact_diagnostics_hardening_ready =
      surface.parse_artifact_diagnostics_hardening_consistent;
  const bool parse_recovery_determinism_hardening_ready =
      surface.parse_recovery_determinism_hardening_consistent;
  const bool parse_snapshot_replay_ready =
      parse_snapshot_ready &&
      parse_artifact_replay_key_ready &&
      parse_artifact_diagnostics_hardening_ready &&
      surface.parse_artifact_edge_case_robustness_consistent &&
      surface.long_tail_grammar_edge_case_robustness_ready &&
      surface.long_tail_grammar_diagnostics_hardening_ready &&
      surface.long_tail_grammar_recovery_determinism_ready &&
      parse_recovery_determinism_hardening_ready;
  const bool typed_edge_case_compatibility_alignment =
      typed_sema_lowering_readiness.typed_edge_case_compatibility_alignment;
  const bool typed_edge_case_robustness_alignment =
      typed_sema_lowering_readiness.typed_edge_case_robustness_alignment;
  const bool typed_diagnostics_hardening_alignment =
      typed_sema_lowering_readiness.typed_diagnostics_hardening_alignment;
  const bool typed_recovery_determinism_alignment =
      typed_sema_lowering_readiness.typed_recovery_determinism_alignment;
  const bool typed_conformance_matrix_alignment =
      typed_sema_lowering_readiness.typed_conformance_matrix_alignment;
  const bool typed_conformance_corpus_alignment =
      typed_sema_lowering_readiness.typed_conformance_corpus_alignment;
  const bool typed_performance_quality_guardrails_alignment =
      typed_sema_lowering_readiness.typed_performance_quality_guardrails_alignment;
  const bool typed_cross_lane_integration_alignment =
      typed_sema_lowering_readiness.typed_cross_lane_integration_alignment;
  const bool typed_docs_runbook_sync_alignment =
      typed_sema_lowering_readiness.typed_docs_runbook_sync_alignment;
  const bool typed_release_candidate_replay_dry_run_alignment =
      typed_sema_lowering_readiness.typed_release_candidate_replay_dry_run_alignment;
  const bool typed_advanced_core_shard1_alignment =
      typed_sema_lowering_readiness.typed_advanced_core_shard1_alignment;
  const bool typed_advanced_edge_compatibility_shard1_alignment =
      typed_sema_lowering_readiness.typed_advanced_edge_compatibility_shard1_alignment;
  const bool typed_advanced_diagnostics_shard1_alignment =
      typed_sema_lowering_readiness.typed_advanced_diagnostics_shard1_alignment;
  const bool typed_advanced_conformance_shard1_alignment =
      typed_sema_lowering_readiness.typed_advanced_conformance_shard1_alignment;
  const bool typed_advanced_integration_shard1_alignment =
      typed_sema_lowering_readiness.typed_advanced_integration_shard1_alignment;
  const bool typed_advanced_performance_shard1_alignment =
      typed_sema_lowering_readiness.typed_advanced_performance_shard1_alignment;
  const bool typed_advanced_core_shard2_alignment =
      typed_sema_lowering_readiness.typed_advanced_core_shard2_alignment;
  const bool typed_advanced_edge_compatibility_shard2_alignment =
      typed_sema_lowering_readiness.typed_advanced_edge_compatibility_shard2_alignment;
  const bool typed_advanced_diagnostics_shard2_alignment =
      typed_sema_lowering_readiness.typed_advanced_diagnostics_shard2_alignment;
  const bool typed_advanced_conformance_shard2_alignment =
      typed_sema_lowering_readiness.typed_advanced_conformance_shard2_alignment;
  const bool typed_advanced_integration_shard2_alignment =
      typed_sema_lowering_readiness.typed_advanced_integration_shard2_alignment;
  const bool typed_integration_closeout_signoff_alignment =
      typed_sema_lowering_readiness.typed_integration_closeout_signoff_alignment;
  const bool typed_core_feature_ready =
      typed_sema_lowering_readiness.typed_core_feature_ready;
  const bool sema_handoff_ready =
      typed_sema_lowering_readiness.sema_handoff_ready;
  const bool semantic_handoff_deterministic =
      typed_sema_lowering_readiness.semantic_handoff_deterministic;
  const Objc3ParseLoweringConformancePerformanceReadinessRecord
      conformance_performance_readiness =
          ApplyObjc3ParseLoweringConformancePerformanceReadiness(
              surface,
              toolchain_runtime_ga_operations_recovery_determinism_consistent,
              toolchain_runtime_ga_operations_recovery_determinism_ready,
              parse_snapshot_replay_ready,
              sema_handoff_ready,
              semantic_handoff_deterministic,
              typed_core_feature_ready);
  const Objc3ToolchainRuntimeGaOperationsCloseoutReadinessRecord
      toolchain_runtime_ga_operations_closeout_readiness =
          ApplyObjc3ToolchainRuntimeGaOperationsCloseoutReadiness(
              surface,
              diagnostics_clear,
              sema_handoff_ready,
              conformance_performance_readiness
                  .toolchain_runtime_ga_operations_cross_lane_integration_consistent,
              conformance_performance_readiness
                  .toolchain_runtime_ga_operations_cross_lane_integration_ready);
  surface.ready_for_lowering = diagnostics_clear &&
                               parse_snapshot_replay_ready &&
                               sema_handoff_ready &&
                               surface.lowering_boundary_ready &&
                               conformance_performance_readiness
                                   .parse_lowering_conformance_matrix_ready &&
                               surface.parser_diagnostic_grammar_hooks_conformance_corpus_ready &&
                               conformance_performance_readiness
                                   .parse_lowering_conformance_corpus_ready &&
                               conformance_performance_readiness
                                   .parse_lowering_performance_quality_guardrails_ready_gate &&
                               surface.long_tail_grammar_gate_signoff_ready;

  if (surface.ready_for_lowering || !surface.failure_reason.empty()) {
    return surface;
  }

  if (surface.lexer_diagnostic_count != 0) {
    surface.failure_reason = "lexer diagnostics present";
    return surface;
  }

  if (surface.parser_diagnostic_count != 0) {
    surface.failure_reason = "parser diagnostics present";
    return surface;
  }

  if (surface.semantic_diagnostic_count != 0) {
    surface.failure_reason = "semantic diagnostics present";
    return surface;
  }

  if (!surface.parser_contract_snapshot_present) {
    surface.failure_reason = "parser contract snapshot missing";
    return surface;
  }

  if (!surface.parser_contract_deterministic) {
    surface.failure_reason = "parser handoff is not deterministic";
    return surface;
  }

  if (!surface.parser_recovery_replay_ready) {
    surface.failure_reason = "parser recovery handoff is not replay ready";
    return surface;
  }

  if (!surface.long_tail_grammar_core_feature_consistent) {
    surface.failure_reason = "long-tail grammar core feature is inconsistent";
    return surface;
  }

  if (!surface.long_tail_grammar_handoff_key_deterministic) {
    surface.failure_reason = "long-tail grammar handoff key is not deterministic";
    return surface;
  }

  if (!surface.long_tail_grammar_expansion_accounting_consistent) {
    surface.failure_reason = "long-tail grammar expansion accounting is inconsistent";
    return surface;
  }

  if (!surface.parse_artifact_handoff_consistent) {
    surface.failure_reason = "parse artifact handoff is inconsistent";
    return surface;
  }

  if (!surface.parser_diagnostic_surface_consistent) {
    surface.failure_reason = "parser diagnostics surface is inconsistent";
    return surface;
  }

  if (!surface.parser_diagnostic_source_precision_scaffold_ready) {
    surface.failure_reason = "parser diagnostic source-precision scaffold is not ready";
    return surface;
  }

  if (!surface.parser_diagnostic_grammar_hooks_core_feature_ready) {
    surface.failure_reason = "parser diagnostic grammar hooks core feature is not ready";
    return surface;
  }

  if (!surface.parser_diagnostic_grammar_hooks_core_feature_expansion_accounting_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks core feature expansion accounting is inconsistent";
    return surface;
  }

  if (!surface.parser_diagnostic_grammar_hooks_core_feature_expansion_replay_keys_ready) {
    surface.failure_reason =
        "parser diagnostic grammar hooks core feature expansion replay keys are not ready";
    return surface;
  }

  if (!surface.parser_diagnostic_grammar_hooks_core_feature_expansion_ready) {
    surface.failure_reason = "parser diagnostic grammar hooks core feature expansion is not ready";
    return surface;
  }

  if (!surface.parser_diagnostic_code_surface_deterministic) {
    surface.failure_reason = "parser diagnostic code surface is not deterministic";
    return surface;
  }

  if (!surface.parse_artifact_handoff_deterministic) {
    surface.failure_reason = "parse artifact handoff is not deterministic";
    return surface;
  }

  if (!surface.parse_artifact_layout_fingerprint_consistent) {
    surface.failure_reason = "parse artifact layout fingerprint is inconsistent";
    return surface;
  }

  if (!surface.parse_artifact_fingerprint_consistent) {
    surface.failure_reason = "parse artifact fingerprint is inconsistent";
    return surface;
  }

  if (!surface.compatibility_handoff_consistent) {
    surface.failure_reason = "compatibility handoff is inconsistent";
    return surface;
  }

  if (!surface.long_tail_grammar_compatibility_handoff_ready) {
    surface.failure_reason = "long-tail grammar compatibility handoff is not ready";
    return surface;
  }

  if (!surface.parse_artifact_replay_key_deterministic) {
    surface.failure_reason = "parse artifact replay key is not deterministic";
    return surface;
  }

  if (!surface.long_tail_grammar_replay_keys_ready) {
    surface.failure_reason = "long-tail grammar replay keys are not ready";
    return surface;
  }

  if (!surface.long_tail_grammar_expansion_ready) {
    surface.failure_reason = "long-tail grammar core feature expansion is not ready";
    return surface;
  }

  if (!surface.parse_artifact_diagnostics_hardening_consistent) {
    surface.failure_reason = "parse artifact diagnostics hardening is inconsistent";
    return surface;
  }

  if (!surface.parser_token_count_budget_consistent) {
    surface.failure_reason = "parser token count budget is inconsistent";
    return surface;
  }

  if (!surface.language_version_pragma_coordinate_order_consistent) {
    surface.failure_reason = "language-version pragma coordinate order is inconsistent";
    return surface;
  }

  if (!surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_consistent) {
    surface.failure_reason = "parser diagnostic grammar hooks edge-case compatibility is inconsistent";
    return surface;
  }

  if (!surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_ready) {
    surface.failure_reason = "parser diagnostic grammar hooks edge-case compatibility is not ready";
    return surface;
  }

  if (!surface.parser_diagnostic_grammar_hooks_edge_case_expansion_consistent) {
    surface.failure_reason = "parser diagnostic grammar hooks edge-case expansion is inconsistent";
    return surface;
  }

  if (!surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready) {
    surface.failure_reason = "parser diagnostic grammar hooks edge-case robustness is not ready";
    return surface;
  }

  if (!surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent) {
    surface.failure_reason = "parser diagnostic grammar hooks diagnostics hardening is inconsistent";
    return surface;
  }

  if (!surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready) {
    surface.failure_reason = "parser diagnostic grammar hooks diagnostics hardening is not ready";
    return surface;
  }

  if (!surface.parser_diagnostic_grammar_hooks_recovery_determinism_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks recovery/determinism hardening is inconsistent";
    return surface;
  }

  if (!surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready) {
    surface.failure_reason =
        "parser diagnostic grammar hooks recovery/determinism hardening is not ready";
    return surface;
  }

  if (!surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks conformance matrix is inconsistent";
    return surface;
  }

  if (!surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready) {
    surface.failure_reason =
        "parser diagnostic grammar hooks conformance matrix is not ready";
    return surface;
  }

  if (!surface.parser_diagnostic_grammar_hooks_conformance_corpus_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks conformance corpus is inconsistent";
    return surface;
  }

  if (!surface.parser_diagnostic_grammar_hooks_conformance_corpus_ready) {
    surface.failure_reason =
        "parser diagnostic grammar hooks conformance corpus is not ready";
    return surface;
  }

  if (!surface.parse_artifact_edge_case_robustness_consistent) {
    surface.failure_reason = "parse artifact edge-case robustness is inconsistent";
    return surface;
  }

  if (!surface.long_tail_grammar_edge_case_compatibility_consistent) {
    surface.failure_reason = "long-tail grammar edge-case compatibility is inconsistent";
    return surface;
  }

  if (!surface.long_tail_grammar_edge_case_compatibility_ready) {
    surface.failure_reason = "long-tail grammar edge-case compatibility is not ready";
    return surface;
  }

  if (!surface.long_tail_grammar_edge_case_expansion_consistent) {
    surface.failure_reason = "long-tail grammar edge-case expansion is inconsistent";
    return surface;
  }

  if (!surface.long_tail_grammar_edge_case_robustness_ready) {
    surface.failure_reason = "long-tail grammar edge-case robustness is not ready";
    return surface;
  }

  if (!surface.long_tail_grammar_diagnostics_hardening_consistent) {
    surface.failure_reason = "long-tail grammar diagnostics hardening is inconsistent";
    return surface;
  }

  if (!surface.long_tail_grammar_diagnostics_hardening_ready) {
    surface.failure_reason = "long-tail grammar diagnostics hardening is not ready";
    return surface;
  }

  if (
      !IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningConsistent(
          surface.parser_recovery_replay_ready,
          surface.parse_artifact_replay_key_deterministic,
          surface.long_tail_grammar_replay_keys_ready,
          surface.long_tail_grammar_diagnostics_hardening_ready,
          surface.parse_recovery_determinism_hardening_consistent,
          surface.parse_artifact_handoff_key,
          surface.parse_artifact_replay_key,
          surface.parse_artifact_diagnostics_hardening_key,
          surface.parse_artifact_edge_robustness_key,
          surface.long_tail_grammar_handoff_key,
          surface.long_tail_grammar_diagnostics_hardening_key,
          surface.parse_recovery_determinism_hardening_key)) {
    surface.failure_reason = "toolchain/runtime GA operations recovery/determinism hardening is inconsistent";
    return surface;
  }

  if (
      !IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningReady(
          IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningConsistent(
              surface.parser_recovery_replay_ready,
              surface.parse_artifact_replay_key_deterministic,
              surface.long_tail_grammar_replay_keys_ready,
              surface.long_tail_grammar_diagnostics_hardening_ready,
              surface.parse_recovery_determinism_hardening_consistent,
              surface.parse_artifact_handoff_key,
              surface.parse_artifact_replay_key,
              surface.parse_artifact_diagnostics_hardening_key,
              surface.parse_artifact_edge_robustness_key,
              surface.long_tail_grammar_handoff_key,
              surface.long_tail_grammar_diagnostics_hardening_key,
              surface.parse_recovery_determinism_hardening_key),
          surface.long_tail_grammar_recovery_determinism_consistent,
          surface.long_tail_grammar_recovery_determinism_ready,
          surface.long_tail_grammar_recovery_determinism_key)) {
    surface.failure_reason = "toolchain/runtime GA operations recovery/determinism hardening is not ready";
    return surface;
  }

  if (!surface.long_tail_grammar_recovery_determinism_consistent) {
    surface.failure_reason = "long-tail grammar recovery/determinism hardening is inconsistent";
    return surface;
  }

  if (!surface.long_tail_grammar_recovery_determinism_ready) {
    surface.failure_reason = "long-tail grammar recovery/determinism hardening is not ready";
    return surface;
  }

  if (!conformance_performance_readiness
           .toolchain_runtime_ga_operations_conformance_matrix_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations conformance matrix is inconsistent";
    return surface;
  }

  if (!conformance_performance_readiness
           .toolchain_runtime_ga_operations_conformance_matrix_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations conformance matrix is not ready";
    return surface;
  }

  if (!conformance_performance_readiness
           .toolchain_runtime_ga_operations_conformance_corpus_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations conformance corpus is inconsistent";
    return surface;
  }

  if (!conformance_performance_readiness
           .toolchain_runtime_ga_operations_conformance_corpus_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations conformance corpus is not ready";
    return surface;
  }

  if (!surface.long_tail_grammar_conformance_matrix_consistent) {
    surface.failure_reason = "long-tail grammar conformance matrix is inconsistent";
    return surface;
  }

  if (!surface.long_tail_grammar_conformance_matrix_ready) {
    surface.failure_reason = "long-tail grammar conformance matrix is not ready";
    return surface;
  }

  if (!surface.parse_recovery_determinism_hardening_consistent) {
    surface.failure_reason = "parse recovery/determinism hardening is inconsistent";
    return surface;
  }

  if (!surface.semantic_integration_surface_built) {
    surface.failure_reason = "semantic integration surface not built";
    return surface;
  }

  if (!surface.semantic_diagnostics_deterministic) {
    surface.failure_reason = "semantic diagnostics handoff is not deterministic";
    return surface;
  }

  if (!surface.semantic_type_metadata_deterministic) {
    surface.failure_reason = "semantic type metadata handoff is not deterministic";
    return surface;
  }

  if (!surface.protocol_category_deterministic) {
    surface.failure_reason = "protocol/category handoff is not deterministic";
    return surface;
  }

  if (!surface.class_protocol_category_linking_deterministic) {
    surface.failure_reason = "class/protocol/category linking handoff is not deterministic";
    return surface;
  }

  if (!surface.selector_normalization_deterministic) {
    surface.failure_reason = "selector normalization handoff is not deterministic";
    return surface;
  }

  if (!surface.property_attribute_deterministic) {
    surface.failure_reason = "property attribute handoff is not deterministic";
    return surface;
  }

  if (!surface.symbol_graph_deterministic) {
    surface.failure_reason = "symbol graph handoff is not deterministic";
    return surface;
  }

  if (!surface.scope_resolution_deterministic) {
    surface.failure_reason = "scope resolution handoff is not deterministic";
    return surface;
  }

  if (!surface.object_pointer_type_handoff_deterministic) {
    surface.failure_reason = "object pointer/nullability handoff is not deterministic";
    return surface;
  }

  if (!surface.typed_handoff_key_deterministic) {
    surface.failure_reason = "typed sema-to-lowering handoff key is not deterministic";
    return surface;
  }

  if (!surface.typed_sema_core_feature_consistent) {
    surface.failure_reason = "typed sema-to-lowering core feature contract is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_core_feature_expansion_consistent) {
    surface.failure_reason = "typed sema-to-lowering core feature expansion is inconsistent";
    return surface;
  }

  if (surface.typed_sema_core_feature_expansion_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering core feature expansion key is empty";
    return surface;
  }

  if (!surface.typed_sema_edge_case_compatibility_consistent) {
    surface.failure_reason = "typed sema-to-lowering edge-case compatibility is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_edge_case_compatibility_ready) {
    surface.failure_reason = "typed sema-to-lowering edge-case compatibility is not ready";
    return surface;
  }

  if (surface.typed_sema_edge_case_compatibility_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering edge-case compatibility key is empty";
    return surface;
  }

  if (!surface.typed_sema_edge_case_expansion_consistent) {
    surface.failure_reason = "typed sema-to-lowering edge-case expansion is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_edge_case_robustness_ready) {
    surface.failure_reason = "typed sema-to-lowering edge-case robustness is not ready";
    return surface;
  }

  if (surface.typed_sema_edge_case_robustness_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering edge-case robustness key is empty";
    return surface;
  }

  if (!surface.typed_sema_diagnostics_hardening_consistent) {
    surface.failure_reason = "typed sema-to-lowering diagnostics hardening is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_diagnostics_hardening_ready) {
    surface.failure_reason = "typed sema-to-lowering diagnostics hardening is not ready";
    return surface;
  }

  if (surface.typed_sema_diagnostics_hardening_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering diagnostics hardening key is empty";
    return surface;
  }

  if (!surface.typed_sema_recovery_determinism_consistent) {
    surface.failure_reason = "typed sema-to-lowering recovery/determinism is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_recovery_determinism_ready) {
    surface.failure_reason = "typed sema-to-lowering recovery/determinism is not ready";
    return surface;
  }

  if (surface.typed_sema_recovery_determinism_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering recovery/determinism key is empty";
    return surface;
  }

  if (!surface.typed_sema_conformance_matrix_consistent) {
    surface.failure_reason = "typed sema-to-lowering conformance matrix is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_conformance_matrix_ready) {
    surface.failure_reason = "typed sema-to-lowering conformance matrix is not ready";
    return surface;
  }

  if (surface.typed_sema_conformance_matrix_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering conformance matrix key is empty";
    return surface;
  }

  if (!surface.typed_sema_conformance_corpus_consistent) {
    surface.failure_reason = "typed sema-to-lowering conformance corpus is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_conformance_corpus_ready) {
    surface.failure_reason = "typed sema-to-lowering conformance corpus is not ready";
    return surface;
  }

  if (surface.typed_sema_conformance_corpus_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering conformance corpus key is empty";
    return surface;
  }

  if (!surface.typed_sema_performance_quality_guardrails_consistent) {
    surface.failure_reason = "typed sema-to-lowering performance/quality guardrails are inconsistent";
    return surface;
  }

  if (!surface.typed_sema_performance_quality_guardrails_ready) {
    surface.failure_reason = "typed sema-to-lowering performance/quality guardrails are not ready";
    return surface;
  }

  if (surface.typed_sema_performance_quality_guardrails_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering performance/quality guardrails key is empty";
    return surface;
  }

  if (!surface.typed_sema_cross_lane_integration_consistent) {
    surface.failure_reason = "typed sema-to-lowering cross-lane integration is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_cross_lane_integration_ready) {
    surface.failure_reason = "typed sema-to-lowering cross-lane integration is not ready";
    return surface;
  }

  if (surface.typed_sema_cross_lane_integration_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering cross-lane integration key is empty";
    return surface;
  }

  if (!surface.typed_sema_docs_runbook_sync_consistent) {
    surface.failure_reason = "typed sema-to-lowering docs/runbook synchronization is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_docs_runbook_sync_ready) {
    surface.failure_reason = "typed sema-to-lowering docs/runbook synchronization is not ready";
    return surface;
  }

  if (surface.typed_sema_docs_runbook_sync_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering docs/runbook synchronization key is empty";
    return surface;
  }

  if (!surface.typed_sema_release_candidate_replay_dry_run_consistent) {
    surface.failure_reason =
        "typed sema-to-lowering release-candidate replay dry-run is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_release_candidate_replay_dry_run_ready) {
    surface.failure_reason =
        "typed sema-to-lowering release-candidate replay dry-run is not ready";
    return surface;
  }

  if (surface.typed_sema_release_candidate_replay_dry_run_key.empty()) {
    surface.failure_reason =
        "typed sema-to-lowering release-candidate replay dry-run key is empty";
    return surface;
  }

  if (!surface.typed_sema_advanced_core_shard1_consistent) {
    surface.failure_reason = "typed sema-to-lowering advanced core shard 1 is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_advanced_core_shard1_ready) {
    surface.failure_reason = "typed sema-to-lowering advanced core shard 1 is not ready";
    return surface;
  }

  if (surface.typed_sema_advanced_core_shard1_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering advanced core shard 1 key is empty";
    return surface;
  }

  if (!surface.typed_sema_advanced_edge_compatibility_shard1_consistent) {
    surface.failure_reason =
        "typed sema-to-lowering advanced edge compatibility shard 1 is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_advanced_edge_compatibility_shard1_ready) {
    surface.failure_reason =
        "typed sema-to-lowering advanced edge compatibility shard 1 is not ready";
    return surface;
  }

  if (surface.typed_sema_advanced_edge_compatibility_shard1_key.empty()) {
    surface.failure_reason =
        "typed sema-to-lowering advanced edge compatibility shard 1 key is empty";
    return surface;
  }

  if (!surface.typed_sema_advanced_diagnostics_shard1_consistent) {
    surface.failure_reason = "typed sema-to-lowering advanced diagnostics shard 1 is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_advanced_diagnostics_shard1_ready) {
    surface.failure_reason = "typed sema-to-lowering advanced diagnostics shard 1 is not ready";
    return surface;
  }

  if (surface.typed_sema_advanced_diagnostics_shard1_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering advanced diagnostics shard 1 key is empty";
    return surface;
  }

  if (!surface.typed_sema_advanced_conformance_shard1_consistent) {
    surface.failure_reason = "typed sema-to-lowering advanced conformance shard 1 is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_advanced_conformance_shard1_ready) {
    surface.failure_reason = "typed sema-to-lowering advanced conformance shard 1 is not ready";
    return surface;
  }

  if (surface.typed_sema_advanced_conformance_shard1_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering advanced conformance shard 1 key is empty";
    return surface;
  }

  if (!surface.typed_sema_advanced_integration_shard1_consistent) {
    surface.failure_reason = "typed sema-to-lowering advanced integration shard 1 is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_advanced_integration_shard1_ready) {
    surface.failure_reason = "typed sema-to-lowering advanced integration shard 1 is not ready";
    return surface;
  }

  if (surface.typed_sema_advanced_integration_shard1_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering advanced integration shard 1 key is empty";
    return surface;
  }

  if (!surface.typed_sema_advanced_performance_shard1_consistent) {
    surface.failure_reason = "typed sema-to-lowering advanced performance shard 1 is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_advanced_performance_shard1_ready) {
    surface.failure_reason = "typed sema-to-lowering advanced performance shard 1 is not ready";
    return surface;
  }

  if (surface.typed_sema_advanced_performance_shard1_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering advanced performance shard 1 key is empty";
    return surface;
  }

  if (!surface.typed_sema_advanced_core_shard2_consistent) {
    surface.failure_reason = "typed sema-to-lowering advanced core shard 2 is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_advanced_core_shard2_ready) {
    surface.failure_reason = "typed sema-to-lowering advanced core shard 2 is not ready";
    return surface;
  }

  if (surface.typed_sema_advanced_core_shard2_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering advanced core shard 2 key is empty";
    return surface;
  }

  if (!surface.typed_sema_advanced_edge_compatibility_shard2_consistent) {
    surface.failure_reason =
        "typed sema-to-lowering advanced edge compatibility shard 2 is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_advanced_edge_compatibility_shard2_ready) {
    surface.failure_reason =
        "typed sema-to-lowering advanced edge compatibility shard 2 is not ready";
    return surface;
  }

  if (surface.typed_sema_advanced_edge_compatibility_shard2_key.empty()) {
    surface.failure_reason =
        "typed sema-to-lowering advanced edge compatibility shard 2 key is empty";
    return surface;
  }

  if (!surface.typed_sema_advanced_diagnostics_shard2_consistent) {
    surface.failure_reason = "typed sema-to-lowering advanced diagnostics shard 2 is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_advanced_diagnostics_shard2_ready) {
    surface.failure_reason = "typed sema-to-lowering advanced diagnostics shard 2 is not ready";
    return surface;
  }

  if (surface.typed_sema_advanced_diagnostics_shard2_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering advanced diagnostics shard 2 key is empty";
    return surface;
  }

  if (!surface.typed_sema_advanced_conformance_shard2_consistent) {
    surface.failure_reason = "typed sema-to-lowering advanced conformance shard 2 is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_advanced_conformance_shard2_ready) {
    surface.failure_reason = "typed sema-to-lowering advanced conformance shard 2 is not ready";
    return surface;
  }

  if (surface.typed_sema_advanced_conformance_shard2_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering advanced conformance shard 2 key is empty";
    return surface;
  }

  if (!surface.typed_sema_advanced_integration_shard2_consistent) {
    surface.failure_reason = "typed sema-to-lowering advanced integration shard 2 is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_advanced_integration_shard2_ready) {
    surface.failure_reason = "typed sema-to-lowering advanced integration shard 2 is not ready";
    return surface;
  }

  if (surface.typed_sema_advanced_integration_shard2_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering advanced integration shard 2 key is empty";
    return surface;
  }

  if (!surface.typed_sema_integration_closeout_signoff_consistent) {
    surface.failure_reason = "typed sema-to-lowering integration closeout/sign-off is inconsistent";
    return surface;
  }

  if (!surface.typed_sema_integration_closeout_signoff_ready) {
    surface.failure_reason = "typed sema-to-lowering integration closeout/sign-off is not ready";
    return surface;
  }

  if (surface.typed_sema_integration_closeout_signoff_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering integration closeout/sign-off key is empty";
    return surface;
  }

  if (!typed_edge_case_compatibility_alignment) {
    surface.failure_reason = "typed sema-to-lowering edge-case compatibility drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_edge_case_robustness_alignment) {
    surface.failure_reason = "typed sema-to-lowering edge-case robustness drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_diagnostics_hardening_alignment) {
    surface.failure_reason = "typed sema-to-lowering diagnostics hardening drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_recovery_determinism_alignment) {
    surface.failure_reason = "typed sema-to-lowering recovery/determinism drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_conformance_matrix_alignment) {
    surface.failure_reason = "typed sema-to-lowering conformance matrix drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_conformance_corpus_alignment) {
    surface.failure_reason = "typed sema-to-lowering conformance corpus drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_performance_quality_guardrails_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering performance/quality guardrails drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_cross_lane_integration_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering cross-lane integration drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_docs_runbook_sync_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering docs/runbook synchronization drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_release_candidate_replay_dry_run_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering release-candidate replay dry-run drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_advanced_core_shard1_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering advanced core shard 1 drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_advanced_edge_compatibility_shard1_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering advanced edge compatibility shard 1 drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_advanced_diagnostics_shard1_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering advanced diagnostics shard 1 drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_advanced_conformance_shard1_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering advanced conformance shard 1 drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_advanced_integration_shard1_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering advanced integration shard 1 drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_advanced_performance_shard1_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering advanced performance shard 1 drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_advanced_core_shard2_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering advanced core shard 2 drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_advanced_edge_compatibility_shard2_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering advanced edge compatibility shard 2 drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_advanced_diagnostics_shard2_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering advanced diagnostics shard 2 drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_advanced_conformance_shard2_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering advanced conformance shard 2 drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_advanced_integration_shard2_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering advanced integration shard 2 drifted from parse/lowering readiness";
    return surface;
  }

  if (!typed_integration_closeout_signoff_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering integration closeout/sign-off drifted from parse/lowering readiness";
    return surface;
  }

  if (!surface.lowering_boundary_ready) {
    surface.failure_reason = "lowering boundary is not ready";
    return surface;
  }

  if (!surface.parse_lowering_conformance_matrix_consistent) {
    surface.failure_reason = "parse-lowering conformance matrix is inconsistent";
    return surface;
  }

  if (!surface.parse_lowering_conformance_corpus_consistent) {
    surface.failure_reason = "parse-lowering conformance corpus is inconsistent";
    return surface;
  }

  if (!conformance_performance_readiness
           .toolchain_runtime_ga_operations_performance_quality_guardrails_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations performance quality guardrails are inconsistent";
    return surface;
  }

  if (!conformance_performance_readiness
           .toolchain_runtime_ga_operations_performance_quality_guardrails_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations performance quality guardrails are not ready";
    return surface;
  }

  if (!surface.parse_lowering_performance_quality_guardrails_consistent) {
    surface.failure_reason = "parse-lowering performance/quality guardrails are inconsistent";
    return surface;
  }

  if (!conformance_performance_readiness
           .toolchain_runtime_ga_operations_cross_lane_integration_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations cross-lane integration is inconsistent";
    return surface;
  }

  if (!conformance_performance_readiness
           .toolchain_runtime_ga_operations_cross_lane_integration_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations cross-lane integration is not ready";
    return surface;
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness
           .docs_runbook_sync_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations docs and runbook synchronization is inconsistent";
    return surface;
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness
           .docs_runbook_sync_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations docs and runbook synchronization is not ready";
    return surface;
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness
           .advanced_core_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced core workpack is inconsistent";
    return surface;
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness
           .advanced_core_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced core workpack is not ready";
    return surface;
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness
           .advanced_edge_compatibility_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced edge compatibility workpack is inconsistent";
    return surface;
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness
           .advanced_edge_compatibility_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced edge compatibility workpack is not ready";
    return surface;
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness
           .advanced_diagnostics_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced diagnostics workpack is inconsistent";
    return surface;
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness
           .advanced_diagnostics_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced diagnostics workpack is not ready";
    return surface;
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness
           .advanced_conformance_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced conformance workpack is inconsistent";
    return surface;
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness
           .advanced_conformance_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced conformance workpack is not ready";
    return surface;
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness
           .advanced_integration_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced integration workpack is inconsistent";
    return surface;
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness
           .advanced_integration_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced integration workpack is not ready";
    return surface;
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness
           .advanced_performance_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced performance workpack is inconsistent";
    return surface;
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness
           .advanced_performance_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced performance workpack is not ready";
    return surface;
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness
           .advanced_core_shard2_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced core workpack (shard 2) is inconsistent";
    return surface;
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness
           .advanced_core_shard2_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced core workpack (shard 2) is not ready";
    return surface;
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness
           .integration_closeout_signoff_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations integration closeout and sign-off is inconsistent";
    return surface;
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness
           .integration_closeout_signoff_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations integration closeout and sign-off is not ready";
    return surface;
  }

  if (!surface.long_tail_grammar_integration_closeout_consistent) {
    surface.failure_reason = "long-tail grammar integration closeout is inconsistent";
    return surface;
  }

  if (!surface.long_tail_grammar_gate_signoff_ready) {
    surface.failure_reason = "long-tail grammar gate sign-off is not ready";
    return surface;
  }

  surface.failure_reason = "parse-lowering readiness failed";
  return surface;
}

inline bool IsObjc3ParseLoweringReadinessSurfaceReady(const Objc3ParseLoweringReadinessSurface &surface,
                                                      std::string &reason) {
  if (surface.ready_for_lowering) {
    reason.clear();
    return true;
  }
  reason = surface.failure_reason.empty() ? "parse-lowering readiness surface not ready" : surface.failure_reason;
  return false;
}
