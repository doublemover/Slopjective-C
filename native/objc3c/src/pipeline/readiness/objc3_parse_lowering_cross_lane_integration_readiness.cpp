#include "pipeline/readiness/objc3_parse_lowering_conformance_performance_readiness_private.h"

#include <string>

#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_readiness_keys.h"

void ApplyObjc3ParseLoweringCrossLaneIntegrationReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    Objc3ParseLoweringConformancePerformanceReadinessRecord &record,
    bool parse_snapshot_replay_ready,
    bool sema_handoff_ready) {
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
}
