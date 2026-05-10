void AppendObjc3RuntimeTranslationUnitRegistrationContractManifestFields(
    std::ostream &manifest,
    const Objc3RuntimeTranslationUnitRegistrationContractSummary
        &runtime_translation_unit_registration_contract) {
  manifest
      << ",\"runtime_translation_unit_registration_contract_id\":\""
      << runtime_translation_unit_registration_contract.contract_id
      << "\",\"runtime_translation_unit_registration_binary_boundary_contract_id\":\""
      << runtime_translation_unit_registration_contract
             .binary_boundary_contract_id
      << "\",\"runtime_translation_unit_registration_archive_static_link_contract_id\":\""
      << runtime_translation_unit_registration_contract
             .archive_static_link_contract_id
      << "\",\"runtime_translation_unit_registration_object_emission_closeout_contract_id\":\""
      << runtime_translation_unit_registration_contract
             .object_emission_closeout_contract_id
      << "\",\"runtime_translation_unit_registration_runtime_support_library_link_wiring_contract_id\":\""
      << runtime_translation_unit_registration_contract
             .runtime_support_library_link_wiring_contract_id
      << "\",\"runtime_translation_unit_registration_payload_model\":\""
      << runtime_translation_unit_registration_contract.registration_payload_model
      << "\",\"runtime_translation_unit_registration_runtime_owned_payload_artifact_count\":"
      << runtime_translation_unit_registration_contract
             .runtime_owned_payload_artifact_count
      << ",\"runtime_translation_unit_registration_payload_artifact_relative_path\":\""
      << runtime_translation_unit_registration_contract
             .runtime_owned_payload_artifacts[0]
      << "\",\"runtime_translation_unit_registration_linker_response_artifact_relative_path\":\""
      << runtime_translation_unit_registration_contract
             .runtime_owned_payload_artifacts[1]
      << "\",\"runtime_translation_unit_registration_discovery_artifact_relative_path\":\""
      << runtime_translation_unit_registration_contract
             .runtime_owned_payload_artifacts[2]
      << "\",\"runtime_translation_unit_registration_constructor_root_symbol\":\""
      << runtime_translation_unit_registration_contract.constructor_root_symbol
      << "\",\"runtime_translation_unit_registration_constructor_root_ownership_model\":\""
      << runtime_translation_unit_registration_contract
             .constructor_root_ownership_model
      << "\",\"runtime_translation_unit_registration_constructor_emission_mode\":\""
      << runtime_translation_unit_registration_contract.constructor_emission_mode
      << "\",\"runtime_translation_unit_registration_constructor_priority_policy\":\""
      << runtime_translation_unit_registration_contract.constructor_priority_policy
      << "\",\"runtime_translation_unit_registration_entrypoint_symbol\":\""
      << runtime_translation_unit_registration_contract
             .registration_entrypoint_symbol
      << "\",\"runtime_translation_unit_registration_translation_unit_identity_model\":\""
      << runtime_translation_unit_registration_contract
             .translation_unit_identity_model
      << "\",\"runtime_translation_unit_registration_boundary_frozen\":"
      << (runtime_translation_unit_registration_contract.boundary_frozen ? "true"
                                                                        : "false")
      << ",\"runtime_translation_unit_registration_fail_closed\":"
      << (runtime_translation_unit_registration_contract.fail_closed ? "true"
                                                                    : "false")
      << ",\"runtime_translation_unit_registration_binary_boundary_ready\":"
      << (runtime_translation_unit_registration_contract.binary_boundary_ready
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_archive_static_link_surface_ready\":"
      << (runtime_translation_unit_registration_contract
                  .archive_static_link_surface_ready
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_object_emission_closeout_surface_ready\":"
      << (runtime_translation_unit_registration_contract
                  .object_emission_closeout_surface_ready
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_runtime_support_library_link_wiring_ready\":"
      << (runtime_translation_unit_registration_contract
                  .runtime_support_library_link_wiring_ready
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_runtime_owned_payload_inventory_published\":"
      << (runtime_translation_unit_registration_contract
                  .runtime_owned_payload_inventory_published
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_constructor_root_reserved_not_emitted\":"
      << (runtime_translation_unit_registration_contract
                  .constructor_root_reserved_not_emitted
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_startup_registration_not_yet_landed\":"
      << (runtime_translation_unit_registration_contract
                  .startup_registration_not_yet_landed
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_runtime_bootstrap_not_yet_landed\":"
      << (runtime_translation_unit_registration_contract
                  .runtime_bootstrap_not_yet_landed
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_explicit_non_goals_published\":"
      << (runtime_translation_unit_registration_contract
                  .explicit_non_goals_published
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_ready_for_manifest_implementation\":"
      << (runtime_translation_unit_registration_contract
                  .ready_for_registration_manifest_implementation
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_binary_boundary_replay_key\":\""
      << EscapeJsonString(runtime_translation_unit_registration_contract
                              .binary_boundary_replay_key)
      << "\",\"runtime_translation_unit_registration_replay_key\":\""
      << EscapeJsonString(runtime_translation_unit_registration_contract.replay_key)
      << "\",\"runtime_translation_unit_registration_failure_reason\":\""
      << EscapeJsonString(
             runtime_translation_unit_registration_contract.failure_reason)
      << "\"";
}
