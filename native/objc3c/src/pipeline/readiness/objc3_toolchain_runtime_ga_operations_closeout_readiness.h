#pragma once

#include "pipeline/objc3_frontend_types.h"

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

Objc3ToolchainRuntimeGaOperationsCloseoutReadinessRecord
ApplyObjc3ToolchainRuntimeGaOperationsCloseoutReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    bool diagnostics_clear,
    bool sema_handoff_ready,
    bool toolchain_runtime_ga_operations_cross_lane_integration_consistent,
    bool toolchain_runtime_ga_operations_cross_lane_integration_ready);
