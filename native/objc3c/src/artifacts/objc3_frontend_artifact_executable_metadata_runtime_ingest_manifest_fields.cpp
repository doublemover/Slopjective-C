#include "artifacts/objc3_frontend_artifact_executable_metadata_runtime_ingest_manifest_fields.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_runtime_metadata_plan.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {

using objc3::io::EscapeJsonString;

void AppendObjc3FrontendArtifactExecutableMetadataRuntimeIngestManifestFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactRuntimeMetadataPlan &runtime_metadata_plan) {
  const Objc3ExecutableMetadataDebugProjectionSummary
      &executable_metadata_debug_projection =
          runtime_metadata_plan.executable_metadata_debug_projection;
  const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
      &executable_metadata_runtime_ingest_packaging_contract =
          runtime_metadata_plan
              .executable_metadata_runtime_ingest_packaging_contract;
  const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary
      &executable_metadata_runtime_ingest_binary_boundary =
          runtime_metadata_plan
              .executable_metadata_runtime_ingest_binary_boundary;

  manifest
      << ",\"executable_metadata_debug_projection_contract_id\":\""
      << EscapeJsonString(executable_metadata_debug_projection.contract_id)
      << "\",\"executable_metadata_debug_projection_typed_handoff_contract_id\":\""
      << EscapeJsonString(
             executable_metadata_debug_projection
                 .typed_lowering_handoff_contract_id)
      << "\",\"executable_metadata_debug_projection_source_graph_contract_id\":\""
      << EscapeJsonString(
             executable_metadata_debug_projection.source_graph_contract_id)
      << "\",\"executable_metadata_debug_projection_named_metadata_name\":\""
      << EscapeJsonString(executable_metadata_debug_projection.named_metadata_name)
      << "\",\"executable_metadata_debug_projection_manifest_surface_path\":\""
      << EscapeJsonString(
             executable_metadata_debug_projection.manifest_surface_path)
      << "\",\"executable_metadata_debug_projection_typed_handoff_surface_path\":\""
      << EscapeJsonString(
             executable_metadata_debug_projection.typed_handoff_surface_path)
      << "\",\"executable_metadata_debug_projection_source_graph_surface_path\":\""
      << EscapeJsonString(
             executable_metadata_debug_projection.source_graph_surface_path)
      << "\",\"executable_metadata_debug_projection_matrix_published\":"
      << (executable_metadata_debug_projection.matrix_published ? "true"
                                                                : "false")
      << ",\"executable_metadata_debug_projection_fail_closed\":"
      << (executable_metadata_debug_projection.fail_closed ? "true" : "false")
      << ",\"executable_metadata_debug_projection_manifest_debug_surface_published\":"
      << (executable_metadata_debug_projection.manifest_debug_surface_published
              ? "true"
              : "false")
      << ",\"executable_metadata_debug_projection_ir_named_metadata_published\":"
      << (executable_metadata_debug_projection.ir_named_metadata_published
              ? "true"
              : "false")
      << ",\"executable_metadata_debug_projection_replay_anchor_deterministic\":"
      << (executable_metadata_debug_projection.replay_anchor_deterministic
              ? "true"
              : "false")
      << ",\"executable_metadata_debug_projection_active_typed_handoff_ready\":"
      << (executable_metadata_debug_projection.active_typed_handoff_ready
              ? "true"
              : "false")
      << ",\"executable_metadata_debug_projection_matrix_row_count\":"
      << executable_metadata_debug_projection.matrix_row_count
      << ",\"executable_metadata_debug_projection_replay_key\":\""
      << EscapeJsonString(executable_metadata_debug_projection.replay_key)
      << "\",\"executable_metadata_debug_projection_active_typed_handoff_replay_key\":\""
      << EscapeJsonString(
             executable_metadata_debug_projection
                 .active_typed_handoff_replay_key)
      << "\",\"executable_metadata_debug_projection_failure_reason\":\""
      << EscapeJsonString(executable_metadata_debug_projection.failure_reason)
      << "\",\"executable_metadata_runtime_ingest_packaging_contract_id\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_packaging_contract.contract_id)
      << "\",\"executable_metadata_runtime_ingest_packaging_typed_handoff_contract_id\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_packaging_contract
                 .typed_lowering_handoff_contract_id)
      << "\",\"executable_metadata_runtime_ingest_packaging_debug_projection_contract_id\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_packaging_contract
                 .debug_projection_contract_id)
      << "\",\"executable_metadata_runtime_ingest_packaging_surface_path\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_packaging_contract
                 .packaging_surface_path)
      << "\",\"executable_metadata_runtime_ingest_packaging_typed_handoff_surface_path\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_packaging_contract
                 .typed_handoff_surface_path)
      << "\",\"executable_metadata_runtime_ingest_packaging_debug_projection_surface_path\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_packaging_contract
                 .debug_projection_surface_path)
      << "\",\"executable_metadata_runtime_ingest_packaging_payload_model\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_packaging_contract
                 .packaging_payload_model)
      << "\",\"executable_metadata_runtime_ingest_packaging_transport_artifact_relative_path\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_packaging_contract
                 .transport_artifact_relative_path)
      << "\",\"executable_metadata_runtime_ingest_packaging_boundary_frozen\":"
      << (executable_metadata_runtime_ingest_packaging_contract.boundary_frozen
              ? "true"
              : "false")
      << ",\"executable_metadata_runtime_ingest_packaging_fail_closed\":"
      << (executable_metadata_runtime_ingest_packaging_contract.fail_closed
              ? "true"
              : "false")
      << ",\"executable_metadata_runtime_ingest_packaging_typed_handoff_ready\":"
      << (executable_metadata_runtime_ingest_packaging_contract
                  .typed_lowering_handoff_ready
              ? "true"
              : "false")
      << ",\"executable_metadata_runtime_ingest_packaging_debug_projection_ready\":"
      << (executable_metadata_runtime_ingest_packaging_contract
                  .debug_projection_ready
              ? "true"
              : "false")
      << ",\"executable_metadata_runtime_ingest_packaging_manifest_transport_frozen\":"
      << (executable_metadata_runtime_ingest_packaging_contract
                  .manifest_transport_frozen
              ? "true"
              : "false")
      << ",\"executable_metadata_runtime_ingest_packaging_runtime_section_emission_not_yet_landed\":"
      << (executable_metadata_runtime_ingest_packaging_contract
                  .runtime_section_emission_not_yet_landed
              ? "true"
              : "false")
      << ",\"executable_metadata_runtime_ingest_packaging_startup_registration_not_yet_landed\":"
      << (executable_metadata_runtime_ingest_packaging_contract
                  .startup_registration_not_yet_landed
              ? "true"
              : "false")
      << ",\"executable_metadata_runtime_ingest_packaging_runtime_loader_registration_not_yet_landed\":"
      << (executable_metadata_runtime_ingest_packaging_contract
                  .runtime_loader_registration_not_yet_landed
              ? "true"
              : "false")
      << ",\"executable_metadata_runtime_ingest_packaging_explicit_non_goals_published\":"
      << (executable_metadata_runtime_ingest_packaging_contract
                  .explicit_non_goals_published
              ? "true"
              : "false")
      << ",\"executable_metadata_runtime_ingest_packaging_ready_for_packaging_implementation\":"
      << (executable_metadata_runtime_ingest_packaging_contract
                  .ready_for_packaging_implementation
              ? "true"
              : "false")
      << ",\"executable_metadata_runtime_ingest_packaging_typed_handoff_replay_key\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_packaging_contract
                 .typed_lowering_handoff_replay_key)
      << "\",\"executable_metadata_runtime_ingest_packaging_debug_projection_replay_key\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_packaging_contract
                 .debug_projection_replay_key)
      << "\",\"executable_metadata_runtime_ingest_packaging_replay_key\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_packaging_contract.replay_key)
      << "\",\"executable_metadata_runtime_ingest_packaging_failure_reason\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_packaging_contract.failure_reason)
      << "\",\"executable_metadata_runtime_ingest_binary_boundary_contract_id\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_binary_boundary.contract_id)
      << "\",\"executable_metadata_runtime_ingest_binary_boundary_packaging_contract_id\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_binary_boundary
                 .packaging_contract_id)
      << "\",\"executable_metadata_runtime_ingest_binary_boundary_surface_path\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_binary_boundary
                 .binary_boundary_surface_path)
      << "\",\"executable_metadata_runtime_ingest_binary_boundary_payload_model\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_binary_boundary.payload_model)
      << "\",\"executable_metadata_runtime_ingest_binary_boundary_envelope_format\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_binary_boundary.envelope_format)
      << "\",\"executable_metadata_runtime_ingest_binary_boundary_artifact_relative_path\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_binary_boundary
                 .artifact_relative_path)
      << "\",\"executable_metadata_runtime_ingest_binary_boundary_artifact_suffix\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_binary_boundary.artifact_suffix)
      << "\",\"executable_metadata_runtime_ingest_binary_boundary_magic\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_binary_boundary.binary_magic)
      << "\",\"executable_metadata_runtime_ingest_binary_boundary_chunk_count\":"
      << executable_metadata_runtime_ingest_binary_boundary.chunk_count
      << ",\"executable_metadata_runtime_ingest_binary_boundary_fail_closed\":"
      << (executable_metadata_runtime_ingest_binary_boundary.fail_closed
              ? "true"
              : "false")
      << ",\"executable_metadata_runtime_ingest_binary_boundary_binary_payload_present\":"
      << (executable_metadata_runtime_ingest_binary_boundary
                  .binary_payload_present
              ? "true"
              : "false")
      << ",\"executable_metadata_runtime_ingest_binary_boundary_binary_envelope_deterministic\":"
      << (executable_metadata_runtime_ingest_binary_boundary
                  .binary_envelope_deterministic
              ? "true"
              : "false")
      << ",\"executable_metadata_runtime_ingest_binary_boundary_ready_for_section_emission_handoff\":"
      << (executable_metadata_runtime_ingest_binary_boundary
                  .ready_for_section_emission_handoff
              ? "true"
              : "false")
      << ",\"executable_metadata_runtime_ingest_binary_boundary_payload_bytes\":"
      << executable_metadata_runtime_ingest_binary_boundary.payload_bytes
      << ",\"executable_metadata_runtime_ingest_binary_boundary_packaging_contract_replay_key\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_binary_boundary
                 .packaging_contract_replay_key)
      << "\",\"executable_metadata_runtime_ingest_binary_boundary_typed_handoff_replay_key\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_binary_boundary
                 .typed_lowering_handoff_replay_key)
      << "\",\"executable_metadata_runtime_ingest_binary_boundary_debug_projection_replay_key\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_binary_boundary
                 .debug_projection_replay_key)
      << "\",\"executable_metadata_runtime_ingest_binary_boundary_replay_key\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_binary_boundary.replay_key)
      << "\",\"executable_metadata_runtime_ingest_binary_boundary_failure_reason\":\""
      << EscapeJsonString(
             executable_metadata_runtime_ingest_binary_boundary.failure_reason)
      << "\"";
}

}  // namespace objc3::artifacts::frontend
