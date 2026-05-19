#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

inline std::string BuildObjc3TypedSemaToLoweringAdvancedCoreShard1Key(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-advanced-core-shard1:v1:typed_release_candidate_replay_dry_run_ready=" +
         std::string(surface.typed_release_candidate_replay_dry_run_ready ? "true" : "false") +
         ";typed_advanced_core_shard1_consistent=" +
         std::string(surface.typed_advanced_core_shard1_consistent ? "true" : "false") +
         ";typed_advanced_core_shard1_ready=" +
         std::string(surface.typed_advanced_core_shard1_ready ? "true" : "false") +
         ";typed_release_candidate_replay_dry_run_key=" +
         surface.typed_release_candidate_replay_dry_run_key;
}

inline std::string BuildObjc3TypedSemaToLoweringAdvancedEdgeCompatibilityShard1Key(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-advanced-edge-compatibility-shard1:v1:typed_advanced_core_shard1_ready=" +
         std::string(surface.typed_advanced_core_shard1_ready ? "true" : "false") +
         ";typed_advanced_edge_compatibility_shard1_consistent=" +
         std::string(surface.typed_advanced_edge_compatibility_shard1_consistent ? "true" : "false") +
         ";typed_advanced_edge_compatibility_shard1_ready=" +
         std::string(surface.typed_advanced_edge_compatibility_shard1_ready ? "true" : "false") +
         ";typed_advanced_core_shard1_key=" + surface.typed_advanced_core_shard1_key;
}

inline std::string BuildObjc3TypedSemaToLoweringAdvancedDiagnosticsShard1Key(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-advanced-diagnostics-shard1:v1:typed_advanced_edge_compatibility_shard1_ready=" +
         std::string(surface.typed_advanced_edge_compatibility_shard1_ready ? "true" : "false") +
         ";typed_advanced_diagnostics_shard1_consistent=" +
         std::string(surface.typed_advanced_diagnostics_shard1_consistent ? "true" : "false") +
         ";typed_advanced_diagnostics_shard1_ready=" +
         std::string(surface.typed_advanced_diagnostics_shard1_ready ? "true" : "false") +
         ";typed_advanced_edge_compatibility_shard1_key=" + surface.typed_advanced_edge_compatibility_shard1_key;
}

inline std::string BuildObjc3TypedSemaToLoweringAdvancedConformanceShard1Key(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-advanced-conformance-shard1:v1:typed_advanced_diagnostics_shard1_ready=" +
         std::string(surface.typed_advanced_diagnostics_shard1_ready ? "true" : "false") +
         ";typed_advanced_conformance_shard1_consistent=" +
         std::string(surface.typed_advanced_conformance_shard1_consistent ? "true" : "false") +
         ";typed_advanced_conformance_shard1_ready=" +
         std::string(surface.typed_advanced_conformance_shard1_ready ? "true" : "false") +
         ";typed_advanced_diagnostics_shard1_key=" + surface.typed_advanced_diagnostics_shard1_key;
}

inline std::string BuildObjc3TypedSemaToLoweringAdvancedIntegrationShard1Key(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-advanced-integration-shard1:v1:typed_advanced_conformance_shard1_ready=" +
         std::string(surface.typed_advanced_conformance_shard1_ready ? "true" : "false") +
         ";typed_advanced_integration_shard1_consistent=" +
         std::string(surface.typed_advanced_integration_shard1_consistent ? "true" : "false") +
         ";typed_advanced_integration_shard1_ready=" +
         std::string(surface.typed_advanced_integration_shard1_ready ? "true" : "false") +
         ";typed_advanced_conformance_shard1_key=" + surface.typed_advanced_conformance_shard1_key;
}

inline std::string BuildObjc3TypedSemaToLoweringAdvancedPerformanceShard1Key(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-advanced-performance-shard1:v1:typed_advanced_integration_shard1_ready=" +
         std::string(surface.typed_advanced_integration_shard1_ready ? "true" : "false") +
         ";typed_advanced_performance_shard1_consistent=" +
         std::string(surface.typed_advanced_performance_shard1_consistent ? "true" : "false") +
         ";typed_advanced_performance_shard1_ready=" +
         std::string(surface.typed_advanced_performance_shard1_ready ? "true" : "false") +
         ";typed_advanced_integration_shard1_key=" + surface.typed_advanced_integration_shard1_key;
}

