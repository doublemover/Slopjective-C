#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

#include "ast/objc3_ast_contracts_source_property_metadata.h"
#include "runtime/metadata/executable_metadata_debug_projection.h"
#include "sema/model/semantic_type_executable_metadata_contracts.h"

struct Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary {
  std::string contract_id =
      kObjc3ExecutableMetadataRuntimeIngestPackagingContractId;
  std::string typed_lowering_handoff_contract_id =
      kObjc3ExecutableMetadataTypedLoweringHandoffContractId;
  std::string debug_projection_contract_id =
      kObjc3ExecutableMetadataDebugProjectionContractId;
  std::string packaging_surface_path =
      kObjc3ExecutableMetadataRuntimeIngestPackagingSurfacePath;
  std::string typed_handoff_surface_path =
      kObjc3ExecutableMetadataTypedLoweringHandoffManifestSurfacePath;
  std::string debug_projection_surface_path =
      kObjc3ExecutableMetadataDebugProjectionManifestSurfacePath;
  std::string packaging_payload_model =
      kObjc3ExecutableMetadataRuntimeIngestPackagingPayloadModel;
  std::string transport_artifact_relative_path =
      kObjc3ExecutableMetadataRuntimeIngestPackagingTransportArtifact;
  bool boundary_frozen = false;
  bool fail_closed = false;
  bool typed_lowering_handoff_ready = false;
  bool debug_projection_ready = false;
  bool manifest_transport_frozen = false;
  bool runtime_section_emission_not_yet_landed = false;
  bool startup_registration_not_yet_landed = false;
  bool runtime_loader_registration_not_yet_landed = false;
  bool explicit_non_goals_published = false;
  bool ready_for_packaging_implementation = false;
  std::string typed_lowering_handoff_replay_key;
  std::string debug_projection_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataRuntimeIngestPackagingContractSummary(
    const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
        &summary) {
  return !summary.contract_id.empty() &&
         !summary.typed_lowering_handoff_contract_id.empty() &&
         !summary.debug_projection_contract_id.empty() &&
         !summary.packaging_surface_path.empty() &&
         !summary.typed_handoff_surface_path.empty() &&
         !summary.debug_projection_surface_path.empty() &&
         !summary.packaging_payload_model.empty() &&
         !summary.transport_artifact_relative_path.empty() &&
         summary.boundary_frozen && summary.fail_closed &&
         summary.typed_lowering_handoff_ready && summary.debug_projection_ready &&
         summary.manifest_transport_frozen &&
         summary.runtime_section_emission_not_yet_landed &&
         summary.startup_registration_not_yet_landed &&
         summary.runtime_loader_registration_not_yet_landed &&
         summary.explicit_non_goals_published &&
         summary.ready_for_packaging_implementation &&
         !summary.typed_lowering_handoff_replay_key.empty() &&
         !summary.debug_projection_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary {
  std::string contract_id =
      kObjc3ExecutableMetadataRuntimeIngestBinaryBoundaryContractId;
  std::string packaging_contract_id =
      kObjc3ExecutableMetadataRuntimeIngestPackagingContractId;
  std::string typed_lowering_handoff_contract_id =
      kObjc3ExecutableMetadataTypedLoweringHandoffContractId;
  std::string debug_projection_contract_id =
      kObjc3ExecutableMetadataDebugProjectionContractId;
  std::string packaging_surface_path =
      kObjc3ExecutableMetadataRuntimeIngestPackagingSurfacePath;
  std::string binary_boundary_surface_path =
      kObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySurfacePath;
  std::string payload_model =
      kObjc3ExecutableMetadataRuntimeIngestPackagingPayloadModel;
  std::string envelope_format =
      kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeFormat;
  std::string artifact_relative_path =
      kObjc3ExecutableMetadataRuntimeIngestBinaryArtifactRelativePath;
  std::string artifact_suffix =
      kObjc3ExecutableMetadataRuntimeIngestBinaryArtifactSuffix;
  std::string binary_magic = kObjc3ExecutableMetadataRuntimeIngestBinaryMagic;
  std::uint32_t envelope_version =
      kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeVersion;
  std::uint32_t chunk_count =
      kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeChunkCount;
  std::array<std::string, 3u> chunk_names = {
      kObjc3ExecutableMetadataRuntimeIngestBinaryPackagingChunkName,
      kObjc3ExecutableMetadataRuntimeIngestBinaryTypedHandoffChunkName,
      kObjc3ExecutableMetadataRuntimeIngestBinaryDebugProjectionChunkName};
  bool fail_closed = false;
  bool packaging_contract_ready = false;
  bool typed_lowering_handoff_ready = false;
  bool debug_projection_ready = false;
  bool binary_payload_present = false;
  bool binary_boundary_emitted = false;
  bool binary_envelope_deterministic = false;
  bool ready_for_section_emission_handoff = false;
  std::size_t payload_bytes = 0;
  std::string packaging_contract_replay_key;
  std::string typed_lowering_handoff_replay_key;
  std::string debug_projection_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary(
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary &summary) {
  if (summary.contract_id.empty() || summary.packaging_contract_id.empty() ||
      summary.typed_lowering_handoff_contract_id.empty() ||
      summary.debug_projection_contract_id.empty() ||
      summary.packaging_surface_path.empty() ||
      summary.binary_boundary_surface_path.empty() ||
      summary.payload_model.empty() || summary.envelope_format.empty() ||
      summary.artifact_relative_path.empty() || summary.artifact_suffix.empty() ||
      summary.binary_magic.empty() || summary.envelope_version == 0u ||
      summary.chunk_count != summary.chunk_names.size() || !summary.fail_closed ||
      !summary.packaging_contract_ready ||
      !summary.typed_lowering_handoff_ready ||
      !summary.debug_projection_ready || !summary.binary_payload_present ||
      !summary.binary_boundary_emitted ||
      !summary.binary_envelope_deterministic ||
      !summary.ready_for_section_emission_handoff || summary.payload_bytes == 0u ||
      summary.packaging_contract_replay_key.empty() ||
      summary.typed_lowering_handoff_replay_key.empty() ||
      summary.debug_projection_replay_key.empty() || summary.replay_key.empty() ||
      !summary.failure_reason.empty()) {
    return false;
  }
  for (const auto &chunk_name : summary.chunk_names) {
    if (chunk_name.empty()) {
      return false;
    }
  }
  return true;
}
