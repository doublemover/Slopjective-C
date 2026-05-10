#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_closeout_readiness_private.h"

#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_advanced_readiness_keys.h"
#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_readiness_keys.h"

void ApplyObjc3ToolchainRuntimeGaOperationsDocsRunbookSyncCloseoutReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    Objc3ToolchainRuntimeGaOperationsCloseoutReadinessRecord &record,
    bool toolchain_runtime_ga_operations_cross_lane_integration_consistent,
    bool toolchain_runtime_ga_operations_cross_lane_integration_ready,
    std::string &toolchain_runtime_ga_operations_docs_runbook_sync_key) {
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
  toolchain_runtime_ga_operations_docs_runbook_sync_key =
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
}

void ApplyObjc3ToolchainRuntimeGaOperationsAdvancedCoreCloseoutReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    Objc3ToolchainRuntimeGaOperationsCloseoutReadinessRecord &record,
    const std::string &toolchain_runtime_ga_operations_docs_runbook_sync_key,
    std::string &toolchain_runtime_ga_operations_advanced_core_key) {
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
  toolchain_runtime_ga_operations_advanced_core_key =
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
}
