#include <string>

#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_closeout_readiness.h"
#include "pipeline/readiness/objc3_long_tail_grammar_readiness_keys.h"
#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_advanced_readiness_keys.h"
#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_closeout_readiness_private.h"
#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_readiness_keys.h"

namespace {

#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_closeout_advanced_tail_readiness.inc"

}  // namespace

Objc3ToolchainRuntimeGaOperationsCloseoutReadinessRecord
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
  std::string toolchain_runtime_ga_operations_docs_runbook_sync_key;
  ApplyObjc3ToolchainRuntimeGaOperationsDocsRunbookSyncCloseoutReadiness(
      surface,
      record,
      toolchain_runtime_ga_operations_cross_lane_integration_consistent,
      toolchain_runtime_ga_operations_cross_lane_integration_ready,
      toolchain_runtime_ga_operations_docs_runbook_sync_key);
  std::string toolchain_runtime_ga_operations_advanced_core_key;
  ApplyObjc3ToolchainRuntimeGaOperationsAdvancedCoreCloseoutReadiness(
      surface,
      record,
      toolchain_runtime_ga_operations_docs_runbook_sync_key,
      toolchain_runtime_ga_operations_advanced_core_key);
  ApplyObjc3ToolchainRuntimeGaOperationsAdvancedTailCloseoutReadiness(
      surface, record, toolchain_runtime_ga_operations_advanced_core_key);

  return record;
}
