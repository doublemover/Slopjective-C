#pragma once

#include "pipeline/readiness/objc3_parse_lowering_conformance_performance_readiness.h"

void ApplyObjc3ParseLoweringConformanceMatrixReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    Objc3ParseLoweringConformancePerformanceReadinessRecord &record,
    bool toolchain_runtime_ga_operations_recovery_determinism_consistent,
    bool toolchain_runtime_ga_operations_recovery_determinism_ready,
    bool sema_handoff_ready,
    bool semantic_handoff_deterministic,
    bool typed_core_feature_ready);

void ApplyObjc3ParseLoweringConformanceCorpusReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    Objc3ParseLoweringConformancePerformanceReadinessRecord &record,
    bool sema_handoff_ready,
    bool semantic_handoff_deterministic);

void ApplyObjc3ParseLoweringPerformanceQualityGuardrailsReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    Objc3ParseLoweringConformancePerformanceReadinessRecord &record);

void ApplyObjc3ParseLoweringCrossLaneIntegrationReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    Objc3ParseLoweringConformancePerformanceReadinessRecord &record,
    bool parse_snapshot_replay_ready,
    bool sema_handoff_ready);
