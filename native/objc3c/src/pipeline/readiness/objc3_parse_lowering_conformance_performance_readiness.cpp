#include "pipeline/readiness/objc3_parse_lowering_conformance_performance_readiness.h"

#include "pipeline/readiness/objc3_parse_lowering_conformance_performance_readiness_private.h"

Objc3ParseLoweringConformancePerformanceReadinessRecord
ApplyObjc3ParseLoweringConformancePerformanceReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    bool toolchain_runtime_ga_operations_recovery_determinism_consistent,
    bool toolchain_runtime_ga_operations_recovery_determinism_ready,
    bool parse_snapshot_replay_ready,
    bool sema_handoff_ready,
    bool semantic_handoff_deterministic,
    bool typed_core_feature_ready) {
  Objc3ParseLoweringConformancePerformanceReadinessRecord record;

  ApplyObjc3ParseLoweringConformanceMatrixReadiness(
      surface,
      record,
      toolchain_runtime_ga_operations_recovery_determinism_consistent,
      toolchain_runtime_ga_operations_recovery_determinism_ready,
      sema_handoff_ready,
      semantic_handoff_deterministic,
      typed_core_feature_ready);
  ApplyObjc3ParseLoweringConformanceCorpusReadiness(
      surface,
      record,
      sema_handoff_ready,
      semantic_handoff_deterministic);
  ApplyObjc3ParseLoweringPerformanceQualityGuardrailsReadiness(surface, record);
  ApplyObjc3ParseLoweringCrossLaneIntegrationReadiness(
      surface,
      record,
      parse_snapshot_replay_ready,
      sema_handoff_ready);

  return record;
}
