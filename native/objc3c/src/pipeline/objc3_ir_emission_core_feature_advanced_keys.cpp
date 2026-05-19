#include "pipeline/objc3_ir_emission_core_feature_implementation_surface.h"

#include <sstream>

std::string BuildObjc3IREmissionCoreFeatureAdvancedCoreShard1Key(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "ir-emission-core-feature-advanced-core-shard1:v1:"
      << "cross-lane-integration-sync-ready="
      << (surface.core_feature_cross_lane_integration_sync_ready ? "true"
                                                                 : "false")
      << ";pass-graph-advanced-core-shard1-ready="
      << (surface.pass_graph_advanced_core_shard1_ready ? "true" : "false")
      << ";parse-artifact-advanced-core-shard1-consistent="
      << (surface.parse_artifact_advanced_core_shard1_consistent ? "true"
                                                                 : "false")
      << ";typed-handoff-advanced-core-shard1-consistent="
      << (surface.typed_handoff_advanced_core_shard1_consistent ? "true"
                                                                : "false")
      << ";advanced-core-shard1-consistent="
      << (surface.advanced_core_shard1_consistent ? "true" : "false")
      << ";advanced-core-shard1-key-transport-ready="
      << (surface.advanced_core_shard1_key_transport_ready ? "true" : "false")
      << ";advanced-core-shard1-ready="
      << (surface.core_feature_advanced_core_shard1_ready ? "true" : "false")
      << ";pass-graph-advanced-core-shard1-key="
      << surface.pass_graph_advanced_core_shard1_key
      << ";parse-artifact-advanced-core-shard1-key="
      << surface.parse_artifact_advanced_core_shard1_key
      << ";typed-handoff-advanced-core-shard1-key="
      << surface.typed_handoff_advanced_core_shard1_key;
  return key.str();
}

std::string BuildObjc3IREmissionCoreFeatureAdvancedEdgeCompatibilityShard1Key(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "ir-emission-core-feature-advanced-edge-compatibility-shard1:v1:"
      << "advanced-core-shard1-ready="
      << (surface.core_feature_advanced_core_shard1_ready ? "true" : "false")
      << ";pass-graph-advanced-edge-compatibility-shard1-ready="
      << (surface.pass_graph_advanced_edge_compatibility_shard1_ready ? "true"
                                                                       : "false")
      << ";parse-artifact-advanced-edge-compatibility-shard1-consistent="
      << (surface.parse_artifact_advanced_edge_compatibility_shard1_consistent
              ? "true"
              : "false")
      << ";typed-handoff-advanced-edge-compatibility-shard1-consistent="
      << (surface.typed_handoff_advanced_edge_compatibility_shard1_consistent
              ? "true"
              : "false")
      << ";advanced-edge-compatibility-shard1-consistent="
      << (surface.advanced_edge_compatibility_shard1_consistent ? "true"
                                                                : "false")
      << ";advanced-edge-compatibility-shard1-key-transport-ready="
      << (surface.advanced_edge_compatibility_shard1_key_transport_ready
              ? "true"
              : "false")
      << ";advanced-edge-compatibility-shard1-ready="
      << (surface.core_feature_advanced_edge_compatibility_shard1_ready ? "true"
                                                                         : "false")
      << ";pass-graph-advanced-edge-compatibility-shard1-key="
      << surface.pass_graph_advanced_edge_compatibility_shard1_key
      << ";parse-artifact-advanced-edge-compatibility-shard1-key="
      << surface.parse_artifact_advanced_edge_compatibility_shard1_key
      << ";typed-handoff-advanced-edge-compatibility-shard1-key="
      << surface.typed_handoff_advanced_edge_compatibility_shard1_key;
  return key.str();
}

