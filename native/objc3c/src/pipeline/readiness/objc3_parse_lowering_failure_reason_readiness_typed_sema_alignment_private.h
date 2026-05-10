#pragma once

#include "pipeline/readiness/objc3_parse_lowering_failure_reason_readiness.h"

namespace objc3_parse_lowering_failure_reason_readiness_detail {

inline const char *FindTypedSemaLoweringAlignmentFailureReason(
    const Objc3TypedSemaLoweringReadinessRecord
        &typed_sema_lowering_readiness) {
  if (!typed_sema_lowering_readiness.typed_edge_case_compatibility_alignment) {
    return "typed sema-to-lowering edge-case compatibility drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_edge_case_robustness_alignment) {
    return "typed sema-to-lowering edge-case robustness drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_diagnostics_hardening_alignment) {
    return "typed sema-to-lowering diagnostics hardening drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_recovery_determinism_alignment) {
    return "typed sema-to-lowering recovery/determinism drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_conformance_matrix_alignment) {
    return "typed sema-to-lowering conformance matrix drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_conformance_corpus_alignment) {
    return "typed sema-to-lowering conformance corpus drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_performance_quality_guardrails_alignment) {
    return "typed sema-to-lowering performance/quality guardrails drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_cross_lane_integration_alignment) {
    return "typed sema-to-lowering cross-lane integration drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_docs_runbook_sync_alignment) {
    return "typed sema-to-lowering docs/runbook synchronization drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_release_candidate_replay_dry_run_alignment) {
    return "typed sema-to-lowering release-candidate replay dry-run drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_advanced_core_shard1_alignment) {
    return "typed sema-to-lowering advanced core shard 1 drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_advanced_edge_compatibility_shard1_alignment) {
    return "typed sema-to-lowering advanced edge compatibility shard 1 drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_advanced_diagnostics_shard1_alignment) {
    return "typed sema-to-lowering advanced diagnostics shard 1 drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_advanced_conformance_shard1_alignment) {
    return "typed sema-to-lowering advanced conformance shard 1 drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_advanced_integration_shard1_alignment) {
    return "typed sema-to-lowering advanced integration shard 1 drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_advanced_performance_shard1_alignment) {
    return "typed sema-to-lowering advanced performance shard 1 drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_advanced_core_shard2_alignment) {
    return "typed sema-to-lowering advanced core shard 2 drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_advanced_edge_compatibility_shard2_alignment) {
    return "typed sema-to-lowering advanced edge compatibility shard 2 drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_advanced_diagnostics_shard2_alignment) {
    return "typed sema-to-lowering advanced diagnostics shard 2 drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_advanced_conformance_shard2_alignment) {
    return "typed sema-to-lowering advanced conformance shard 2 drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_advanced_integration_shard2_alignment) {
    return "typed sema-to-lowering advanced integration shard 2 drifted from parse/lowering readiness";
  }

  if (!typed_sema_lowering_readiness.typed_integration_closeout_signoff_alignment) {
    return "typed sema-to-lowering integration closeout/sign-off drifted from parse/lowering readiness";
  }

  return nullptr;
}

}  // namespace objc3_parse_lowering_failure_reason_readiness_detail
