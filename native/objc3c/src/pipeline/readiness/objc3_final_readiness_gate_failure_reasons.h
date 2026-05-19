#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

struct Objc3FinalReadinessGateFailureReasonInputs {
  bool replay_keys_ready = false;
  bool lane_expansion_consistent = false;
  bool lane_edge_case_compatibility_consistent = false;
  bool lane_edge_case_expansion_consistent = false;
  bool lane_diagnostics_hardening_consistent = false;
  bool lane_recovery_determinism_consistent = false;
  bool lane_conformance_matrix_consistent = false;
  bool lane_conformance_corpus_consistent = false;
  bool lane_performance_quality_guardrails_consistent = false;
  bool lane_cross_lane_integration_consistent = false;
  bool lane_docs_runbook_sync_consistent = false;
  bool lane_release_candidate_replay_dry_run_consistent = false;
  bool lane_advanced_core_shard1_consistent = false;
  bool lane_advanced_edge_compatibility_shard1_consistent = false;
  bool lane_advanced_diagnostics_shard1_consistent = false;
  bool lane_advanced_conformance_shard1_consistent = false;
  bool lane_advanced_integration_shard1_consistent = false;
  bool lane_advanced_performance_shard1_consistent = false;
  bool lane_advanced_core_shard2_consistent = false;
  bool lane_advanced_core_shard3_consistent = false;
  bool lane_advanced_edge_compatibility_shard3_consistent = false;
  bool lane_advanced_diagnostics_shard3_consistent = false;
  bool lane_advanced_conformance_shard3_consistent = false;
  bool lane_advanced_integration_shard3_consistent = false;
  bool lane_advanced_performance_shard3_consistent = false;
  bool lane_advanced_core_shard4_consistent = false;
  bool lane_advanced_edge_compatibility_shard4_consistent = false;
  bool lane_advanced_integration_closeout_signoff_consistent = false;
  bool lane_advanced_edge_compatibility_shard2_consistent = false;
  bool lane_advanced_diagnostics_shard2_consistent = false;
  bool lane_advanced_conformance_shard2_consistent = false;
  bool lane_advanced_integration_shard2_consistent = false;
  bool lane_advanced_performance_shard2_consistent = false;
  bool lane_integration_closeout_signoff_consistent = false;
};

void ApplyObjc3FinalReadinessGateFailureReason(
    Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FinalReadinessGateFailureReasonInputs &inputs);

bool IsObjc3FinalReadinessGateCoreFeatureImplementationSurfaceReady(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    std::string &reason);
