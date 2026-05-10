void AppendObjc3RuntimeTranslationUnitRegistrationManifestFields(
    std::ostream &manifest,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest) {
  manifest
      << ",\"runtime_translation_unit_registration_manifest_contract_id\":\""
      << runtime_translation_unit_registration_manifest.contract_id
      << "\",\"runtime_translation_unit_registration_manifest_payload_model\":\""
      << runtime_translation_unit_registration_manifest.manifest_payload_model
      << "\",\"runtime_translation_unit_registration_manifest_artifact_relative_path\":\""
      << runtime_translation_unit_registration_manifest
             .manifest_artifact_relative_path
      << "\",\"runtime_translation_unit_registration_manifest_runtime_owned_payload_artifact_count\":"
      << runtime_translation_unit_registration_manifest
             .runtime_owned_payload_artifact_count
      << ",\"runtime_translation_unit_registration_manifest_runtime_support_library_archive_relative_path\":\""
      << runtime_translation_unit_registration_manifest
             .runtime_support_library_archive_relative_path
      << "\",\"runtime_translation_unit_registration_manifest_constructor_root_symbol\":\""
      << runtime_translation_unit_registration_manifest.constructor_root_symbol
      << "\",\"runtime_translation_unit_registration_manifest_constructor_root_ownership_model\":\""
      << runtime_translation_unit_registration_manifest
             .constructor_root_ownership_model
      << "\",\"runtime_translation_unit_registration_manifest_authority_model\":\""
      << runtime_translation_unit_registration_manifest.manifest_authority_model
      << "\",\"runtime_translation_unit_registration_manifest_init_stub_symbol_prefix\":\""
      << runtime_translation_unit_registration_manifest
             .constructor_init_stub_symbol_prefix
      << "\",\"runtime_translation_unit_registration_manifest_init_stub_ownership_model\":\""
      << runtime_translation_unit_registration_manifest
             .constructor_init_stub_ownership_model
      << "\",\"runtime_translation_unit_registration_manifest_constructor_priority_policy\":\""
      << runtime_translation_unit_registration_manifest.constructor_priority_policy
      << "\",\"runtime_translation_unit_registration_manifest_registration_entrypoint_symbol\":\""
      << runtime_translation_unit_registration_manifest.registration_entrypoint_symbol
      << "\",\"runtime_translation_unit_registration_manifest_translation_unit_identity_model\":\""
      << runtime_translation_unit_registration_manifest
             .translation_unit_identity_model
      << "\",\"runtime_translation_unit_registration_manifest_launch_integration_contract_id\":\""
      << runtime_translation_unit_registration_manifest
             .launch_integration_contract_id
      << "\",\"runtime_translation_unit_registration_manifest_runtime_library_resolution_model\":\""
      << runtime_translation_unit_registration_manifest
             .runtime_library_resolution_model
      << "\",\"runtime_translation_unit_registration_manifest_driver_linker_flag_consumption_model\":\""
      << runtime_translation_unit_registration_manifest
             .driver_linker_flag_consumption_model
      << "\",\"runtime_translation_unit_registration_manifest_compile_wrapper_command_surface\":\""
      << runtime_translation_unit_registration_manifest
             .compile_wrapper_command_surface
      << "\",\"runtime_translation_unit_registration_manifest_compile_proof_command_surface\":\""
      << runtime_translation_unit_registration_manifest
             .compile_proof_command_surface
      << "\",\"runtime_translation_unit_registration_manifest_execution_smoke_command_surface\":\""
      << runtime_translation_unit_registration_manifest
             .execution_smoke_command_surface
      << "\",\"runtime_translation_unit_registration_manifest_class_descriptor_count\":"
      << runtime_translation_unit_registration_manifest.class_descriptor_count
      << ",\"runtime_translation_unit_registration_manifest_protocol_descriptor_count\":"
      << runtime_translation_unit_registration_manifest.protocol_descriptor_count
      << ",\"runtime_translation_unit_registration_manifest_category_descriptor_count\":"
      << runtime_translation_unit_registration_manifest.category_descriptor_count
      << ",\"runtime_translation_unit_registration_manifest_property_descriptor_count\":"
      << runtime_translation_unit_registration_manifest.property_descriptor_count
      << ",\"runtime_translation_unit_registration_manifest_ivar_descriptor_count\":"
      << runtime_translation_unit_registration_manifest.ivar_descriptor_count
      << ",\"runtime_translation_unit_registration_manifest_total_descriptor_count\":"
      << runtime_translation_unit_registration_manifest.total_descriptor_count
      << ",\"runtime_translation_unit_registration_manifest_translation_unit_registration_order_ordinal\":"
      << runtime_translation_unit_registration_manifest
             .translation_unit_registration_order_ordinal
      << ",\"runtime_translation_unit_registration_manifest_fail_closed\":"
      << (runtime_translation_unit_registration_manifest.fail_closed ? "true"
                                                                    : "false")
      << ",\"runtime_translation_unit_registration_manifest_contract_ready\":"
      << (runtime_translation_unit_registration_manifest
                  .translation_unit_registration_contract_ready
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_manifest_runtime_support_library_link_wiring_ready\":"
      << (runtime_translation_unit_registration_manifest
                  .runtime_support_library_link_wiring_ready
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_manifest_template_published\":"
      << (runtime_translation_unit_registration_manifest
                  .runtime_manifest_template_published
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_manifest_constructor_root_manifest_authoritative\":"
      << (runtime_translation_unit_registration_manifest
                  .constructor_root_manifest_authoritative
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_manifest_constructor_root_reserved_for_lowering\":"
      << (runtime_translation_unit_registration_manifest
                  .constructor_root_reserved_for_lowering
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_manifest_init_stub_emission_deferred_to_lowering\":"
      << (runtime_translation_unit_registration_manifest
                  .init_stub_emission_deferred_to_lowering
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_manifest_artifact_emitted_by_driver\":"
      << (runtime_translation_unit_registration_manifest
                  .runtime_registration_artifact_emitted_by_driver
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_manifest_ready_for_lowering_init_stub_emission\":"
      << (runtime_translation_unit_registration_manifest
                  .ready_for_lowering_init_stub_emission
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_manifest_launch_integration_ready\":"
      << (runtime_translation_unit_registration_manifest.launch_integration_ready
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_manifest_translation_unit_registration_replay_key\":\""
      << EscapeJsonString(runtime_translation_unit_registration_manifest
                              .translation_unit_registration_replay_key)
      << "\",\"runtime_translation_unit_registration_manifest_replay_key\":\""
      << EscapeJsonString(runtime_translation_unit_registration_manifest.replay_key)
      << "\",\"runtime_translation_unit_registration_manifest_failure_reason\":\""
      << EscapeJsonString(runtime_translation_unit_registration_manifest.failure_reason)
      << "\"";
}
