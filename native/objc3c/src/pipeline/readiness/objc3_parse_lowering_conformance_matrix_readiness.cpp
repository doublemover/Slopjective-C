#include "pipeline/readiness/objc3_parse_lowering_conformance_performance_readiness_private.h"

#include <string>

#include "pipeline/readiness/objc3_long_tail_grammar_readiness_keys.h"
#include "pipeline/readiness/objc3_parse_lowering_conformance_keys.h"
#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_readiness_keys.h"

void ApplyObjc3ParseLoweringConformanceMatrixReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    Objc3ParseLoweringConformancePerformanceReadinessRecord &record,
    bool toolchain_runtime_ga_operations_recovery_determinism_consistent,
    bool toolchain_runtime_ga_operations_recovery_determinism_ready,
    bool sema_handoff_ready,
    bool semantic_handoff_deterministic,
    bool typed_core_feature_ready) {
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
}
