#pragma once

#include "pipeline/objc3_frontend_types.h"

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

Objc3ParseLoweringConformancePerformanceReadinessRecord
ApplyObjc3ParseLoweringConformancePerformanceReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    bool toolchain_runtime_ga_operations_recovery_determinism_consistent,
    bool toolchain_runtime_ga_operations_recovery_determinism_ready,
    bool parse_snapshot_replay_ready,
    bool sema_handoff_ready,
    bool semantic_handoff_deterministic,
    bool typed_core_feature_ready);
