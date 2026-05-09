#pragma once

#include <string>

#include "pipeline/parse_lowering_diagnostic_keys.h"
#include "pipeline/objc3_frontend_types.h"
#include "pipeline/readiness/objc3_long_tail_grammar_readiness_keys.h"
#include "pipeline/readiness/objc3_parse_lowering_conformance_keys.h"
#include "pipeline/readiness/objc3_parse_lowering_diagnostic_grammar_hooks_readiness.h"
#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_readiness_keys.h"

struct Objc3ParseLoweringRecoveryDeterminismReadinessRecord {
  bool toolchain_runtime_ga_operations_recovery_determinism_consistent = false;
  bool toolchain_runtime_ga_operations_recovery_determinism_ready = false;
};

inline Objc3ParseLoweringRecoveryDeterminismReadinessRecord
ApplyObjc3ParseLoweringRecoveryDeterminismReadiness(
    Objc3ParseLoweringReadinessSurface &surface) {
  Objc3ParseLoweringRecoveryDeterminismReadinessRecord record;

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
  ApplyObjc3ParseLoweringDiagnosticGrammarHooksConformanceMatrixReadiness(surface);
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
  record.toolchain_runtime_ga_operations_recovery_determinism_consistent =
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
  record.toolchain_runtime_ga_operations_recovery_determinism_ready =
      IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningReady(
          record.toolchain_runtime_ga_operations_recovery_determinism_consistent,
          surface.long_tail_grammar_recovery_determinism_consistent,
          surface.long_tail_grammar_recovery_determinism_ready,
          surface.long_tail_grammar_recovery_determinism_key);
  surface.parse_recovery_determinism_hardening_consistent &=
      record.toolchain_runtime_ga_operations_recovery_determinism_consistent;
  surface.long_tail_grammar_recovery_determinism_consistent =
      surface.long_tail_grammar_recovery_determinism_consistent &&
      record.toolchain_runtime_ga_operations_recovery_determinism_consistent;
  surface.long_tail_grammar_recovery_determinism_ready =
      surface.long_tail_grammar_recovery_determinism_ready &&
      record.toolchain_runtime_ga_operations_recovery_determinism_ready;
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
          record.toolchain_runtime_ga_operations_recovery_determinism_consistent,
          record.toolchain_runtime_ga_operations_recovery_determinism_ready);
  surface.parse_recovery_determinism_hardening_key +=
      ";toolchain_runtime_ga_operations_recovery_determinism_key=" +
      toolchain_runtime_ga_operations_recovery_determinism_key;
  surface.long_tail_grammar_recovery_determinism_key +=
      ";toolchain_runtime_ga_operations_recovery_determinism_key=" +
      toolchain_runtime_ga_operations_recovery_determinism_key;

  return record;
}
