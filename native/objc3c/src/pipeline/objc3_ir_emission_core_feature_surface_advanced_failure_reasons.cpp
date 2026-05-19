#include "pipeline/objc3_ir_emission_core_feature_surface_failure_reason_helpers.h"

namespace objc3_ir_emission_core_feature_surface {

void PublishObjc3IREmissionCoreFeatureAdvancedFailureReasons(
    Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  if (surface.core_feature_advanced_core_shard1_ready) {
    surface.advanced_core_shard1_failure_reason.clear();
  } else if (!surface.core_feature_cross_lane_integration_sync_ready) {
    surface.advanced_core_shard1_failure_reason =
        "IR emission core feature cross-lane integration sync is not ready";
  } else if (!surface.pass_graph_advanced_core_shard1_ready) {
    surface.advanced_core_shard1_failure_reason =
        "IR emission core feature pass-graph advanced core shard 1 is not ready";
  } else if (!surface.parse_artifact_advanced_core_shard1_consistent) {
    surface.advanced_core_shard1_failure_reason =
        "IR emission core feature parse artifact advanced core shard 1 is inconsistent";
  } else if (!surface.typed_handoff_advanced_core_shard1_consistent) {
    surface.advanced_core_shard1_failure_reason =
        "IR emission core feature typed handoff advanced core shard 1 is inconsistent";
  } else if (!surface.advanced_core_shard1_consistent) {
    surface.advanced_core_shard1_failure_reason =
        "IR emission core feature advanced core shard 1 is inconsistent";
  } else if (!surface.advanced_core_shard1_key_transport_ready) {
    surface.advanced_core_shard1_failure_reason =
        "IR emission core feature advanced core shard 1 key transport is not ready";
  } else {
    surface.advanced_core_shard1_failure_reason =
        "IR emission core feature advanced core shard 1 surface is not ready";
  }

  if (surface.core_feature_advanced_edge_compatibility_shard1_ready) {
    surface.advanced_edge_compatibility_shard1_failure_reason.clear();
  } else if (!surface.core_feature_advanced_core_shard1_ready) {
    surface.advanced_edge_compatibility_shard1_failure_reason =
        "IR emission core feature advanced core shard 1 is not ready";
  } else if (!surface.pass_graph_advanced_edge_compatibility_shard1_ready) {
    surface.advanced_edge_compatibility_shard1_failure_reason =
        "IR emission core feature pass-graph advanced edge compatibility shard 1 is not ready";
  } else if (!surface.parse_artifact_advanced_edge_compatibility_shard1_consistent) {
    surface.advanced_edge_compatibility_shard1_failure_reason =
        "IR emission core feature parse artifact advanced edge compatibility shard 1 is inconsistent";
  } else if (!surface.typed_handoff_advanced_edge_compatibility_shard1_consistent) {
    surface.advanced_edge_compatibility_shard1_failure_reason =
        "IR emission core feature typed handoff advanced edge compatibility shard 1 is inconsistent";
  } else if (!surface.advanced_edge_compatibility_shard1_consistent) {
    surface.advanced_edge_compatibility_shard1_failure_reason =
        "IR emission core feature advanced edge compatibility shard 1 is inconsistent";
  } else if (!surface.advanced_edge_compatibility_shard1_key_transport_ready) {
    surface.advanced_edge_compatibility_shard1_failure_reason =
        "IR emission core feature advanced edge compatibility shard 1 key transport is not ready";
  } else {
    surface.advanced_edge_compatibility_shard1_failure_reason =
        "IR emission core feature advanced edge compatibility shard 1 surface is not ready";
  }

  if (surface.core_feature_advanced_diagnostics_shard1_ready) {
    surface.advanced_diagnostics_shard1_failure_reason.clear();
  } else if (!surface.core_feature_advanced_edge_compatibility_shard1_ready) {
    surface.advanced_diagnostics_shard1_failure_reason =
        "IR emission core feature advanced edge compatibility shard 1 is not ready";
  } else if (!surface.pass_graph_advanced_diagnostics_shard1_ready) {
    surface.advanced_diagnostics_shard1_failure_reason =
        "IR emission core feature pass-graph advanced diagnostics shard 1 is not ready";
  } else if (!surface.parse_artifact_advanced_diagnostics_shard1_consistent) {
    surface.advanced_diagnostics_shard1_failure_reason =
        "IR emission core feature parse artifact advanced diagnostics shard 1 is inconsistent";
  } else if (!surface.typed_handoff_advanced_diagnostics_shard1_consistent) {
    surface.advanced_diagnostics_shard1_failure_reason =
        "IR emission core feature typed handoff advanced diagnostics shard 1 is inconsistent";
  } else if (!surface.advanced_diagnostics_shard1_consistent) {
    surface.advanced_diagnostics_shard1_failure_reason =
        "IR emission core feature advanced diagnostics shard 1 is inconsistent";
  } else if (!surface.advanced_diagnostics_shard1_key_transport_ready) {
    surface.advanced_diagnostics_shard1_failure_reason =
        "IR emission core feature advanced diagnostics shard 1 key transport is not ready";
  } else {
    surface.advanced_diagnostics_shard1_failure_reason =
        "IR emission core feature advanced diagnostics shard 1 surface is not ready";
  }

  if (surface.core_feature_advanced_conformance_shard1_ready) {
    surface.advanced_conformance_shard1_failure_reason.clear();
  } else if (!surface.core_feature_advanced_diagnostics_shard1_ready) {
    surface.advanced_conformance_shard1_failure_reason =
        "IR emission core feature advanced diagnostics shard 1 is not ready";
  } else if (!surface.pass_graph_advanced_conformance_shard1_ready) {
    surface.advanced_conformance_shard1_failure_reason =
        "IR emission core feature pass-graph advanced conformance shard 1 is not ready";
  } else if (!surface.parse_artifact_advanced_conformance_shard1_consistent) {
    surface.advanced_conformance_shard1_failure_reason =
        "IR emission core feature parse artifact advanced conformance shard 1 is inconsistent";
  } else if (!surface.typed_handoff_advanced_conformance_shard1_consistent) {
    surface.advanced_conformance_shard1_failure_reason =
        "IR emission core feature typed handoff advanced conformance shard 1 is inconsistent";
  } else if (!surface.advanced_conformance_shard1_consistent) {
    surface.advanced_conformance_shard1_failure_reason =
        "IR emission core feature advanced conformance shard 1 is inconsistent";
  } else if (!surface.advanced_conformance_shard1_key_transport_ready) {
    surface.advanced_conformance_shard1_failure_reason =
        "IR emission core feature advanced conformance shard 1 key transport is not ready";
  } else {
    surface.advanced_conformance_shard1_failure_reason =
        "IR emission core feature advanced conformance shard 1 surface is not ready";
  }

  if (surface.core_feature_advanced_integration_shard1_ready) {
    surface.advanced_integration_shard1_failure_reason.clear();
  } else if (!surface.core_feature_advanced_conformance_shard1_ready) {
    surface.advanced_integration_shard1_failure_reason =
        "IR emission core feature advanced conformance shard 1 is not ready";
  } else if (!surface.pass_graph_advanced_integration_shard1_ready) {
    surface.advanced_integration_shard1_failure_reason =
        "IR emission core feature pass-graph advanced integration shard 1 is not ready";
  } else if (!surface.parse_artifact_advanced_integration_shard1_consistent) {
    surface.advanced_integration_shard1_failure_reason =
        "IR emission core feature parse artifact advanced integration shard 1 is inconsistent";
  } else if (!surface.typed_handoff_advanced_integration_shard1_consistent) {
    surface.advanced_integration_shard1_failure_reason =
        "IR emission core feature typed handoff advanced integration shard 1 is inconsistent";
  } else if (!surface.advanced_integration_shard1_consistent) {
    surface.advanced_integration_shard1_failure_reason =
        "IR emission core feature advanced integration shard 1 is inconsistent";
  } else if (!surface.advanced_integration_shard1_key_transport_ready) {
    surface.advanced_integration_shard1_failure_reason =
        "IR emission core feature advanced integration shard 1 key transport is not ready";
  } else {
    surface.advanced_integration_shard1_failure_reason =
        "IR emission core feature advanced integration shard 1 surface is not ready";
  }

}

}  // namespace objc3_ir_emission_core_feature_surface
