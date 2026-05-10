#pragma once

#include "lower/model/lowered_module_diagnostics_surfaces.h"
#include "lower/model/lowered_module_final_readiness_surface.h"
#include "lower/model/lowered_module_pipeline_surfaces.h"
#include "lower/model/lowered_module_runtime_surfaces.h"
#include "lower/model/lowered_owner_contracts.h"

inline Objc3LoweringOwnerContractRecord
Objc3LoweringRuntimeStabilityOwnerContractRecord(
    const Objc3LoweringRuntimeStabilityInvariantScaffold &surface) {
  const bool producer_ready =
      surface.typed_surface_present && surface.parse_readiness_surface_present;
  const bool consumer_ready =
      surface.lowering_boundary_ready && surface.parse_ready_for_lowering &&
      surface.runtime_dispatch_contract_consistent;
  const bool replay_key_deterministic =
      surface.typed_handoff_key_deterministic &&
      !surface.lowering_boundary_replay_key.empty() &&
      !surface.typed_handoff_key.empty() &&
      !surface.parse_artifact_replay_key.empty();
  const bool artifact_publication_ready =
      surface.invariant_proofs_ready && surface.modular_split_ready &&
      !surface.scaffold_key.empty();
  const bool fail_closed =
      surface.typed_core_feature_consistent && surface.failure_reason.empty();

  return BuildObjc3LoweringOwnerContractRecord(
      kObjc3LoweringOwnerRuntimeStabilityContractId,
      kObjc3LoweringOwnerRuntimeStability, surface.typed_handoff_key,
      surface.lowering_boundary_replay_key, surface.scaffold_key,
      producer_ready, consumer_ready, replay_key_deterministic,
      artifact_publication_ready, fail_closed, surface.failure_reason);
}

inline Objc3LoweringOwnerContractRecord
Objc3LoweringPipelinePassGraphOwnerContractRecord(
    const Objc3LoweringPipelinePassGraphScaffold &surface) {
  const bool producer_ready =
      surface.lex_stage_ready && surface.parse_stage_ready &&
      surface.sema_stage_ready && surface.typed_surface_ready &&
      surface.parse_lowering_readiness_ready;
  const bool consumer_ready =
      surface.lowering_ir_boundary_ready &&
      surface.runtime_dispatch_declaration_ready &&
      surface.ir_emission_entrypoint_ready;
  const bool replay_key_deterministic =
      !surface.lowering_boundary_replay_key.empty() &&
      !surface.runtime_dispatch_declaration_replay_key.empty() &&
      !surface.pass_graph_key.empty();
  const bool artifact_publication_ready =
      surface.semantic_stability_scaffold_ready &&
      surface.lowering_runtime_invariant_scaffold_ready &&
      surface.pass_graph_ready;
  const bool fail_closed = surface.failure_reason.empty();

  return BuildObjc3LoweringOwnerContractRecord(
      kObjc3LoweringOwnerPipelinePassGraphContractId,
      kObjc3LoweringOwnerPipelinePassGraph,
      surface.runtime_dispatch_declaration_replay_key,
      surface.lowering_boundary_replay_key, surface.pass_graph_key,
      producer_ready, consumer_ready, replay_key_deterministic,
      artifact_publication_ready, fail_closed, surface.failure_reason);
}

inline Objc3LoweringOwnerContractRecord
Objc3LoweringPipelinePassGraphCoreFeatureOwnerContractRecord(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface) {
  const bool producer_ready =
      surface.scaffold_ready &&
      surface.lowering_boundary_replay_key_consistent &&
      surface.runtime_dispatch_declaration_consistent;
  const bool consumer_ready =
      surface.direct_ir_entrypoint_enabled &&
      surface.dispatch_shape_sharding_ready &&
      surface.llc_object_emission_route_deterministic;
  const bool replay_key_deterministic =
      surface.replay_proof_artifact_key_ready &&
      !surface.lowering_boundary_replay_key.empty() &&
      !surface.runtime_dispatch_declaration_replay_key.empty() &&
      !surface.core_feature_key.empty();
  const bool artifact_publication_ready =
      surface.expansion_ready && surface.core_feature_ready &&
      surface.edge_case_dispatch_shape_coverage_ready &&
      surface.replay_proof_expansion_ready;
  const bool fail_closed =
      surface.compatibility_handoff_consistent &&
      surface.language_version_pragma_coordinate_order_consistent &&
      surface.failure_reason.empty();

  return BuildObjc3LoweringOwnerContractRecord(
      kObjc3LoweringOwnerPassGraphCoreFeatureContractId,
      kObjc3LoweringOwnerPassGraphCoreFeature,
      surface.runtime_dispatch_declaration_replay_key,
      surface.lowering_boundary_replay_key, surface.core_feature_key,
      producer_ready, consumer_ready, replay_key_deterministic,
      artifact_publication_ready, fail_closed, surface.failure_reason);
}

