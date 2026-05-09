#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"
#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_advanced_readiness_keys.h"
#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_readiness_keys.h"

struct Objc3ToolchainRuntimeGaOperationsCloseoutReadinessRecord {
  bool docs_runbook_sync_consistent = false;
  bool docs_runbook_sync_ready = false;
  bool advanced_core_consistent = false;
  bool advanced_core_ready = false;
  bool advanced_edge_compatibility_consistent = false;
  bool advanced_edge_compatibility_ready = false;
  bool advanced_diagnostics_consistent = false;
  bool advanced_diagnostics_ready = false;
  bool advanced_conformance_consistent = false;
  bool advanced_conformance_ready = false;
  bool advanced_integration_consistent = false;
  bool advanced_integration_ready = false;
  bool advanced_performance_consistent = false;
  bool advanced_performance_ready = false;
  bool advanced_core_shard2_consistent = false;
  bool advanced_core_shard2_ready = false;
  bool integration_closeout_signoff_consistent = false;
  bool integration_closeout_signoff_ready = false;
};

inline Objc3ToolchainRuntimeGaOperationsCloseoutReadinessRecord
ApplyObjc3ToolchainRuntimeGaOperationsCloseoutReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    bool diagnostics_clear,
    bool sema_handoff_ready,
    bool toolchain_runtime_ga_operations_cross_lane_integration_consistent,
    bool toolchain_runtime_ga_operations_cross_lane_integration_ready) {
  Objc3ToolchainRuntimeGaOperationsCloseoutReadinessRecord record;

  surface.long_tail_grammar_integration_closeout_consistent =
      surface.long_tail_grammar_conformance_matrix_ready &&
      surface.parse_lowering_conformance_corpus_consistent &&
      surface.parse_lowering_performance_quality_guardrails_consistent &&
      toolchain_runtime_ga_operations_cross_lane_integration_consistent &&
      surface.long_tail_grammar_recovery_determinism_ready &&
      !surface.long_tail_grammar_conformance_matrix_key.empty() &&
      !surface.parse_lowering_conformance_corpus_key.empty() &&
      !surface.parse_lowering_performance_quality_guardrails_key.empty();
  surface.long_tail_grammar_gate_signoff_ready =
      surface.long_tail_grammar_integration_closeout_consistent &&
      toolchain_runtime_ga_operations_cross_lane_integration_ready &&
      diagnostics_clear &&
      sema_handoff_ready &&
      surface.lowering_boundary_ready &&
      !surface.parse_artifact_replay_key.empty() &&
      !surface.lowering_boundary_replay_key.empty();
  surface.long_tail_grammar_integration_closeout_key =
      BuildObjc3LongTailGrammarIntegrationCloseoutKey(
          surface.long_tail_grammar_conformance_matrix_ready,
          surface.parse_lowering_conformance_corpus_consistent,
          surface.parse_lowering_performance_quality_guardrails_consistent,
          toolchain_runtime_ga_operations_cross_lane_integration_consistent,
          toolchain_runtime_ga_operations_cross_lane_integration_ready,
          surface.long_tail_grammar_recovery_determinism_ready,
          sema_handoff_ready,
          surface.lowering_boundary_ready,
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready);
  record.docs_runbook_sync_consistent =
      IsObjc3ToolchainRuntimeGaOperationsDocsRunbookSyncConsistent(
          toolchain_runtime_ga_operations_cross_lane_integration_consistent,
          toolchain_runtime_ga_operations_cross_lane_integration_ready,
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  record.docs_runbook_sync_ready =
      IsObjc3ToolchainRuntimeGaOperationsDocsRunbookSyncReady(
          record.docs_runbook_sync_consistent,
          surface.long_tail_grammar_integration_closeout_key);
  const std::string toolchain_runtime_ga_operations_docs_runbook_sync_key =
      BuildObjc3ToolchainRuntimeGaOperationsDocsRunbookSyncKey(
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          record.docs_runbook_sync_consistent,
          record.docs_runbook_sync_ready);
  surface.toolchain_runtime_ga_operations_docs_runbook_sync_consistent =
      record.docs_runbook_sync_consistent;
  surface.toolchain_runtime_ga_operations_docs_runbook_sync_ready =
      record.docs_runbook_sync_ready;
  surface.toolchain_runtime_ga_operations_docs_runbook_sync_key =
      toolchain_runtime_ga_operations_docs_runbook_sync_key;
  surface.long_tail_grammar_integration_closeout_consistent =
      surface.long_tail_grammar_integration_closeout_consistent &&
      record.docs_runbook_sync_consistent;
  surface.long_tail_grammar_gate_signoff_ready =
      surface.long_tail_grammar_gate_signoff_ready &&
      record.docs_runbook_sync_ready;
  surface.long_tail_grammar_integration_closeout_key +=
      ";toolchain_runtime_ga_operations_docs_runbook_sync_key=" +
      toolchain_runtime_ga_operations_docs_runbook_sync_key;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_docs_runbook_sync_key=" +
      toolchain_runtime_ga_operations_docs_runbook_sync_key;

  record.advanced_core_consistent =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreConsistent(
          record.docs_runbook_sync_consistent,
          record.docs_runbook_sync_ready,
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready,
          toolchain_runtime_ga_operations_docs_runbook_sync_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  record.advanced_core_ready =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreReady(
          record.advanced_core_consistent,
          toolchain_runtime_ga_operations_docs_runbook_sync_key,
          surface.long_tail_grammar_integration_closeout_key);
  const std::string toolchain_runtime_ga_operations_advanced_core_key =
      BuildObjc3ToolchainRuntimeGaOperationsAdvancedCoreKey(
          record.docs_runbook_sync_consistent,
          record.docs_runbook_sync_ready,
          toolchain_runtime_ga_operations_docs_runbook_sync_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          record.advanced_core_consistent,
          record.advanced_core_ready);
  surface.toolchain_runtime_ga_operations_advanced_core_consistent =
      record.advanced_core_consistent;
  surface.toolchain_runtime_ga_operations_advanced_core_ready =
      record.advanced_core_ready;
  surface.toolchain_runtime_ga_operations_advanced_core_key =
      toolchain_runtime_ga_operations_advanced_core_key;
  surface.long_tail_grammar_integration_closeout_consistent =
      surface.long_tail_grammar_integration_closeout_consistent &&
      record.advanced_core_consistent;
  surface.long_tail_grammar_gate_signoff_ready =
      surface.long_tail_grammar_gate_signoff_ready &&
      record.advanced_core_ready;
  surface.long_tail_grammar_integration_closeout_key +=
      ";toolchain_runtime_ga_operations_advanced_core_key=" +
      toolchain_runtime_ga_operations_advanced_core_key;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_advanced_core_key=" +
      toolchain_runtime_ga_operations_advanced_core_key;

  record.advanced_edge_compatibility_consistent =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityConsistent(
          record.advanced_core_consistent,
          record.advanced_core_ready,
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready,
          toolchain_runtime_ga_operations_advanced_core_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  record.advanced_edge_compatibility_ready =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityReady(
          record.advanced_edge_compatibility_consistent,
          toolchain_runtime_ga_operations_advanced_core_key,
          surface.long_tail_grammar_integration_closeout_key);
  const std::string toolchain_runtime_ga_operations_advanced_edge_compatibility_key =
      BuildObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityKey(
          record.advanced_core_consistent,
          record.advanced_core_ready,
          toolchain_runtime_ga_operations_advanced_core_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          record.advanced_edge_compatibility_consistent,
          record.advanced_edge_compatibility_ready);
  surface.toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent =
      record.advanced_edge_compatibility_consistent;
  surface.toolchain_runtime_ga_operations_advanced_edge_compatibility_ready =
      record.advanced_edge_compatibility_ready;
  surface.toolchain_runtime_ga_operations_advanced_edge_compatibility_key =
      toolchain_runtime_ga_operations_advanced_edge_compatibility_key;
  surface.long_tail_grammar_integration_closeout_consistent =
      surface.long_tail_grammar_integration_closeout_consistent &&
      record.advanced_edge_compatibility_consistent;
  surface.long_tail_grammar_gate_signoff_ready =
      surface.long_tail_grammar_gate_signoff_ready &&
      record.advanced_edge_compatibility_ready;
  surface.long_tail_grammar_integration_closeout_key +=
      ";toolchain_runtime_ga_operations_advanced_edge_compatibility_key=" +
      toolchain_runtime_ga_operations_advanced_edge_compatibility_key;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_advanced_edge_compatibility_key=" +
      toolchain_runtime_ga_operations_advanced_edge_compatibility_key;

  record.advanced_diagnostics_consistent =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedDiagnosticsConsistent(
          record.advanced_edge_compatibility_consistent,
          record.advanced_edge_compatibility_ready,
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready,
          toolchain_runtime_ga_operations_advanced_edge_compatibility_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  record.advanced_diagnostics_ready =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedDiagnosticsReady(
          record.advanced_diagnostics_consistent,
          toolchain_runtime_ga_operations_advanced_edge_compatibility_key,
          surface.long_tail_grammar_integration_closeout_key);
  const std::string toolchain_runtime_ga_operations_advanced_diagnostics_key =
      BuildObjc3ToolchainRuntimeGaOperationsAdvancedDiagnosticsKey(
          record.advanced_edge_compatibility_consistent,
          record.advanced_edge_compatibility_ready,
          toolchain_runtime_ga_operations_advanced_edge_compatibility_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          record.advanced_diagnostics_consistent,
          record.advanced_diagnostics_ready);
  surface.toolchain_runtime_ga_operations_advanced_diagnostics_consistent =
      record.advanced_diagnostics_consistent;
  surface.toolchain_runtime_ga_operations_advanced_diagnostics_ready =
      record.advanced_diagnostics_ready;
  surface.toolchain_runtime_ga_operations_advanced_diagnostics_key =
      toolchain_runtime_ga_operations_advanced_diagnostics_key;
  surface.long_tail_grammar_integration_closeout_consistent =
      surface.long_tail_grammar_integration_closeout_consistent &&
      record.advanced_diagnostics_consistent;
  surface.long_tail_grammar_gate_signoff_ready =
      surface.long_tail_grammar_gate_signoff_ready &&
      record.advanced_diagnostics_ready;
  surface.long_tail_grammar_integration_closeout_key +=
      ";toolchain_runtime_ga_operations_advanced_diagnostics_key=" +
      toolchain_runtime_ga_operations_advanced_diagnostics_key;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_advanced_diagnostics_key=" +
      toolchain_runtime_ga_operations_advanced_diagnostics_key;

  record.advanced_conformance_consistent =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedConformanceConsistent(
          record.advanced_diagnostics_consistent,
          record.advanced_diagnostics_ready,
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready,
          toolchain_runtime_ga_operations_advanced_diagnostics_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  record.advanced_conformance_ready =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedConformanceReady(
          record.advanced_conformance_consistent,
          toolchain_runtime_ga_operations_advanced_diagnostics_key,
          surface.long_tail_grammar_integration_closeout_key);
  const std::string toolchain_runtime_ga_operations_advanced_conformance_key =
      BuildObjc3ToolchainRuntimeGaOperationsAdvancedConformanceKey(
          record.advanced_diagnostics_consistent,
          record.advanced_diagnostics_ready,
          toolchain_runtime_ga_operations_advanced_diagnostics_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          record.advanced_conformance_consistent,
          record.advanced_conformance_ready);
  surface.toolchain_runtime_ga_operations_advanced_conformance_consistent =
      record.advanced_conformance_consistent;
  surface.toolchain_runtime_ga_operations_advanced_conformance_ready =
      record.advanced_conformance_ready;
  surface.toolchain_runtime_ga_operations_advanced_conformance_key =
      toolchain_runtime_ga_operations_advanced_conformance_key;
  surface.long_tail_grammar_integration_closeout_consistent =
      surface.long_tail_grammar_integration_closeout_consistent &&
      record.advanced_conformance_consistent;
  surface.long_tail_grammar_gate_signoff_ready =
      surface.long_tail_grammar_gate_signoff_ready &&
      record.advanced_conformance_ready;
  surface.long_tail_grammar_integration_closeout_key +=
      ";toolchain_runtime_ga_operations_advanced_conformance_key=" +
      toolchain_runtime_ga_operations_advanced_conformance_key;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_advanced_conformance_key=" +
      toolchain_runtime_ga_operations_advanced_conformance_key;

  record.advanced_integration_consistent =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedIntegrationConsistent(
          record.advanced_conformance_consistent,
          record.advanced_conformance_ready,
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready,
          toolchain_runtime_ga_operations_advanced_conformance_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  record.advanced_integration_ready =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedIntegrationReady(
          record.advanced_integration_consistent,
          toolchain_runtime_ga_operations_advanced_conformance_key,
          surface.long_tail_grammar_integration_closeout_key);
  const std::string toolchain_runtime_ga_operations_advanced_integration_key =
      BuildObjc3ToolchainRuntimeGaOperationsAdvancedIntegrationKey(
          record.advanced_conformance_consistent,
          record.advanced_conformance_ready,
          toolchain_runtime_ga_operations_advanced_conformance_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          record.advanced_integration_consistent,
          record.advanced_integration_ready);
  surface.toolchain_runtime_ga_operations_advanced_integration_consistent =
      record.advanced_integration_consistent;
  surface.toolchain_runtime_ga_operations_advanced_integration_ready =
      record.advanced_integration_ready;
  surface.toolchain_runtime_ga_operations_advanced_integration_key =
      toolchain_runtime_ga_operations_advanced_integration_key;
  surface.long_tail_grammar_integration_closeout_consistent =
      surface.long_tail_grammar_integration_closeout_consistent &&
      record.advanced_integration_consistent;
  surface.long_tail_grammar_gate_signoff_ready =
      surface.long_tail_grammar_gate_signoff_ready &&
      record.advanced_integration_ready;
  surface.long_tail_grammar_integration_closeout_key +=
      ";toolchain_runtime_ga_operations_advanced_integration_key=" +
      toolchain_runtime_ga_operations_advanced_integration_key;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_advanced_integration_key=" +
      toolchain_runtime_ga_operations_advanced_integration_key;

  record.advanced_performance_consistent =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedPerformanceConsistent(
          record.advanced_integration_consistent,
          record.advanced_integration_ready,
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready,
          toolchain_runtime_ga_operations_advanced_integration_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  record.advanced_performance_ready =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedPerformanceReady(
          record.advanced_performance_consistent,
          toolchain_runtime_ga_operations_advanced_integration_key,
          surface.long_tail_grammar_integration_closeout_key);
  const std::string toolchain_runtime_ga_operations_advanced_performance_key =
      BuildObjc3ToolchainRuntimeGaOperationsAdvancedPerformanceKey(
          record.advanced_integration_consistent,
          record.advanced_integration_ready,
          toolchain_runtime_ga_operations_advanced_integration_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          record.advanced_performance_consistent,
          record.advanced_performance_ready);
  surface.toolchain_runtime_ga_operations_advanced_performance_consistent =
      record.advanced_performance_consistent;
  surface.toolchain_runtime_ga_operations_advanced_performance_ready =
      record.advanced_performance_ready;
  surface.toolchain_runtime_ga_operations_advanced_performance_key =
      toolchain_runtime_ga_operations_advanced_performance_key;
  surface.long_tail_grammar_integration_closeout_consistent =
      surface.long_tail_grammar_integration_closeout_consistent &&
      record.advanced_performance_consistent;
  surface.long_tail_grammar_gate_signoff_ready =
      surface.long_tail_grammar_gate_signoff_ready &&
      record.advanced_performance_ready;
  surface.long_tail_grammar_integration_closeout_key +=
      ";toolchain_runtime_ga_operations_advanced_performance_key=" +
      toolchain_runtime_ga_operations_advanced_performance_key;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_advanced_performance_key=" +
      toolchain_runtime_ga_operations_advanced_performance_key;

  record.advanced_core_shard2_consistent =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreShard2Consistent(
          record.advanced_performance_consistent,
          record.advanced_performance_ready,
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready,
          toolchain_runtime_ga_operations_advanced_performance_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  record.advanced_core_shard2_ready =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreShard2Ready(
          record.advanced_core_shard2_consistent,
          toolchain_runtime_ga_operations_advanced_performance_key,
          surface.long_tail_grammar_integration_closeout_key);
  const std::string toolchain_runtime_ga_operations_advanced_core_shard2_key =
      BuildObjc3ToolchainRuntimeGaOperationsAdvancedCoreShard2Key(
          record.advanced_performance_consistent,
          record.advanced_performance_ready,
          toolchain_runtime_ga_operations_advanced_performance_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          record.advanced_core_shard2_consistent,
          record.advanced_core_shard2_ready);
  surface.toolchain_runtime_ga_operations_advanced_core_shard2_consistent =
      record.advanced_core_shard2_consistent;
  surface.toolchain_runtime_ga_operations_advanced_core_shard2_ready =
      record.advanced_core_shard2_ready;
  surface.toolchain_runtime_ga_operations_advanced_core_shard2_key =
      toolchain_runtime_ga_operations_advanced_core_shard2_key;
  surface.long_tail_grammar_integration_closeout_consistent =
      surface.long_tail_grammar_integration_closeout_consistent &&
      record.advanced_core_shard2_consistent;
  surface.long_tail_grammar_gate_signoff_ready =
      surface.long_tail_grammar_gate_signoff_ready &&
      record.advanced_core_shard2_ready;
  surface.long_tail_grammar_integration_closeout_key +=
      ";toolchain_runtime_ga_operations_advanced_core_shard2_key=" +
      toolchain_runtime_ga_operations_advanced_core_shard2_key;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_advanced_core_shard2_key=" +
      toolchain_runtime_ga_operations_advanced_core_shard2_key;

  record.integration_closeout_signoff_consistent =
      IsObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffConsistent(
          record.advanced_core_shard2_consistent,
          record.advanced_core_shard2_ready,
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready,
          toolchain_runtime_ga_operations_advanced_core_shard2_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  record.integration_closeout_signoff_ready =
      IsObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffReady(
          record.integration_closeout_signoff_consistent,
          toolchain_runtime_ga_operations_advanced_core_shard2_key,
          surface.long_tail_grammar_integration_closeout_key);
  const std::string toolchain_runtime_ga_operations_integration_closeout_signoff_key =
      BuildObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffKey(
          record.advanced_core_shard2_consistent,
          record.advanced_core_shard2_ready,
          toolchain_runtime_ga_operations_advanced_core_shard2_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          record.integration_closeout_signoff_consistent,
          record.integration_closeout_signoff_ready);
  surface.toolchain_runtime_ga_operations_integration_closeout_signoff_consistent =
      record.integration_closeout_signoff_consistent;
  surface.toolchain_runtime_ga_operations_integration_closeout_signoff_ready =
      record.integration_closeout_signoff_ready;
  surface.toolchain_runtime_ga_operations_integration_closeout_signoff_key =
      toolchain_runtime_ga_operations_integration_closeout_signoff_key;
  surface.long_tail_grammar_integration_closeout_consistent =
      surface.long_tail_grammar_integration_closeout_consistent &&
      record.integration_closeout_signoff_consistent;
  surface.long_tail_grammar_gate_signoff_ready =
      surface.long_tail_grammar_gate_signoff_ready &&
      record.integration_closeout_signoff_ready;
  surface.long_tail_grammar_integration_closeout_key +=
      ";toolchain_runtime_ga_operations_integration_closeout_signoff_key=" +
      toolchain_runtime_ga_operations_integration_closeout_signoff_key;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_integration_closeout_signoff_key=" +
      toolchain_runtime_ga_operations_integration_closeout_signoff_key;

  return record;
}
