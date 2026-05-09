#include "artifacts/objc3_frontend_runtime_ingest_binary_artifacts.h"

#include <cstdint>
#include <sstream>
#include <string>

#include "ast/objc3_ast_contracts.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

void AppendRuntimeIngestBinaryEnvelopeU32(std::string &payload,
                                          std::uint32_t value) {
  for (std::uint32_t shift = 0; shift < 32u; shift += 8u) {
    payload.push_back(
        static_cast<char>((value >> shift) & static_cast<std::uint32_t>(0xFFu)));
  }
}

void AppendRuntimeIngestBinaryEnvelopeChunk(std::string &payload,
                                            const std::string &chunk_name,
                                            const std::string &chunk_payload) {
  AppendRuntimeIngestBinaryEnvelopeU32(
      payload, static_cast<std::uint32_t>(chunk_name.size()));
  payload.append(chunk_name);
  AppendRuntimeIngestBinaryEnvelopeU32(
      payload, static_cast<std::uint32_t>(chunk_payload.size()));
  payload.append(chunk_payload);
}

}  // namespace

std::string BuildExecutableMetadataRuntimeIngestPackagingReplayKey(
    const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";typed_contract=" << summary.typed_lowering_handoff_contract_id
      << ";debug_contract=" << summary.debug_projection_contract_id
      << ";packaging_surface_path=" << summary.packaging_surface_path
      << ";typed_handoff_surface_path=" << summary.typed_handoff_surface_path
      << ";debug_projection_surface_path="
      << summary.debug_projection_surface_path
      << ";payload_model=" << summary.packaging_payload_model
      << ";transport_artifact=" << summary.transport_artifact_relative_path
      << ";typed_replay=" << summary.typed_lowering_handoff_replay_key
      << ";debug_replay=" << summary.debug_projection_replay_key;
  return out.str();
}

Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
BuildExecutableMetadataRuntimeIngestPackagingContractSummary(
    const Objc3ExecutableMetadataTypedLoweringHandoff
        &executable_metadata_typed_lowering_handoff,
    const Objc3ExecutableMetadataDebugProjectionSummary
        &executable_metadata_debug_projection) {
  Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary summary;
  summary.boundary_frozen = true;
  summary.fail_closed = true;
  summary.manifest_transport_frozen = true;
  summary.runtime_section_emission_not_yet_landed = true;
  summary.startup_registration_not_yet_landed = true;
  summary.runtime_loader_registration_not_yet_landed = true;
  summary.explicit_non_goals_published = true;
  summary.typed_lowering_handoff_ready =
      IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
          executable_metadata_typed_lowering_handoff);
  summary.debug_projection_ready =
      IsReadyObjc3ExecutableMetadataDebugProjectionSummary(
          executable_metadata_debug_projection);
  if (summary.typed_lowering_handoff_ready) {
    summary.typed_lowering_handoff_replay_key =
        executable_metadata_typed_lowering_handoff.replay_key;
  }
  if (summary.debug_projection_ready) {
    summary.debug_projection_replay_key =
        executable_metadata_debug_projection.replay_key;
  }
  summary.ready_for_packaging_implementation =
      summary.typed_lowering_handoff_ready && summary.debug_projection_ready;
  if (summary.ready_for_packaging_implementation) {
    summary.replay_key =
        BuildExecutableMetadataRuntimeIngestPackagingReplayKey(summary);
  }
  if (!IsReadyObjc3ExecutableMetadataRuntimeIngestPackagingContractSummary(
          summary)) {
    summary.failure_reason =
        "runtime ingest packaging contract boundary is incomplete";
  }
  return summary;
}

