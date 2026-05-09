#include "pipeline/objc3_ir_emission_core_feature_implementation_surface.h"

namespace objc3_ir_emission_core_feature_surface {

void PublishObjc3IREmissionCoreFeatureSurfaceFailureReasons(
    Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  if (surface.core_feature_expansion_ready) {
    surface.expansion_failure_reason.clear();
  } else if (!surface.core_feature_impl_ready) {
    surface.expansion_failure_reason =
        "IR emission core feature implementation is not ready";
  } else if (!surface.pass_graph_expansion_ready) {
    surface.expansion_failure_reason = "pass-graph expansion is not ready";
  } else if (!surface.expansion_metadata_transport_ready) {
    surface.expansion_failure_reason =
        "IR emission core feature expansion metadata transport is not ready";
  } else if (!surface.runtime_boundary_handoff_ready) {
    surface.expansion_failure_reason =
        "runtime boundary handoff replay surface is not ready";
  } else if (!surface.direct_ir_entrypoint_ready) {
    surface.expansion_failure_reason = "direct IR entrypoint is not ready";
  } else {
    surface.expansion_failure_reason =
        "IR emission core feature expansion surface is not ready";
  }

  if (surface.core_feature_edge_case_compatibility_ready) {
    surface.edge_case_compatibility_failure_reason.clear();
  } else if (!surface.core_feature_expansion_ready) {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature expansion is not ready";
  } else if (!surface.pass_graph_edge_case_compatibility_ready) {
    surface.edge_case_compatibility_failure_reason =
        "pass-graph edge-case compatibility is not ready";
  } else if (!surface.compatibility_handoff_consistent) {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature compatibility handoff is inconsistent";
  } else if (!surface.language_version_pragma_coordinate_order_consistent) {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature language version pragma coordinate order is inconsistent";
  } else if (!surface.parse_artifact_edge_case_robustness_consistent) {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature parse artifact edge-case robustness is inconsistent";
  } else if (!surface.parse_artifact_replay_key_deterministic) {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature parse artifact replay key is not deterministic";
  } else if (!surface.edge_case_compatibility_key_transport_ready) {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature edge-case compatibility key transport is not ready";
  } else {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature edge-case compatibility surface is not ready";
  }

  if (surface.core_feature_edge_case_robustness_ready) {
    surface.edge_case_robustness_failure_reason.clear();
  } else if (!surface.core_feature_edge_case_compatibility_ready) {
    surface.edge_case_robustness_failure_reason =
        "IR emission core feature edge-case compatibility is not ready";
  } else if (!surface.pass_graph_edge_case_robustness_ready) {
    surface.edge_case_robustness_failure_reason =
        "pass-graph edge-case robustness is not ready";
  } else if (!surface.edge_case_expansion_consistent) {
    surface.edge_case_robustness_failure_reason =
        "IR emission core feature edge-case expansion is inconsistent";
  } else if (!surface.parse_artifact_edge_case_robustness_ready) {
    surface.edge_case_robustness_failure_reason =
        "IR emission core feature parse artifact edge-case robustness is not ready";
  } else if (!surface.edge_case_robustness_key_transport_ready) {
    surface.edge_case_robustness_failure_reason =
        "IR emission core feature edge-case robustness key transport is not ready";
  } else {
    surface.edge_case_robustness_failure_reason =
        "IR emission core feature edge-case robustness surface is not ready";
  }

  if (surface.core_feature_diagnostics_hardening_ready) {
    surface.diagnostics_hardening_failure_reason.clear();
  } else if (!surface.core_feature_edge_case_robustness_ready) {
    surface.diagnostics_hardening_failure_reason =
        "IR emission core feature edge-case robustness is not ready";
  } else if (!surface.pass_graph_diagnostics_hardening_ready) {
    surface.diagnostics_hardening_failure_reason =
        "pass-graph diagnostics hardening is not ready";
  } else if (!surface.parse_artifact_diagnostics_hardening_consistent) {
    surface.diagnostics_hardening_failure_reason =
        "IR emission core feature parse artifact diagnostics hardening is inconsistent";
  } else if (!surface.diagnostics_hardening_consistent) {
    surface.diagnostics_hardening_failure_reason =
        "IR emission core feature diagnostics hardening is inconsistent";
  } else if (!surface.diagnostics_hardening_key_transport_ready) {
    surface.diagnostics_hardening_failure_reason =
        "IR emission core feature diagnostics hardening key transport is not ready";
  } else {
    surface.diagnostics_hardening_failure_reason =
        "IR emission core feature diagnostics hardening surface is not ready";
  }

  if (surface.core_feature_recovery_determinism_ready) {
    surface.recovery_determinism_failure_reason.clear();
  } else if (!surface.core_feature_diagnostics_hardening_ready) {
    surface.recovery_determinism_failure_reason =
        "IR emission core feature diagnostics hardening is not ready";
  } else if (!surface.pass_graph_recovery_determinism_ready) {
    surface.recovery_determinism_failure_reason =
        "pass-graph recovery determinism is not ready";
  } else if (!surface.parse_artifact_recovery_determinism_hardening_consistent) {
    surface.recovery_determinism_failure_reason =
        "IR emission core feature parse artifact recovery determinism hardening is inconsistent";
  } else if (!surface.recovery_determinism_consistent) {
    surface.recovery_determinism_failure_reason =
        "IR emission core feature recovery determinism hardening is inconsistent";
  } else if (!surface.recovery_determinism_key_transport_ready) {
    surface.recovery_determinism_failure_reason =
        "IR emission core feature recovery determinism hardening key transport is not ready";
  } else {
    surface.recovery_determinism_failure_reason =
        "IR emission core feature recovery determinism hardening surface is not ready";
  }

  if (surface.core_feature_conformance_matrix_ready) {
    surface.conformance_matrix_failure_reason.clear();
  } else if (!surface.core_feature_recovery_determinism_ready) {
    surface.conformance_matrix_failure_reason =
        "IR emission core feature recovery determinism hardening is not ready";
  } else if (!surface.pass_graph_conformance_matrix_ready) {
    surface.conformance_matrix_failure_reason =
        "pass-graph conformance matrix is not ready";
  } else if (!surface.parse_artifact_conformance_matrix_consistent) {
    surface.conformance_matrix_failure_reason =
        "IR emission core feature parse artifact conformance matrix is inconsistent";
  } else if (!surface.conformance_matrix_consistent) {
    surface.conformance_matrix_failure_reason =
        "IR emission core feature conformance matrix is inconsistent";
  } else if (!surface.conformance_matrix_key_transport_ready) {
    surface.conformance_matrix_failure_reason =
        "IR emission core feature conformance matrix key transport is not ready";
  } else {
    surface.conformance_matrix_failure_reason =
        "IR emission core feature conformance matrix surface is not ready";
  }

  if (surface.core_feature_conformance_corpus_ready) {
    surface.conformance_corpus_failure_reason.clear();
  } else if (!surface.core_feature_conformance_matrix_ready) {
    surface.conformance_corpus_failure_reason =
        "IR emission core feature conformance matrix is not ready";
  } else if (!surface.pass_graph_conformance_corpus_ready) {
    surface.conformance_corpus_failure_reason =
        "pass-graph conformance corpus is not ready";
  } else if (!surface.parse_artifact_conformance_corpus_consistent) {
    surface.conformance_corpus_failure_reason =
        "IR emission core feature parse artifact conformance corpus is inconsistent";
  } else if (!surface.conformance_corpus_consistent) {
    surface.conformance_corpus_failure_reason =
        "IR emission core feature conformance corpus is inconsistent";
  } else if (!surface.conformance_corpus_key_transport_ready) {
    surface.conformance_corpus_failure_reason =
        "IR emission core feature conformance corpus key transport is not ready";
  } else {
    surface.conformance_corpus_failure_reason =
        "IR emission core feature conformance corpus surface is not ready";
  }

  if (surface.core_feature_performance_quality_guardrails_ready) {
    surface.performance_quality_guardrails_failure_reason.clear();
  } else if (!surface.core_feature_conformance_corpus_ready) {
    surface.performance_quality_guardrails_failure_reason =
        "IR emission core feature conformance corpus is not ready";
  } else if (!surface.pass_graph_performance_quality_guardrails_ready) {
    surface.performance_quality_guardrails_failure_reason =
        "pass-graph performance quality guardrails are not ready";
  } else if (!surface.parse_artifact_performance_quality_guardrails_consistent) {
    surface.performance_quality_guardrails_failure_reason =
        "IR emission core feature parse artifact performance quality guardrails are inconsistent";
  } else if (!surface.performance_quality_guardrails_consistent) {
    surface.performance_quality_guardrails_failure_reason =
        "IR emission core feature performance quality guardrails are inconsistent";
  } else if (!surface.performance_quality_guardrails_key_transport_ready) {
    surface.performance_quality_guardrails_failure_reason =
        "IR emission core feature performance quality guardrails key transport is not ready";
  } else {
    surface.performance_quality_guardrails_failure_reason =
        "IR emission core feature performance quality guardrails surface is not ready";
  }

  if (surface.core_feature_cross_lane_integration_sync_ready) {
    surface.cross_lane_integration_sync_failure_reason.clear();
  } else if (!surface.core_feature_performance_quality_guardrails_ready) {
    surface.cross_lane_integration_sync_failure_reason =
        "IR emission core feature performance quality guardrails are not ready";
  } else if (!surface.pass_graph_cross_lane_integration_sync_ready) {
    surface.cross_lane_integration_sync_failure_reason =
        "pass-graph cross-lane integration sync is not ready";
  } else if (!surface.parse_artifact_cross_lane_integration_sync_consistent) {
    surface.cross_lane_integration_sync_failure_reason =
        "IR emission core feature parse artifact cross-lane integration sync is inconsistent";
  } else if (!surface.cross_lane_integration_sync_consistent) {
    surface.cross_lane_integration_sync_failure_reason =
        "IR emission core feature cross-lane integration sync is inconsistent";
  } else if (!surface.cross_lane_integration_sync_key_transport_ready) {
    surface.cross_lane_integration_sync_failure_reason =
        "IR emission core feature cross-lane integration sync key transport is not ready";
  } else {
    surface.cross_lane_integration_sync_failure_reason =
        "IR emission core feature cross-lane integration sync surface is not ready";
  }

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

  if (surface.core_feature_impl_ready) {
    return;
  }

  if (!surface.modular_split_ready) {
    surface.failure_reason =
        "IR emission completeness modular split scaffold is not ready";
  } else if (!surface.metadata_transport_ready) {
    surface.failure_reason =
        "IR emission completeness metadata transport is not ready";
  } else if (!surface.pass_graph_core_feature_ready) {
    surface.failure_reason = "pass-graph core feature is not ready";
  } else if (!surface.runtime_boundary_handoff_ready) {
    surface.failure_reason =
        "runtime boundary handoff replay surface is not ready";
  } else if (!surface.direct_ir_entrypoint_ready) {
    surface.failure_reason = "direct IR entrypoint is not ready";
  } else {
    surface.failure_reason =
        "IR emission core feature implementation surface is not ready";
  }
}

}  // namespace objc3_ir_emission_core_feature_surface
