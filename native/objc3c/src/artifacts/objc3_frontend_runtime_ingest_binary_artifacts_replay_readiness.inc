Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary
BuildExecutableMetadataRuntimeIngestBinaryBoundarySummary(
    const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
        &packaging_contract,
    const Objc3ExecutableMetadataTypedLoweringHandoff &typed_lowering_handoff,
    const Objc3ExecutableMetadataDebugProjectionSummary &debug_projection,
    const std::string &binary_payload) {
  Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary summary;
  summary.fail_closed = true;
  summary.packaging_contract_ready =
      IsReadyObjc3ExecutableMetadataRuntimeIngestPackagingContractSummary(
          packaging_contract);
  summary.typed_lowering_handoff_ready =
      IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
          typed_lowering_handoff);
  summary.debug_projection_ready =
      IsReadyObjc3ExecutableMetadataDebugProjectionSummary(debug_projection);
  if (summary.packaging_contract_ready) {
    summary.packaging_contract_replay_key = packaging_contract.replay_key;
  }
  if (summary.typed_lowering_handoff_ready) {
    summary.typed_lowering_handoff_replay_key =
        typed_lowering_handoff.replay_key;
  }
  if (summary.debug_projection_ready) {
    summary.debug_projection_replay_key = debug_projection.replay_key;
  }
  summary.binary_payload_present = !binary_payload.empty();
  summary.binary_boundary_emitted = summary.binary_payload_present;
  summary.binary_envelope_deterministic =
      summary.binary_payload_present && summary.packaging_contract_ready &&
      summary.typed_lowering_handoff_ready && summary.debug_projection_ready;
  summary.ready_for_section_emission_handoff =
      summary.packaging_contract_ready && summary.binary_envelope_deterministic;
  summary.payload_bytes = binary_payload.size();
  if (summary.ready_for_section_emission_handoff) {
    summary.replay_key =
        BuildExecutableMetadataRuntimeIngestBinaryBoundaryReplayKey(summary);
  }
  if (!IsReadyObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary(
          summary)) {
    summary.failure_reason =
        "runtime ingest binary boundary payload is incomplete";
  }
  return summary;
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
