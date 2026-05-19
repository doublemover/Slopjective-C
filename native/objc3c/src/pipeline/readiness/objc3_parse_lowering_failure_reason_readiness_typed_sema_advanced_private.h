#pragma once

#include "pipeline/readiness/objc3_parse_lowering_failure_reason_readiness.h"

namespace objc3_parse_lowering_failure_reason_readiness_detail {

inline const char *FindTypedSemaLoweringAdvancedSurfaceFailureReason(
    const Objc3ParseLoweringReadinessSurface &surface) {
  if (!surface.typed_sema_release_candidate_replay_dry_run_consistent) {
    return "typed sema-to-lowering release-candidate replay dry-run is inconsistent";
  }

  if (!surface.typed_sema_release_candidate_replay_dry_run_ready) {
    return "typed sema-to-lowering release-candidate replay dry-run is not ready";
  }

  if (surface.typed_sema_release_candidate_replay_dry_run_key.empty()) {
    return "typed sema-to-lowering release-candidate replay dry-run key is empty";
  }

  if (!surface.typed_sema_advanced_core_shard1_consistent) {
    return "typed sema-to-lowering advanced core shard 1 is inconsistent";
  }

  if (!surface.typed_sema_advanced_core_shard1_ready) {
    return "typed sema-to-lowering advanced core shard 1 is not ready";
  }

  if (surface.typed_sema_advanced_core_shard1_key.empty()) {
    return "typed sema-to-lowering advanced core shard 1 key is empty";
  }

  if (!surface.typed_sema_advanced_edge_compatibility_shard1_consistent) {
    return "typed sema-to-lowering advanced edge compatibility shard 1 is inconsistent";
  }

  if (!surface.typed_sema_advanced_edge_compatibility_shard1_ready) {
    return "typed sema-to-lowering advanced edge compatibility shard 1 is not ready";
  }

  if (surface.typed_sema_advanced_edge_compatibility_shard1_key.empty()) {
    return "typed sema-to-lowering advanced edge compatibility shard 1 key is empty";
  }

  if (!surface.typed_sema_advanced_diagnostics_shard1_consistent) {
    return "typed sema-to-lowering advanced diagnostics shard 1 is inconsistent";
  }

  if (!surface.typed_sema_advanced_diagnostics_shard1_ready) {
    return "typed sema-to-lowering advanced diagnostics shard 1 is not ready";
  }

  if (surface.typed_sema_advanced_diagnostics_shard1_key.empty()) {
    return "typed sema-to-lowering advanced diagnostics shard 1 key is empty";
  }

  if (!surface.typed_sema_advanced_conformance_shard1_consistent) {
    return "typed sema-to-lowering advanced conformance shard 1 is inconsistent";
  }

  if (!surface.typed_sema_advanced_conformance_shard1_ready) {
    return "typed sema-to-lowering advanced conformance shard 1 is not ready";
  }

  if (surface.typed_sema_advanced_conformance_shard1_key.empty()) {
    return "typed sema-to-lowering advanced conformance shard 1 key is empty";
  }

  if (!surface.typed_sema_advanced_integration_shard1_consistent) {
    return "typed sema-to-lowering advanced integration shard 1 is inconsistent";
  }

  if (!surface.typed_sema_advanced_integration_shard1_ready) {
    return "typed sema-to-lowering advanced integration shard 1 is not ready";
  }

  if (surface.typed_sema_advanced_integration_shard1_key.empty()) {
    return "typed sema-to-lowering advanced integration shard 1 key is empty";
  }

  if (!surface.typed_sema_advanced_performance_shard1_consistent) {
    return "typed sema-to-lowering advanced performance shard 1 is inconsistent";
  }

  if (!surface.typed_sema_advanced_performance_shard1_ready) {
    return "typed sema-to-lowering advanced performance shard 1 is not ready";
  }

  if (surface.typed_sema_advanced_performance_shard1_key.empty()) {
    return "typed sema-to-lowering advanced performance shard 1 key is empty";
  }

  if (!surface.typed_sema_advanced_core_shard2_consistent) {
    return "typed sema-to-lowering advanced core shard 2 is inconsistent";
  }

  if (!surface.typed_sema_advanced_core_shard2_ready) {
    return "typed sema-to-lowering advanced core shard 2 is not ready";
  }

  if (surface.typed_sema_advanced_core_shard2_key.empty()) {
    return "typed sema-to-lowering advanced core shard 2 key is empty";
  }

  if (!surface.typed_sema_advanced_edge_compatibility_shard2_consistent) {
    return "typed sema-to-lowering advanced edge compatibility shard 2 is inconsistent";
  }

  if (!surface.typed_sema_advanced_edge_compatibility_shard2_ready) {
    return "typed sema-to-lowering advanced edge compatibility shard 2 is not ready";
  }

  if (surface.typed_sema_advanced_edge_compatibility_shard2_key.empty()) {
    return "typed sema-to-lowering advanced edge compatibility shard 2 key is empty";
  }

  if (!surface.typed_sema_advanced_diagnostics_shard2_consistent) {
    return "typed sema-to-lowering advanced diagnostics shard 2 is inconsistent";
  }

  if (!surface.typed_sema_advanced_diagnostics_shard2_ready) {
    return "typed sema-to-lowering advanced diagnostics shard 2 is not ready";
  }

  if (surface.typed_sema_advanced_diagnostics_shard2_key.empty()) {
    return "typed sema-to-lowering advanced diagnostics shard 2 key is empty";
  }

  if (!surface.typed_sema_advanced_conformance_shard2_consistent) {
    return "typed sema-to-lowering advanced conformance shard 2 is inconsistent";
  }

  if (!surface.typed_sema_advanced_conformance_shard2_ready) {
    return "typed sema-to-lowering advanced conformance shard 2 is not ready";
  }

  if (surface.typed_sema_advanced_conformance_shard2_key.empty()) {
    return "typed sema-to-lowering advanced conformance shard 2 key is empty";
  }

  if (!surface.typed_sema_advanced_integration_shard2_consistent) {
    return "typed sema-to-lowering advanced integration shard 2 is inconsistent";
  }

  if (!surface.typed_sema_advanced_integration_shard2_ready) {
    return "typed sema-to-lowering advanced integration shard 2 is not ready";
  }

  if (surface.typed_sema_advanced_integration_shard2_key.empty()) {
    return "typed sema-to-lowering advanced integration shard 2 key is empty";
  }

  if (!surface.typed_sema_integration_closeout_signoff_consistent) {
    return "typed sema-to-lowering integration closeout/sign-off is inconsistent";
  }

  if (!surface.typed_sema_integration_closeout_signoff_ready) {
    return "typed sema-to-lowering integration closeout/sign-off is not ready";
  }

  if (surface.typed_sema_integration_closeout_signoff_key.empty()) {
    return "typed sema-to-lowering integration closeout/sign-off key is empty";
  }

  return nullptr;
}

}  // namespace objc3_parse_lowering_failure_reason_readiness_detail