inline std::string BuildObjc3TypedSemaToLoweringAdvancedCoreShard2Key(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-advanced-core-shard2:v1:typed_advanced_performance_shard1_ready=" +
         std::string(surface.typed_advanced_performance_shard1_ready ? "true" : "false") +
         ";typed_advanced_core_shard2_consistent=" +
         std::string(surface.typed_advanced_core_shard2_consistent ? "true" : "false") +
         ";typed_advanced_core_shard2_ready=" +
         std::string(surface.typed_advanced_core_shard2_ready ? "true" : "false") +
         ";typed_advanced_performance_shard1_key=" + surface.typed_advanced_performance_shard1_key;
}

inline std::string BuildObjc3TypedSemaToLoweringAdvancedEdgeCompatibilityShard2Key(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-advanced-edge-compatibility-shard2:v1:typed_advanced_core_shard2_ready=" +
         std::string(surface.typed_advanced_core_shard2_ready ? "true" : "false") +
         ";typed_advanced_edge_compatibility_shard2_consistent=" +
         std::string(surface.typed_advanced_edge_compatibility_shard2_consistent ? "true" : "false") +
         ";typed_advanced_edge_compatibility_shard2_ready=" +
         std::string(surface.typed_advanced_edge_compatibility_shard2_ready ? "true" : "false") +
         ";typed_advanced_core_shard2_key=" + surface.typed_advanced_core_shard2_key;
}

inline std::string BuildObjc3TypedSemaToLoweringAdvancedDiagnosticsShard2Key(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-advanced-diagnostics-shard2:v1:typed_advanced_edge_compatibility_shard2_ready=" +
         std::string(surface.typed_advanced_edge_compatibility_shard2_ready ? "true" : "false") +
         ";typed_advanced_diagnostics_shard2_consistent=" +
         std::string(surface.typed_advanced_diagnostics_shard2_consistent ? "true" : "false") +
         ";typed_advanced_diagnostics_shard2_ready=" +
         std::string(surface.typed_advanced_diagnostics_shard2_ready ? "true" : "false") +
         ";typed_advanced_edge_compatibility_shard2_key=" + surface.typed_advanced_edge_compatibility_shard2_key;
}

inline std::string BuildObjc3TypedSemaToLoweringAdvancedConformanceShard2Key(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-advanced-conformance-shard2:v1:typed_advanced_diagnostics_shard2_ready=" +
         std::string(surface.typed_advanced_diagnostics_shard2_ready ? "true" : "false") +
         ";typed_advanced_conformance_shard2_consistent=" +
         std::string(surface.typed_advanced_conformance_shard2_consistent ? "true" : "false") +
         ";typed_advanced_conformance_shard2_ready=" +
         std::string(surface.typed_advanced_conformance_shard2_ready ? "true" : "false") +
         ";typed_advanced_diagnostics_shard2_key=" + surface.typed_advanced_diagnostics_shard2_key;
}

inline std::string BuildObjc3TypedSemaToLoweringAdvancedIntegrationShard2Key(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-advanced-integration-shard2:v1:typed_advanced_conformance_shard2_ready=" +
         std::string(surface.typed_advanced_conformance_shard2_ready ? "true" : "false") +
         ";typed_advanced_integration_shard2_consistent=" +
         std::string(surface.typed_advanced_integration_shard2_consistent ? "true" : "false") +
         ";typed_advanced_integration_shard2_ready=" +
         std::string(surface.typed_advanced_integration_shard2_ready ? "true" : "false") +
         ";typed_advanced_conformance_shard2_key=" + surface.typed_advanced_conformance_shard2_key;
}

inline std::string BuildObjc3TypedSemaToLoweringIntegrationCloseoutSignoffKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-integration-closeout-signoff:v1:typed_advanced_integration_shard2_ready=" +
         std::string(surface.typed_advanced_integration_shard2_ready ? "true" : "false") +
         ";typed_integration_closeout_signoff_consistent=" +
         std::string(surface.typed_integration_closeout_signoff_consistent ? "true" : "false") +
         ";typed_integration_closeout_signoff_ready=" +
         std::string(surface.typed_integration_closeout_signoff_ready ? "true" : "false") +
         ";typed_advanced_integration_shard2_key=" + surface.typed_advanced_integration_shard2_key;
}
