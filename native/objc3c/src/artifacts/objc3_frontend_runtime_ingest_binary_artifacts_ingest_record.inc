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
