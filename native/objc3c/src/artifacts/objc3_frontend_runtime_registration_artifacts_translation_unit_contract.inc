std::string BuildRuntimeTranslationUnitRegistrationContractReplayKey(
    const Objc3RuntimeTranslationUnitRegistrationContractSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";binary_boundary_contract_id=" << summary.binary_boundary_contract_id
      << ";archive_static_link_contract_id="
      << summary.archive_static_link_contract_id
      << ";object_emission_closeout_contract_id="
      << summary.object_emission_closeout_contract_id
      << ";runtime_support_library_link_wiring_contract_id="
      << summary.runtime_support_library_link_wiring_contract_id
      << ";registration_surface_path=" << summary.registration_surface_path
      << ";registration_payload_model="
      << summary.registration_payload_model
      << ";constructor_root_symbol=" << summary.constructor_root_symbol
      << ";constructor_root_ownership_model="
      << summary.constructor_root_ownership_model
      << ";constructor_emission_mode=" << summary.constructor_emission_mode
      << ";constructor_priority_policy="
      << summary.constructor_priority_policy
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";binary_boundary_replay_key=" << summary.binary_boundary_replay_key;
  for (std::size_t index = 0;
       index < summary.runtime_owned_payload_artifacts.size(); ++index) {
    out << ";runtime_owned_payload_artifacts[" << index
        << "]=" << summary.runtime_owned_payload_artifacts[index];
  }
  return out.str();
}

Objc3RuntimeTranslationUnitRegistrationContractSummary
BuildRuntimeTranslationUnitRegistrationContractSummary(
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary
        &runtime_ingest_binary_boundary,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring) {
  Objc3RuntimeTranslationUnitRegistrationContractSummary summary;
  summary.fail_closed = true;
  summary.boundary_frozen = true;
  summary.binary_boundary_ready =
      IsReadyObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary(
          runtime_ingest_binary_boundary);
  summary.runtime_support_library_link_wiring_ready =
      IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
          runtime_support_library_link_wiring);
  const std::string archive_static_link_summary =
      BuildArtifactRuntimeMetadataArchiveStaticLinkDiscoverySummary();
  summary.archive_static_link_surface_ready =
      archive_static_link_summary.find(
          std::string("contract=") +
          kObjc3ArtifactRuntimeArchiveStaticLinkDiscoveryContractId) !=
          std::string::npos &&
      archive_static_link_summary.find(
          std::string("translation_unit_identity_model=") +
          summary.translation_unit_identity_model) != std::string::npos;
  const std::string object_emission_closeout_summary =
      BuildArtifactRuntimeMetadataObjectEmissionCloseoutSummary();
  summary.object_emission_closeout_surface_ready =
      object_emission_closeout_summary.find(
          std::string("contract=") +
          kObjc3ArtifactRuntimeMetadataObjectEmissionCloseoutContractId) !=
          std::string::npos &&
      object_emission_closeout_summary.find(
          "non_goals=no-startup-registration-or-runtime-bootstrap") !=
          std::string::npos;
  summary.runtime_owned_payload_inventory_published = true;
  summary.constructor_root_reserved_not_emitted = true;
  summary.startup_registration_not_yet_landed = true;
  summary.runtime_bootstrap_not_yet_landed = true;
  summary.explicit_non_goals_published = true;
  summary.runtime_owned_payload_artifact_count =
      summary.runtime_owned_payload_artifacts.size();
  if (summary.binary_boundary_ready) {
    summary.binary_boundary_replay_key = runtime_ingest_binary_boundary.replay_key;
  }
  summary.ready_for_registration_manifest_implementation =
      summary.binary_boundary_ready &&
      summary.archive_static_link_surface_ready &&
      summary.object_emission_closeout_surface_ready &&
      summary.runtime_support_library_link_wiring_ready;
  if (summary.ready_for_registration_manifest_implementation) {
    summary.replay_key =
        BuildRuntimeTranslationUnitRegistrationContractReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeTranslationUnitRegistrationContractSummary(summary)) {
    summary.failure_reason =
        "translation-unit registration surface contract is incomplete";
  }
  return summary;
}
