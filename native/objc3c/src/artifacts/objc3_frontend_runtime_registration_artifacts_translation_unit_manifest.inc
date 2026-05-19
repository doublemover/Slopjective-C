std::string BuildRuntimeTranslationUnitRegistrationManifestReplayKey(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";translation_unit_registration_contract_id="
      << summary.translation_unit_registration_contract_id
      << ";runtime_support_library_link_wiring_contract_id="
      << summary.runtime_support_library_link_wiring_contract_id
      << ";manifest_surface_path=" << summary.manifest_surface_path
      << ";manifest_payload_model=" << summary.manifest_payload_model
      << ";manifest_artifact_relative_path="
      << summary.manifest_artifact_relative_path
      << ";runtime_support_library_archive_relative_path="
      << summary.runtime_support_library_archive_relative_path
      << ";constructor_root_symbol=" << summary.constructor_root_symbol
      << ";constructor_root_ownership_model="
      << summary.constructor_root_ownership_model
      << ";manifest_authority_model=" << summary.manifest_authority_model
      << ";constructor_init_stub_symbol_prefix="
      << summary.constructor_init_stub_symbol_prefix
      << ";constructor_init_stub_ownership_model="
      << summary.constructor_init_stub_ownership_model
      << ";constructor_priority_policy="
      << summary.constructor_priority_policy
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";class_descriptor_count=" << summary.class_descriptor_count
      << ";protocol_descriptor_count=" << summary.protocol_descriptor_count
      << ";category_descriptor_count=" << summary.category_descriptor_count
      << ";property_descriptor_count=" << summary.property_descriptor_count
      << ";ivar_descriptor_count=" << summary.ivar_descriptor_count
      << ";total_descriptor_count=" << summary.total_descriptor_count
      << ";translation_unit_registration_order_ordinal="
      << summary.translation_unit_registration_order_ordinal
      << ";translation_unit_registration_replay_key="
      << summary.translation_unit_registration_replay_key;
  for (std::size_t index = 0;
       index < summary.runtime_owned_payload_artifacts.size(); ++index) {
    out << ";runtime_owned_payload_artifacts[" << index
        << "]=" << summary.runtime_owned_payload_artifacts[index];
  }
  return out.str();
}

Objc3RuntimeTranslationUnitRegistrationManifestSummary
BuildRuntimeTranslationUnitRegistrationManifestSummary(
    const Objc3RuntimeTranslationUnitRegistrationContractSummary
        &registration_contract,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication,
    std::uint64_t translation_unit_registration_order_ordinal) {
  Objc3RuntimeTranslationUnitRegistrationManifestSummary summary;
  summary.fail_closed = true;
  summary.translation_unit_registration_contract_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationContractSummary(
          registration_contract);
  summary.runtime_support_library_link_wiring_ready =
      IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
          runtime_support_library_link_wiring);
  summary.runtime_manifest_template_published = true;
  summary.constructor_root_manifest_authoritative = true;
  summary.constructor_root_reserved_for_lowering = true;
  summary.init_stub_emission_deferred_to_lowering = true;
  summary.runtime_registration_artifact_emitted_by_driver = true;
  summary.runtime_owned_payload_artifact_count =
      summary.runtime_owned_payload_artifacts.size();
  summary.class_descriptor_count =
      runtime_metadata_section_publication.class_descriptor_count;
  summary.protocol_descriptor_count =
      runtime_metadata_section_publication.protocol_descriptor_count;
  summary.category_descriptor_count =
      runtime_metadata_section_publication.category_descriptor_count;
  summary.property_descriptor_count =
      runtime_metadata_section_publication.property_descriptor_count;
  summary.ivar_descriptor_count =
      runtime_metadata_section_publication.ivar_descriptor_count;
  summary.total_descriptor_count =
      runtime_metadata_section_publication.total_descriptor_count;
  if (translation_unit_registration_order_ordinal > 0) {
    summary.translation_unit_registration_order_ordinal =
        translation_unit_registration_order_ordinal;
  }
  if (summary.translation_unit_registration_contract_ready) {
    summary.translation_unit_registration_replay_key =
        registration_contract.replay_key;
  }
  summary.ready_for_lowering_init_stub_emission =
      summary.translation_unit_registration_contract_ready &&
      summary.runtime_support_library_link_wiring_ready;
  summary.launch_integration_ready =
      summary.ready_for_lowering_init_stub_emission;
  if (summary.ready_for_lowering_init_stub_emission) {
    summary.replay_key =
        BuildRuntimeTranslationUnitRegistrationManifestReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(summary)) {
    summary.failure_reason =
        "translation-unit registration manifest summary is incomplete";
  }
  return summary;
}
