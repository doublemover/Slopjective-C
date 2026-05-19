#pragma once

#include <string>

#include "lower/contracts/runtime_dispatch_boundary_contracts.h"
#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_typed_sema_to_lowering_contract_surface.h"

struct Objc3TypedSemaLoweringReadinessRecord {
  Objc3TypedSemaToLoweringContractSurface contract_surface;
  bool typed_core_feature_ready = false;
  bool sema_handoff_ready = false;
  bool semantic_handoff_deterministic = false;
  bool typed_edge_case_compatibility_alignment = false;
  bool typed_edge_case_robustness_alignment = false;
  bool typed_diagnostics_hardening_alignment = false;
  bool typed_recovery_determinism_alignment = false;
  bool typed_conformance_matrix_alignment = false;
  bool typed_conformance_corpus_alignment = false;
  bool typed_performance_quality_guardrails_alignment = false;
  bool typed_cross_lane_integration_alignment = false;
  bool typed_docs_runbook_sync_alignment = false;
  bool typed_release_candidate_replay_dry_run_alignment = false;
  bool typed_advanced_core_shard1_alignment = false;
  bool typed_advanced_edge_compatibility_shard1_alignment = false;
  bool typed_advanced_diagnostics_shard1_alignment = false;
  bool typed_advanced_conformance_shard1_alignment = false;
  bool typed_advanced_integration_shard1_alignment = false;
  bool typed_advanced_performance_shard1_alignment = false;
  bool typed_advanced_core_shard2_alignment = false;
  bool typed_advanced_edge_compatibility_shard2_alignment = false;
  bool typed_advanced_diagnostics_shard2_alignment = false;
  bool typed_advanced_conformance_shard2_alignment = false;
  bool typed_advanced_integration_shard2_alignment = false;
  bool typed_integration_closeout_signoff_alignment = false;
};

bool HasObjc3TypedSemaToLoweringCoreFeatureSurface(
    const Objc3TypedSemaToLoweringContractSurface &surface);

Objc3TypedSemaToLoweringContractSurface ResolveObjc3TypedSemaToLoweringContractSurface(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options);

Objc3TypedSemaLoweringReadinessRecord BuildObjc3TypedSemaLoweringReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options);
