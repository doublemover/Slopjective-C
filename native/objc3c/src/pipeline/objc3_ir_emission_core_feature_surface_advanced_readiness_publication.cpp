#include "pipeline/objc3_ir_emission_core_feature_surface_readiness_publication.h"

namespace objc3_ir_emission_core_feature_surface {

void PublishObjc3IREmissionCoreFeatureAdvancedShard1Readiness(
    Objc3IREmissionCoreFeatureImplementationSurface &surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface,
    const Objc3TypedSemaToLoweringContractSurface &typed_surface) {
  surface.pass_graph_advanced_core_shard1_ready =
      surface.core_feature_cross_lane_integration_sync_ready &&
      surface.pass_graph_cross_lane_integration_sync_ready;
  surface.pass_graph_advanced_core_shard1_key =
      surface.cross_lane_integration_sync_key;
  surface.parse_artifact_advanced_core_shard1_consistent =
      parse_surface.toolchain_runtime_ga_operations_advanced_core_consistent &&
      parse_surface.toolchain_runtime_ga_operations_advanced_core_ready;
  const bool typed_advanced_core_shard1_alignment =
      parse_surface.typed_sema_advanced_core_shard1_consistent ==
          typed_surface.typed_advanced_core_shard1_consistent &&
      parse_surface.typed_sema_advanced_core_shard1_ready ==
          typed_surface.typed_advanced_core_shard1_ready &&
      parse_surface.typed_sema_advanced_core_shard1_key ==
          typed_surface.typed_advanced_core_shard1_key;
  surface.typed_handoff_advanced_core_shard1_consistent =
      typed_advanced_core_shard1_alignment &&
      parse_surface.typed_sema_advanced_core_shard1_consistent &&
      parse_surface.typed_sema_advanced_core_shard1_ready &&
      typed_surface.typed_advanced_core_shard1_consistent &&
      typed_surface.typed_advanced_core_shard1_ready;
  surface.advanced_core_shard1_consistent =
      surface.core_feature_cross_lane_integration_sync_ready &&
      surface.pass_graph_advanced_core_shard1_ready &&
      surface.parse_artifact_advanced_core_shard1_consistent &&
      surface.typed_handoff_advanced_core_shard1_consistent;
  surface.advanced_core_shard1_key_transport_ready =
      !surface.pass_graph_advanced_core_shard1_key.empty() &&
      !surface.parse_artifact_advanced_core_shard1_key.empty() &&
      !surface.typed_handoff_advanced_core_shard1_key.empty();
  surface.core_feature_advanced_core_shard1_ready =
      surface.core_feature_cross_lane_integration_sync_ready &&
      surface.pass_graph_advanced_core_shard1_ready &&
      surface.advanced_core_shard1_consistent &&
      surface.advanced_core_shard1_key_transport_ready;
  surface.advanced_core_shard1_key =
      BuildObjc3IREmissionCoreFeatureAdvancedCoreShard1Key(surface);
  surface.pass_graph_advanced_edge_compatibility_shard1_ready =
      surface.core_feature_advanced_core_shard1_ready &&
      surface.pass_graph_advanced_core_shard1_ready;
  surface.pass_graph_advanced_edge_compatibility_shard1_key =
      surface.advanced_core_shard1_key;
  surface.parse_artifact_advanced_edge_compatibility_shard1_consistent =
      parse_surface
          .toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent &&
      parse_surface.toolchain_runtime_ga_operations_advanced_edge_compatibility_ready;
  const bool typed_advanced_edge_compatibility_shard1_alignment =
      parse_surface.typed_sema_advanced_edge_compatibility_shard1_consistent ==
          typed_surface.typed_advanced_edge_compatibility_shard1_consistent &&
      parse_surface.typed_sema_advanced_edge_compatibility_shard1_ready ==
          typed_surface.typed_advanced_edge_compatibility_shard1_ready &&
      parse_surface.typed_sema_advanced_edge_compatibility_shard1_key ==
          typed_surface.typed_advanced_edge_compatibility_shard1_key;
  surface.typed_handoff_advanced_edge_compatibility_shard1_consistent =
      typed_advanced_edge_compatibility_shard1_alignment &&
      parse_surface.typed_sema_advanced_edge_compatibility_shard1_consistent &&
      parse_surface.typed_sema_advanced_edge_compatibility_shard1_ready &&
      typed_surface.typed_advanced_edge_compatibility_shard1_consistent &&
      typed_surface.typed_advanced_edge_compatibility_shard1_ready;
  surface.advanced_edge_compatibility_shard1_consistent =
      surface.core_feature_advanced_core_shard1_ready &&
      surface.pass_graph_advanced_edge_compatibility_shard1_ready &&
      surface.parse_artifact_advanced_edge_compatibility_shard1_consistent &&
      surface.typed_handoff_advanced_edge_compatibility_shard1_consistent;
  surface.advanced_edge_compatibility_shard1_key_transport_ready =
      !surface.pass_graph_advanced_edge_compatibility_shard1_key.empty() &&
      !surface.parse_artifact_advanced_edge_compatibility_shard1_key.empty() &&
      !surface.typed_handoff_advanced_edge_compatibility_shard1_key.empty();
  surface.core_feature_advanced_edge_compatibility_shard1_ready =
      surface.core_feature_advanced_core_shard1_ready &&
      surface.pass_graph_advanced_edge_compatibility_shard1_ready &&
      surface.advanced_edge_compatibility_shard1_consistent &&
      surface.advanced_edge_compatibility_shard1_key_transport_ready;
  surface.advanced_edge_compatibility_shard1_key =
      BuildObjc3IREmissionCoreFeatureAdvancedEdgeCompatibilityShard1Key(
          surface);
  surface.pass_graph_advanced_diagnostics_shard1_ready =
      surface.core_feature_advanced_edge_compatibility_shard1_ready &&
      surface.pass_graph_advanced_edge_compatibility_shard1_ready;
  surface.pass_graph_advanced_diagnostics_shard1_key =
      surface.advanced_edge_compatibility_shard1_key;
  surface.parse_artifact_advanced_diagnostics_shard1_consistent =
      parse_surface.toolchain_runtime_ga_operations_advanced_diagnostics_consistent &&
      parse_surface.toolchain_runtime_ga_operations_advanced_diagnostics_ready;
  const bool typed_advanced_diagnostics_shard1_alignment =
      parse_surface.typed_sema_advanced_diagnostics_shard1_consistent ==
          typed_surface.typed_advanced_diagnostics_shard1_consistent &&
      parse_surface.typed_sema_advanced_diagnostics_shard1_ready ==
          typed_surface.typed_advanced_diagnostics_shard1_ready &&
      parse_surface.typed_sema_advanced_diagnostics_shard1_key ==
          typed_surface.typed_advanced_diagnostics_shard1_key;
  surface.typed_handoff_advanced_diagnostics_shard1_consistent =
      typed_advanced_diagnostics_shard1_alignment &&
      parse_surface.typed_sema_advanced_diagnostics_shard1_consistent &&
      parse_surface.typed_sema_advanced_diagnostics_shard1_ready &&
      typed_surface.typed_advanced_diagnostics_shard1_consistent &&
      typed_surface.typed_advanced_diagnostics_shard1_ready;
  surface.advanced_diagnostics_shard1_consistent =
      surface.core_feature_advanced_edge_compatibility_shard1_ready &&
      surface.pass_graph_advanced_diagnostics_shard1_ready &&
      surface.parse_artifact_advanced_diagnostics_shard1_consistent &&
      surface.typed_handoff_advanced_diagnostics_shard1_consistent;
  surface.advanced_diagnostics_shard1_key_transport_ready =
      !surface.pass_graph_advanced_diagnostics_shard1_key.empty() &&
      !surface.parse_artifact_advanced_diagnostics_shard1_key.empty() &&
      !surface.typed_handoff_advanced_diagnostics_shard1_key.empty();
  surface.core_feature_advanced_diagnostics_shard1_ready =
      surface.core_feature_advanced_edge_compatibility_shard1_ready &&
      surface.pass_graph_advanced_diagnostics_shard1_ready &&
      surface.advanced_diagnostics_shard1_consistent &&
      surface.advanced_diagnostics_shard1_key_transport_ready;
  surface.advanced_diagnostics_shard1_key =
      BuildObjc3IREmissionCoreFeatureAdvancedDiagnosticsShard1Key(surface);
  surface.pass_graph_advanced_conformance_shard1_ready =
      surface.core_feature_advanced_diagnostics_shard1_ready &&
      surface.pass_graph_advanced_diagnostics_shard1_ready;
  surface.pass_graph_advanced_conformance_shard1_key =
      surface.advanced_diagnostics_shard1_key;
  surface.parse_artifact_advanced_conformance_shard1_consistent =
      parse_surface.toolchain_runtime_ga_operations_advanced_conformance_consistent &&
      parse_surface.toolchain_runtime_ga_operations_advanced_conformance_ready;
  const bool typed_advanced_conformance_shard1_alignment =
      parse_surface.typed_sema_advanced_conformance_shard1_consistent ==
          typed_surface.typed_advanced_conformance_shard1_consistent &&
      parse_surface.typed_sema_advanced_conformance_shard1_ready ==
          typed_surface.typed_advanced_conformance_shard1_ready &&
      parse_surface.typed_sema_advanced_conformance_shard1_key ==
          typed_surface.typed_advanced_conformance_shard1_key;
  surface.typed_handoff_advanced_conformance_shard1_consistent =
      typed_advanced_conformance_shard1_alignment &&
      parse_surface.typed_sema_advanced_conformance_shard1_consistent &&
      parse_surface.typed_sema_advanced_conformance_shard1_ready &&
      typed_surface.typed_advanced_conformance_shard1_consistent &&
      typed_surface.typed_advanced_conformance_shard1_ready;
  surface.advanced_conformance_shard1_consistent =
      surface.core_feature_advanced_diagnostics_shard1_ready &&
      surface.pass_graph_advanced_conformance_shard1_ready &&
      surface.parse_artifact_advanced_conformance_shard1_consistent &&
      surface.typed_handoff_advanced_conformance_shard1_consistent;
  surface.advanced_conformance_shard1_key_transport_ready =
      !surface.pass_graph_advanced_conformance_shard1_key.empty() &&
      !surface.parse_artifact_advanced_conformance_shard1_key.empty() &&
      !surface.typed_handoff_advanced_conformance_shard1_key.empty();
  surface.core_feature_advanced_conformance_shard1_ready =
      surface.core_feature_advanced_diagnostics_shard1_ready &&
      surface.pass_graph_advanced_conformance_shard1_ready &&
      surface.advanced_conformance_shard1_consistent &&
      surface.advanced_conformance_shard1_key_transport_ready;
  surface.advanced_conformance_shard1_key =
      BuildObjc3IREmissionCoreFeatureAdvancedConformanceShard1Key(surface);
  surface.pass_graph_advanced_integration_shard1_ready =
      surface.core_feature_advanced_conformance_shard1_ready &&
      surface.pass_graph_advanced_conformance_shard1_ready;
  surface.pass_graph_advanced_integration_shard1_key =
      surface.advanced_conformance_shard1_key;
  surface.parse_artifact_advanced_integration_shard1_consistent =
      parse_surface.toolchain_runtime_ga_operations_advanced_integration_consistent &&
      parse_surface.toolchain_runtime_ga_operations_advanced_integration_ready;
  const bool typed_advanced_integration_shard1_alignment =
      parse_surface.typed_sema_advanced_integration_shard1_consistent ==
          typed_surface.typed_advanced_integration_shard1_consistent &&
      parse_surface.typed_sema_advanced_integration_shard1_ready ==
          typed_surface.typed_advanced_integration_shard1_ready &&
      parse_surface.typed_sema_advanced_integration_shard1_key ==
          typed_surface.typed_advanced_integration_shard1_key;
  surface.typed_handoff_advanced_integration_shard1_consistent =
      typed_advanced_integration_shard1_alignment &&
      parse_surface.typed_sema_advanced_integration_shard1_consistent &&
      parse_surface.typed_sema_advanced_integration_shard1_ready &&
      typed_surface.typed_advanced_integration_shard1_consistent &&
      typed_surface.typed_advanced_integration_shard1_ready;
  surface.advanced_integration_shard1_consistent =
      surface.core_feature_advanced_conformance_shard1_ready &&
      surface.pass_graph_advanced_integration_shard1_ready &&
      surface.parse_artifact_advanced_integration_shard1_consistent &&
      surface.typed_handoff_advanced_integration_shard1_consistent;
  surface.advanced_integration_shard1_key_transport_ready =
      !surface.pass_graph_advanced_integration_shard1_key.empty() &&
      !surface.parse_artifact_advanced_integration_shard1_key.empty() &&
      !surface.typed_handoff_advanced_integration_shard1_key.empty();
  surface.core_feature_advanced_integration_shard1_ready =
      surface.core_feature_advanced_conformance_shard1_ready &&
      surface.pass_graph_advanced_integration_shard1_ready &&
      surface.advanced_integration_shard1_consistent &&
      surface.advanced_integration_shard1_key_transport_ready;
  surface.advanced_integration_shard1_key =
      BuildObjc3IREmissionCoreFeatureAdvancedIntegrationShard1Key(surface);
}

}  // namespace objc3_ir_emission_core_feature_surface