std::string BuildExecutableMetadataRuntimeIngestPackagingContractSummaryJson(
    const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
        &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"typed_lowering_handoff_contract_id\":\""
      << EscapeJsonString(summary.typed_lowering_handoff_contract_id)
      << "\",\"debug_projection_contract_id\":\""
      << EscapeJsonString(summary.debug_projection_contract_id)
      << "\",\"packaging_surface_path\":\""
      << EscapeJsonString(summary.packaging_surface_path)
      << "\",\"typed_handoff_surface_path\":\""
      << EscapeJsonString(summary.typed_handoff_surface_path)
      << "\",\"debug_projection_surface_path\":\""
      << EscapeJsonString(summary.debug_projection_surface_path)
      << "\",\"packaging_payload_model\":\""
      << EscapeJsonString(summary.packaging_payload_model)
      << "\",\"transport_artifact_relative_path\":\""
      << EscapeJsonString(summary.transport_artifact_relative_path)
      << "\",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataRuntimeIngestPackagingContractSummary(
              summary)
              ? "true"
              : "false")
      << ",\"boundary_frozen\":"
      << (summary.boundary_frozen ? "true" : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"typed_lowering_handoff_ready\":"
      << (summary.typed_lowering_handoff_ready ? "true" : "false")
      << ",\"debug_projection_ready\":"
      << (summary.debug_projection_ready ? "true" : "false")
      << ",\"manifest_transport_frozen\":"
      << (summary.manifest_transport_frozen ? "true" : "false")
      << ",\"runtime_section_emission_not_yet_landed\":"
      << (summary.runtime_section_emission_not_yet_landed ? "true" : "false")
      << ",\"startup_registration_not_yet_landed\":"
      << (summary.startup_registration_not_yet_landed ? "true" : "false")
      << ",\"runtime_loader_registration_not_yet_landed\":"
      << (summary.runtime_loader_registration_not_yet_landed ? "true"
                                                             : "false")
      << ",\"explicit_non_goals_published\":"
      << (summary.explicit_non_goals_published ? "true" : "false")
      << ",\"ready_for_packaging_implementation\":"
      << (summary.ready_for_packaging_implementation ? "true" : "false")
      << ",\"typed_lowering_handoff_replay_key\":\""
      << EscapeJsonString(summary.typed_lowering_handoff_replay_key)
      << "\",\"debug_projection_replay_key\":\""
      << EscapeJsonString(summary.debug_projection_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildExecutableMetadataRuntimeIngestBinaryEnvelopePayload(
    const std::string &packaging_json,
    const std::string &typed_handoff_json,
    const std::string &debug_projection_json) {
  std::string payload;
  payload.reserve(
      std::char_traits<char>::length(
          kObjc3ExecutableMetadataRuntimeIngestBinaryMagic) +
      sizeof(std::uint32_t) * 8u + packaging_json.size() +
      typed_handoff_json.size() + debug_projection_json.size() + 256u);
  payload.append(kObjc3ExecutableMetadataRuntimeIngestBinaryMagic);
  AppendRuntimeIngestBinaryEnvelopeU32(
      payload, kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeVersion);
  AppendRuntimeIngestBinaryEnvelopeU32(
      payload, kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeChunkCount);
  AppendRuntimeIngestBinaryEnvelopeChunk(
      payload,
      kObjc3ExecutableMetadataRuntimeIngestBinaryPackagingChunkName,
      packaging_json);
  AppendRuntimeIngestBinaryEnvelopeChunk(
      payload, kObjc3ExecutableMetadataRuntimeIngestBinaryTypedHandoffChunkName,
      typed_handoff_json);
  AppendRuntimeIngestBinaryEnvelopeChunk(
      payload,
      kObjc3ExecutableMetadataRuntimeIngestBinaryDebugProjectionChunkName,
      debug_projection_json);
  return payload;
}

std::string BuildExecutableMetadataRuntimeIngestBinaryBoundaryReplayKey(
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";packaging_contract_id=" << summary.packaging_contract_id
      << ";typed_contract_id=" << summary.typed_lowering_handoff_contract_id
      << ";debug_contract_id=" << summary.debug_projection_contract_id
      << ";packaging_surface_path=" << summary.packaging_surface_path
      << ";binary_boundary_surface_path=" << summary.binary_boundary_surface_path
      << ";payload_model=" << summary.payload_model
      << ";envelope_format=" << summary.envelope_format
      << ";artifact_relative_path=" << summary.artifact_relative_path
      << ";artifact_suffix=" << summary.artifact_suffix
      << ";binary_magic=" << summary.binary_magic
      << ";envelope_version=" << summary.envelope_version
      << ";chunk_count=" << summary.chunk_count
      << ";payload_bytes=" << summary.payload_bytes
      << ";packaging_replay=" << summary.packaging_contract_replay_key
      << ";typed_replay=" << summary.typed_lowering_handoff_replay_key
      << ";debug_replay=" << summary.debug_projection_replay_key;
  return out.str();
}

std::string BuildExecutableMetadataRuntimeIngestBinaryBoundarySummaryJson(
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"packaging_contract_id\":\""
      << EscapeJsonString(summary.packaging_contract_id)
      << "\",\"typed_lowering_handoff_contract_id\":\""
      << EscapeJsonString(summary.typed_lowering_handoff_contract_id)
      << "\",\"debug_projection_contract_id\":\""
      << EscapeJsonString(summary.debug_projection_contract_id)
      << "\",\"packaging_surface_path\":\""
      << EscapeJsonString(summary.packaging_surface_path)
      << "\",\"binary_boundary_surface_path\":\""
      << EscapeJsonString(summary.binary_boundary_surface_path)
      << "\",\"payload_model\":\""
      << EscapeJsonString(summary.payload_model)
      << "\",\"envelope_format\":\""
      << EscapeJsonString(summary.envelope_format)
      << "\",\"artifact_relative_path\":\""
      << EscapeJsonString(summary.artifact_relative_path)
      << "\",\"artifact_suffix\":\""
      << EscapeJsonString(summary.artifact_suffix)
      << "\",\"binary_magic\":\""
      << EscapeJsonString(summary.binary_magic)
      << "\",\"envelope_version\":" << summary.envelope_version
      << ",\"chunk_count\":" << summary.chunk_count
      << ",\"chunk_names\":[";
  for (std::size_t i = 0; i < summary.chunk_names.size(); ++i) {
    out << "\"" << EscapeJsonString(summary.chunk_names[i]) << "\"";
    if (i + 1u != summary.chunk_names.size()) {
      out << ",";
    }
  }
  out << "],\"ready\":"
      << (IsReadyObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary(
              summary)
              ? "true"
              : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"packaging_contract_ready\":"
      << (summary.packaging_contract_ready ? "true" : "false")
      << ",\"typed_lowering_handoff_ready\":"
      << (summary.typed_lowering_handoff_ready ? "true" : "false")
      << ",\"debug_projection_ready\":"
      << (summary.debug_projection_ready ? "true" : "false")
      << ",\"binary_payload_present\":"
      << (summary.binary_payload_present ? "true" : "false")
      << ",\"binary_boundary_emitted\":"
      << (summary.binary_boundary_emitted ? "true" : "false")
      << ",\"binary_envelope_deterministic\":"
      << (summary.binary_envelope_deterministic ? "true" : "false")
      << ",\"ready_for_section_emission_handoff\":"
      << (summary.ready_for_section_emission_handoff ? "true" : "false")
      << ",\"payload_bytes\":" << summary.payload_bytes
      << ",\"packaging_contract_replay_key\":\""
      << EscapeJsonString(summary.packaging_contract_replay_key)
      << "\",\"typed_lowering_handoff_replay_key\":\""
      << EscapeJsonString(summary.typed_lowering_handoff_replay_key)
      << "\",\"debug_projection_replay_key\":\""
      << EscapeJsonString(summary.debug_projection_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