inline Objc3LoweringOwnerContractRecord
Objc3IREmissionCompletenessOwnerContractRecord(
    const Objc3IREmissionCompletenessScaffold &surface) {
  const bool producer_ready =
      surface.pass_graph_scaffold_ready && surface.core_feature_ready &&
      surface.expansion_ready;
  const bool consumer_ready =
      surface.edge_case_compatibility_ready && surface.metadata_transport_ready;
  const bool replay_key_deterministic =
      !surface.pass_graph_key.empty() && !surface.core_feature_key.empty() &&
      !surface.expansion_key.empty() &&
      !surface.edge_case_compatibility_key.empty() &&
      !surface.scaffold_key.empty();
  const bool artifact_publication_ready =
      surface.modular_split_ready && replay_key_deterministic;
  const bool fail_closed = surface.failure_reason.empty();

  return BuildObjc3LoweringOwnerContractRecord(
      kObjc3LoweringOwnerIREmissionCompletenessContractId,
      kObjc3LoweringOwnerIREmissionCompleteness, surface.pass_graph_key,
      surface.edge_case_compatibility_key, surface.scaffold_key,
      producer_ready, consumer_ready, replay_key_deterministic,
      artifact_publication_ready, fail_closed, surface.failure_reason);
}

inline Objc3LoweringOwnerContractRecord
Objc3LoweringRuntimeDiagnosticsSurfacingOwnerContractRecord(
    const Objc3LoweringRuntimeDiagnosticsSurfacingScaffold &surface) {
  const bool producer_ready =
      surface.stage_diagnostics_bus_consistent &&
      surface.parse_readiness_surface_present &&
      surface.parse_readiness_surface_ready &&
      surface.parse_diagnostics_hardening_consistent;
  const bool consumer_ready =
      surface.parser_source_precision_scaffold_ready &&
      surface.diagnostics_replay_key_ready;
  const bool replay_key_deterministic =
      surface.typed_handoff_key_deterministic &&
      !surface.parse_artifact_replay_key.empty() &&
      !surface.lowering_boundary_replay_key.empty() &&
      !surface.scaffold_key.empty();
  const bool artifact_publication_ready =
      surface.modular_split_ready && replay_key_deterministic;
  const bool fail_closed = surface.failure_reason.empty();

  return BuildObjc3LoweringOwnerContractRecord(
      kObjc3LoweringOwnerDiagnosticsSurfacingContractId,
      kObjc3LoweringOwnerDiagnosticsSurfacing,
      surface.parse_artifact_replay_key, surface.lowering_boundary_replay_key,
      surface.scaffold_key, producer_ready, consumer_ready,
      replay_key_deterministic, artifact_publication_ready, fail_closed,
      surface.failure_reason);
}

inline Objc3LoweringOwnerContractRecord
Objc3ToolchainRuntimeGaOperationsOwnerContractRecord(
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureExpansionSurface
        &surface) {
  const bool producer_ready =
      surface.scaffold_ready && surface.backend_route_deterministic &&
      surface.compile_status_success;
  const bool consumer_ready =
      surface.backend_dispatch_consistent &&
      surface.backend_output_path_deterministic &&
      surface.backend_output_payload_consistent;
  const bool replay_key_deterministic =
      !surface.backend_route_key.empty() && !surface.scaffold_key.empty() &&
      !surface.core_feature_key.empty() &&
      !surface.core_feature_expansion_key.empty();
  const bool artifact_publication_ready =
      surface.backend_output_recorded && surface.core_feature_expansion_ready &&
      surface.core_feature_impl_ready;
  const bool fail_closed =
      surface.edge_case_compatibility_consistent &&
      surface.edge_case_compatibility_ready &&
      surface.failure_reason.empty();

  return BuildObjc3LoweringOwnerContractRecord(
      kObjc3LoweringOwnerToolchainGaContractId,
      kObjc3LoweringOwnerToolchainGa, surface.backend_route_key,
      surface.core_feature_key, surface.core_feature_expansion_key,
      producer_ready, consumer_ready, replay_key_deterministic,
      artifact_publication_ready, fail_closed, surface.failure_reason);
}

inline Objc3LoweringOwnerContractRecord
Objc3FinalReadinessGateOwnerContractRecord(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface) {
  const bool producer_ready =
      surface.governance_contract_ready && surface.modular_split_ready;
  const bool consumer_ready =
      surface.lane_a_core_feature_ready && surface.lane_b_core_feature_ready &&
      surface.lane_c_core_feature_ready && surface.lane_d_core_feature_ready &&
      surface.dependency_chain_ready;
  const bool replay_key_deterministic =
      !surface.governance_key.empty() && !surface.modular_split_key.empty() &&
      !surface.lane_a_key.empty() && !surface.lane_b_key.empty() &&
      !surface.lane_c_key.empty() && !surface.lane_d_key.empty() &&
      !surface.core_feature_key.empty();
  const bool artifact_publication_ready =
      surface.integration_closeout_signoff_consistent &&
      surface.integration_closeout_signoff_ready &&
      !surface.integration_closeout_signoff_key.empty() &&
      surface.core_feature_impl_ready;
  const bool fail_closed =
      surface.cross_lane_integration_consistent &&
      surface.cross_lane_integration_ready && surface.failure_reason.empty();

  return BuildObjc3LoweringOwnerContractRecord(
      kObjc3LoweringOwnerFinalReadinessGateContractId,
      kObjc3LoweringOwnerFinalReadinessGate, surface.governance_key,
      surface.core_feature_key, surface.integration_closeout_signoff_key,
      producer_ready, consumer_ready, replay_key_deterministic,
      artifact_publication_ready, fail_closed, surface.failure_reason);
}