std::string BuildObjc3IREmissionCoreFeatureAdvancedDiagnosticsShard1Key(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "ir-emission-core-feature-advanced-diagnostics-shard1:v1:"
      << "advanced-edge-compatibility-shard1-ready="
      << (surface.core_feature_advanced_edge_compatibility_shard1_ready ? "true"
                                                                         : "false")
      << ";pass-graph-advanced-diagnostics-shard1-ready="
      << (surface.pass_graph_advanced_diagnostics_shard1_ready ? "true"
                                                               : "false")
      << ";parse-artifact-advanced-diagnostics-shard1-consistent="
      << (surface.parse_artifact_advanced_diagnostics_shard1_consistent ? "true"
                                                                        : "false")
      << ";typed-handoff-advanced-diagnostics-shard1-consistent="
      << (surface.typed_handoff_advanced_diagnostics_shard1_consistent ? "true"
                                                                       : "false")
      << ";advanced-diagnostics-shard1-consistent="
      << (surface.advanced_diagnostics_shard1_consistent ? "true" : "false")
      << ";advanced-diagnostics-shard1-key-transport-ready="
      << (surface.advanced_diagnostics_shard1_key_transport_ready ? "true"
                                                                  : "false")
      << ";advanced-diagnostics-shard1-ready="
      << (surface.core_feature_advanced_diagnostics_shard1_ready ? "true"
                                                                 : "false")
      << ";pass-graph-advanced-diagnostics-shard1-key="
      << surface.pass_graph_advanced_diagnostics_shard1_key
      << ";parse-artifact-advanced-diagnostics-shard1-key="
      << surface.parse_artifact_advanced_diagnostics_shard1_key
      << ";typed-handoff-advanced-diagnostics-shard1-key="
      << surface.typed_handoff_advanced_diagnostics_shard1_key;
  return key.str();
}

std::string BuildObjc3IREmissionCoreFeatureAdvancedConformanceShard1Key(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "ir-emission-core-feature-advanced-conformance-shard1:v1:"
      << "advanced-diagnostics-shard1-ready="
      << (surface.core_feature_advanced_diagnostics_shard1_ready ? "true"
                                                                 : "false")
      << ";pass-graph-advanced-conformance-shard1-ready="
      << (surface.pass_graph_advanced_conformance_shard1_ready ? "true"
                                                               : "false")
      << ";parse-artifact-advanced-conformance-shard1-consistent="
      << (surface.parse_artifact_advanced_conformance_shard1_consistent ? "true"
                                                                        : "false")
      << ";typed-handoff-advanced-conformance-shard1-consistent="
      << (surface.typed_handoff_advanced_conformance_shard1_consistent ? "true"
                                                                       : "false")
      << ";advanced-conformance-shard1-consistent="
      << (surface.advanced_conformance_shard1_consistent ? "true" : "false")
      << ";advanced-conformance-shard1-key-transport-ready="
      << (surface.advanced_conformance_shard1_key_transport_ready ? "true"
                                                                  : "false")
      << ";advanced-conformance-shard1-ready="
      << (surface.core_feature_advanced_conformance_shard1_ready ? "true"
                                                                 : "false")
      << ";pass-graph-advanced-conformance-shard1-key="
      << surface.pass_graph_advanced_conformance_shard1_key
      << ";parse-artifact-advanced-conformance-shard1-key="
      << surface.parse_artifact_advanced_conformance_shard1_key
      << ";typed-handoff-advanced-conformance-shard1-key="
      << surface.typed_handoff_advanced_conformance_shard1_key;
  return key.str();
}

std::string BuildObjc3IREmissionCoreFeatureAdvancedIntegrationShard1Key(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "ir-emission-core-feature-advanced-integration-shard1:v1:"
      << "advanced-conformance-shard1-ready="
      << (surface.core_feature_advanced_conformance_shard1_ready ? "true"
                                                                 : "false")
      << ";pass-graph-advanced-integration-shard1-ready="
      << (surface.pass_graph_advanced_integration_shard1_ready ? "true"
                                                               : "false")
      << ";parse-artifact-advanced-integration-shard1-consistent="
      << (surface.parse_artifact_advanced_integration_shard1_consistent ? "true"
                                                                        : "false")
      << ";typed-handoff-advanced-integration-shard1-consistent="
      << (surface.typed_handoff_advanced_integration_shard1_consistent ? "true"
                                                                       : "false")
      << ";advanced-integration-shard1-consistent="
      << (surface.advanced_integration_shard1_consistent ? "true" : "false")
      << ";advanced-integration-shard1-key-transport-ready="
      << (surface.advanced_integration_shard1_key_transport_ready ? "true"
                                                                  : "false")
      << ";advanced-integration-shard1-ready="
      << (surface.core_feature_advanced_integration_shard1_ready ? "true"
                                                                 : "false")
      << ";pass-graph-advanced-integration-shard1-key="
      << surface.pass_graph_advanced_integration_shard1_key
      << ";parse-artifact-advanced-integration-shard1-key="
      << surface.parse_artifact_advanced_integration_shard1_key
      << ";typed-handoff-advanced-integration-shard1-key="
      << surface.typed_handoff_advanced_integration_shard1_key;
  return key.str();
}
