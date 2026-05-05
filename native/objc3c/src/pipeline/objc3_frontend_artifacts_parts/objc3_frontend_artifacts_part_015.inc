                  .manifest_artifact_relative_path
           << "\",\"runtime_translation_unit_registration_manifest_runtime_owned_payload_artifact_count\":"
           << runtime_translation_unit_registration_manifest
                  .runtime_owned_payload_artifact_count
           << ",\"runtime_translation_unit_registration_manifest_runtime_support_library_archive_relative_path\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_support_library_archive_relative_path
           << "\",\"runtime_translation_unit_registration_manifest_constructor_root_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .constructor_root_symbol
           << "\",\"runtime_translation_unit_registration_manifest_constructor_root_ownership_model\":\""
           << runtime_translation_unit_registration_manifest
                  .constructor_root_ownership_model
           << "\",\"runtime_translation_unit_registration_manifest_authority_model\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_authority_model
           << "\",\"runtime_translation_unit_registration_manifest_init_stub_symbol_prefix\":\""
           << runtime_translation_unit_registration_manifest
                  .constructor_init_stub_symbol_prefix
           << "\",\"runtime_translation_unit_registration_manifest_init_stub_ownership_model\":\""
           << runtime_translation_unit_registration_manifest
                  .constructor_init_stub_ownership_model
           << "\",\"runtime_translation_unit_registration_manifest_constructor_priority_policy\":\""
           << runtime_translation_unit_registration_manifest
                  .constructor_priority_policy
           << "\",\"runtime_translation_unit_registration_manifest_registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
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
           << runtime_translation_unit_registration_manifest
                  .class_descriptor_count
           << ",\"runtime_translation_unit_registration_manifest_protocol_descriptor_count\":"
           << runtime_translation_unit_registration_manifest
                  .protocol_descriptor_count
           << ",\"runtime_translation_unit_registration_manifest_category_descriptor_count\":"
           << runtime_translation_unit_registration_manifest
                  .category_descriptor_count
           << ",\"runtime_translation_unit_registration_manifest_property_descriptor_count\":"
           << runtime_translation_unit_registration_manifest
                  .property_descriptor_count
           << ",\"runtime_translation_unit_registration_manifest_ivar_descriptor_count\":"
           << runtime_translation_unit_registration_manifest
                  .ivar_descriptor_count
           << ",\"runtime_translation_unit_registration_manifest_total_descriptor_count\":"
           << runtime_translation_unit_registration_manifest
                  .total_descriptor_count
           << ",\"runtime_translation_unit_registration_manifest_translation_unit_registration_order_ordinal\":"
           << runtime_translation_unit_registration_manifest
                  .translation_unit_registration_order_ordinal
           << ",\"runtime_translation_unit_registration_manifest_fail_closed\":"
           << (runtime_translation_unit_registration_manifest.fail_closed
                   ? "true"
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
           << (runtime_translation_unit_registration_manifest
                       .launch_integration_ready
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_translation_unit_registration_replay_key\":\""
           << EscapeJsonString(
                  runtime_translation_unit_registration_manifest
                      .translation_unit_registration_replay_key)
          << "\",\"runtime_translation_unit_registration_manifest_replay_key\":\""
          << EscapeJsonString(
                 runtime_translation_unit_registration_manifest.replay_key)
          << "\",\"runtime_translation_unit_registration_manifest_failure_reason\":\""
          << EscapeJsonString(
                 runtime_translation_unit_registration_manifest.failure_reason)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_contract_id\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .contract_id)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_path\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .source_surface_path)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_pragma_name\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .registration_descriptor_pragma_name)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_image_root_pragma_name\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .image_root_pragma_name)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_module_identity_source\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .module_identity_source)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_identity_source\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .registration_descriptor_identity_source)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_image_root_identity_source\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .image_root_identity_source)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_bootstrap_visible_metadata_ownership_model\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .bootstrap_visible_metadata_ownership_model)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_module_name\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .module_name)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_identifier\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .registration_descriptor_identifier)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_image_root_identifier\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .image_root_identifier)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_registration_manifest_replay_key\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .registration_manifest_replay_key)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_replay_key\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .replay_key)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_fail_closed\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .fail_closed
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_registration_manifest_contract_ready\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .registration_manifest_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_source_surface_frozen\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .source_surface_frozen
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_prelude_pragma_contract_published\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .prelude_pragma_contract_published
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_identifier_resolved\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .registration_descriptor_identifier_resolved
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_image_root_identifier_resolved\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .image_root_identifier_resolved
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_bootstrap_visible_metadata_ownership_published\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .bootstrap_visible_metadata_ownership_published
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_ready_for_descriptor_frontend_closure\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .ready_for_descriptor_frontend_closure
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_pragma_seen\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .registration_descriptor_pragma_seen
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_pragma_duplicate\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .registration_descriptor_pragma_duplicate
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_pragma_non_leading\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .registration_descriptor_pragma_non_leading
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_pragma_directive_count\":"
          << runtime_registration_descriptor_image_root_source_surface
                 .registration_descriptor_pragma_directive_count
          << ",\"runtime_registration_descriptor_image_root_source_surface_image_root_pragma_seen\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .image_root_pragma_seen
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_image_root_pragma_duplicate\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .image_root_pragma_duplicate
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_image_root_pragma_non_leading\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .image_root_pragma_non_leading
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_image_root_pragma_directive_count\":"
          << runtime_registration_descriptor_image_root_source_surface
                 .image_root_pragma_directive_count
          << ",\"runtime_registration_descriptor_image_root_source_surface_failure_reason\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .failure_reason)
          << "\""
          << ",\"runtime_registration_descriptor_frontend_closure_contract_id\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_frontend_closure.contract_id)
          << "\",\"runtime_registration_descriptor_frontend_closure_source_surface_contract_id\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .source_surface_contract_id)
          << "\",\"runtime_registration_descriptor_frontend_closure_payload_model\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_frontend_closure.payload_model)
          << "\",\"runtime_registration_descriptor_frontend_closure_artifact_relative_path\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .artifact_relative_path)
          << "\",\"runtime_registration_descriptor_frontend_closure_authority_model\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .authority_model)
          << "\",\"runtime_registration_descriptor_frontend_closure_translation_unit_identity_model\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .translation_unit_identity_model)
          << "\",\"runtime_registration_descriptor_frontend_closure_payload_ownership_model\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .payload_ownership_model)
          << "\",\"runtime_registration_descriptor_frontend_closure_registration_descriptor_identifier\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .registration_descriptor_identifier)
          << "\",\"runtime_registration_descriptor_frontend_closure_image_root_identifier\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .image_root_identifier)
          << "\",\"runtime_registration_descriptor_frontend_closure_registration_descriptor_identity_source\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .registration_descriptor_identity_source)
          << "\",\"runtime_registration_descriptor_frontend_closure_image_root_identity_source\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .image_root_identity_source)
          << "\",\"runtime_registration_descriptor_frontend_closure_bootstrap_visible_metadata_ownership_model\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .bootstrap_visible_metadata_ownership_model)
          << "\",\"runtime_registration_descriptor_frontend_closure_class_descriptor_count\":"
          << runtime_registration_descriptor_frontend_closure
                 .class_descriptor_count
          << ",\"runtime_registration_descriptor_frontend_closure_protocol_descriptor_count\":"
          << runtime_registration_descriptor_frontend_closure
                 .protocol_descriptor_count
          << ",\"runtime_registration_descriptor_frontend_closure_category_descriptor_count\":"
          << runtime_registration_descriptor_frontend_closure
                 .category_descriptor_count
          << ",\"runtime_registration_descriptor_frontend_closure_property_descriptor_count\":"
          << runtime_registration_descriptor_frontend_closure
                 .property_descriptor_count
          << ",\"runtime_registration_descriptor_frontend_closure_ivar_descriptor_count\":"
          << runtime_registration_descriptor_frontend_closure
                 .ivar_descriptor_count
          << ",\"runtime_registration_descriptor_frontend_closure_total_descriptor_count\":"
          << runtime_registration_descriptor_frontend_closure.total_descriptor_count
          << ",\"runtime_registration_descriptor_frontend_closure_translation_unit_registration_order_ordinal\":"
          << runtime_registration_descriptor_frontend_closure
                 .translation_unit_registration_order_ordinal
          << ",\"runtime_registration_descriptor_frontend_closure_fail_closed\":"
          << (runtime_registration_descriptor_frontend_closure.fail_closed ? "true"
                                                                          : "false")
          << ",\"runtime_registration_descriptor_frontend_closure_source_surface_contract_ready\":"
          << (runtime_registration_descriptor_frontend_closure
                      .source_surface_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_frontend_closure_registration_manifest_contract_ready\":"
          << (runtime_registration_descriptor_frontend_closure
                      .registration_manifest_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_frontend_closure_descriptor_frontend_surface_published\":"
          << (runtime_registration_descriptor_frontend_closure
                      .descriptor_frontend_surface_published
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_frontend_closure_descriptor_artifact_template_published\":"
          << (runtime_registration_descriptor_frontend_closure
                      .descriptor_artifact_template_published
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_frontend_closure_descriptor_fields_resolved\":"
          << (runtime_registration_descriptor_frontend_closure
                      .descriptor_fields_resolved
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_frontend_closure_ready_for_descriptor_artifact_emission\":"
          << (runtime_registration_descriptor_frontend_closure
                      .ready_for_descriptor_artifact_emission
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_frontend_closure_ready_for_registration_descriptor_lowering\":"
          << (runtime_registration_descriptor_frontend_closure
                      .ready_for_registration_descriptor_lowering
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_frontend_closure_source_surface_replay_key\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .source_surface_replay_key)
          << "\",\"runtime_registration_descriptor_frontend_closure_registration_manifest_replay_key\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .registration_manifest_replay_key)
          << "\",\"runtime_registration_descriptor_frontend_closure_replay_key\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .replay_key)
          << "\",\"runtime_registration_descriptor_frontend_closure_failure_reason\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .failure_reason)
          << "\""
          << ",\"runtime_bootstrap_legality_failure_contract_id\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .contract_id)
          << "\",\"runtime_bootstrap_legality_failure_registration_descriptor_frontend_closure_contract_id\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .registration_descriptor_frontend_closure_contract_id)
          << "\",\"runtime_bootstrap_legality_failure_bootstrap_semantics_contract_id\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .bootstrap_semantics_contract_id)
          << "\",\"runtime_bootstrap_legality_failure_frontend_surface_path\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .frontend_surface_path)
          << "\",\"runtime_bootstrap_legality_failure_duplicate_registration_policy\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .duplicate_registration_policy)
          << "\",\"runtime_bootstrap_legality_failure_image_registration_order_invariant\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .image_registration_order_invariant)
          << "\",\"runtime_bootstrap_legality_failure_failure_mode\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .failure_mode)
          << "\",\"runtime_bootstrap_legality_failure_restart_lifecycle_model\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .restart_lifecycle_model)
          << "\",\"runtime_bootstrap_legality_failure_replay_order_model\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .replay_order_model)
          << "\",\"runtime_bootstrap_legality_failure_image_local_init_reset_model\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .image_local_init_reset_model)
          << "\",\"runtime_bootstrap_legality_failure_catalog_retention_model\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .catalog_retention_model)
          << "\",\"runtime_bootstrap_legality_failure_runtime_state_snapshot_symbol\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .runtime_state_snapshot_symbol)
          << "\",\"runtime_bootstrap_legality_failure_registration_descriptor_identifier\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .registration_descriptor_identifier)
          << "\",\"runtime_bootstrap_legality_failure_image_root_identifier\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .image_root_identifier)
          << "\",\"runtime_bootstrap_legality_failure_registration_descriptor_identity_source\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .registration_descriptor_identity_source)
          << "\",\"runtime_bootstrap_legality_failure_image_root_identity_source\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .image_root_identity_source)
          << "\",\"runtime_bootstrap_legality_failure_translation_unit_registration_order_ordinal\":"
          << runtime_bootstrap_legality_failure_contract
                 .translation_unit_registration_order_ordinal
          << ",\"runtime_bootstrap_legality_failure_fail_closed\":"
          << (runtime_bootstrap_legality_failure_contract.fail_closed ? "true"
                                                                     : "false")
          << ",\"runtime_bootstrap_legality_failure_registration_descriptor_frontend_closure_contract_ready\":"
          << (runtime_bootstrap_legality_failure_contract
                      .registration_descriptor_frontend_closure_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_failure_bootstrap_semantics_contract_ready\":"
          << (runtime_bootstrap_legality_failure_contract
                      .bootstrap_semantics_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_failure_semantic_boundary_ready\":"
          << (runtime_bootstrap_legality_failure_contract.semantic_boundary_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_failure_duplicate_registration_policy_frozen\":"
          << (runtime_bootstrap_legality_failure_contract
                      .duplicate_registration_policy_frozen
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_failure_image_order_invariant_frozen\":"
          << (runtime_bootstrap_legality_failure_contract
                      .image_order_invariant_frozen
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_failure_bootstrap_rejection_frozen\":"
          << (runtime_bootstrap_legality_failure_contract
                      .bootstrap_rejection_frozen
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_failure_restart_boundary_frozen\":"
          << (runtime_bootstrap_legality_failure_contract
                      .restart_boundary_frozen
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_failure_semantic_diagnostics_required\":"
          << (runtime_bootstrap_legality_failure_contract
                      .semantic_diagnostics_required
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_failure_ready_for_lowering_and_runtime\":"
          << (runtime_bootstrap_legality_failure_contract
                      .ready_for_lowering_and_runtime
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_failure_registration_descriptor_frontend_closure_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .registration_descriptor_frontend_closure_replay_key)
          << "\",\"runtime_bootstrap_legality_failure_bootstrap_semantics_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .bootstrap_semantics_replay_key)
          << "\",\"runtime_bootstrap_legality_failure_semantic_boundary_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .semantic_boundary_replay_key)
          << "\",\"runtime_bootstrap_legality_failure_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .replay_key)
          << "\",\"runtime_bootstrap_legality_failure_failure_reason\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .failure_reason)
          << "\""
          << ",\"runtime_bootstrap_legality_semantics_contract_id\":\""
          << EscapeJsonString(runtime_bootstrap_legality_semantics.contract_id)
          << "\",\"runtime_bootstrap_legality_semantics_bootstrap_legality_failure_contract_id\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .bootstrap_legality_failure_contract_id)
          << "\",\"runtime_bootstrap_legality_semantics_registration_descriptor_frontend_closure_contract_id\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .registration_descriptor_frontend_closure_contract_id)
          << "\",\"runtime_bootstrap_legality_semantics_bootstrap_semantics_contract_id\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .bootstrap_semantics_contract_id)
          << "\",\"runtime_bootstrap_legality_semantics_frontend_surface_path\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics.frontend_surface_path)
          << "\",\"runtime_bootstrap_legality_semantics_duplicate_registration_policy\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .duplicate_registration_policy)
          << "\",\"runtime_bootstrap_legality_semantics_image_registration_order_invariant\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .image_registration_order_invariant)
          << "\",\"runtime_bootstrap_legality_semantics_cross_image_legality_model\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics.cross_image_legality_model)
          << "\",\"runtime_bootstrap_legality_semantics_semantic_diagnostic_model\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .semantic_diagnostic_model)
          << "\",\"runtime_bootstrap_legality_semantics_translation_unit_identity_model\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .translation_unit_identity_model)
          << "\",\"runtime_bootstrap_legality_semantics_translation_unit_identity_key\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .translation_unit_identity_key)
          << "\",\"runtime_bootstrap_legality_semantics_registration_descriptor_identifier\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .registration_descriptor_identifier)
          << "\",\"runtime_bootstrap_legality_semantics_image_root_identifier\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics.image_root_identifier)
          << "\",\"runtime_bootstrap_legality_semantics_registration_descriptor_identity_source\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .registration_descriptor_identity_source)
          << "\",\"runtime_bootstrap_legality_semantics_image_root_identity_source\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .image_root_identity_source)
          << "\",\"runtime_bootstrap_legality_semantics_translation_unit_registration_order_ordinal\":"
          << runtime_bootstrap_legality_semantics
                 .translation_unit_registration_order_ordinal
          << ",\"runtime_bootstrap_legality_semantics_fail_closed\":"
          << (runtime_bootstrap_legality_semantics.fail_closed ? "true"
                                                               : "false")
          << ",\"runtime_bootstrap_legality_semantics_semantic_boundary_ready\":"
          << (runtime_bootstrap_legality_semantics.semantic_boundary_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_semantics_bootstrap_legality_failure_contract_ready\":"
          << (runtime_bootstrap_legality_semantics
                      .bootstrap_legality_failure_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_semantics_registration_descriptor_frontend_closure_contract_ready\":"
          << (runtime_bootstrap_legality_semantics
                      .registration_descriptor_frontend_closure_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_semantics_bootstrap_semantics_contract_ready\":"
          << (runtime_bootstrap_legality_semantics
                      .bootstrap_semantics_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_semantics_duplicate_registration_semantics_landed\":"
          << (runtime_bootstrap_legality_semantics
                      .duplicate_registration_semantics_landed
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_semantics_image_order_semantics_landed\":"
          << (runtime_bootstrap_legality_semantics.image_order_semantics_landed
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_semantics_cross_image_legality_semantics_landed\":"
          << (runtime_bootstrap_legality_semantics
                      .cross_image_legality_semantics_landed
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_semantics_semantic_diagnostics_landed\":"
          << (runtime_bootstrap_legality_semantics.semantic_diagnostics_landed
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_semantics_ready_for_lowering_and_runtime\":"
          << (runtime_bootstrap_legality_semantics.ready_for_lowering_and_runtime
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_semantics_semantic_boundary_replay_key\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics.semantic_boundary_replay_key)
          << "\",\"runtime_bootstrap_legality_semantics_bootstrap_legality_failure_contract_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_legality_semantics
                                  .bootstrap_legality_failure_contract_replay_key)
          << "\",\"runtime_bootstrap_legality_semantics_registration_descriptor_frontend_closure_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_legality_semantics
                                  .registration_descriptor_frontend_closure_replay_key)
          << "\",\"runtime_bootstrap_legality_semantics_bootstrap_semantics_replay_key\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .bootstrap_semantics_replay_key)
          << "\",\"runtime_bootstrap_legality_semantics_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_legality_semantics.replay_key)
          << "\",\"runtime_bootstrap_legality_semantics_failure_reason\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics.failure_reason)
          << "\""
          << ",\"runtime_bootstrap_failure_restart_semantics_contract_id\":\""
          << EscapeJsonString(
                 runtime_bootstrap_failure_restart_semantics.contract_id)
          << "\",\"runtime_bootstrap_failure_restart_semantics_bootstrap_legality_semantics_contract_id\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .bootstrap_legality_semantics_contract_id)
          << "\",\"runtime_bootstrap_failure_restart_semantics_bootstrap_reset_contract_id\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .bootstrap_reset_contract_id)
          << "\",\"runtime_bootstrap_failure_restart_semantics_bootstrap_semantics_contract_id\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .bootstrap_semantics_contract_id)
          << "\",\"runtime_bootstrap_failure_restart_semantics_frontend_surface_path\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .frontend_surface_path)
          << "\",\"runtime_bootstrap_failure_restart_semantics_failure_mode\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .failure_mode)
          << "\",\"runtime_bootstrap_failure_restart_semantics_restart_lifecycle_model\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .restart_lifecycle_model)
          << "\",\"runtime_bootstrap_failure_restart_semantics_replay_order_model\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .replay_order_model)
          << "\",\"runtime_bootstrap_failure_restart_semantics_image_local_init_reset_model\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .image_local_init_reset_model)
          << "\",\"runtime_bootstrap_failure_restart_semantics_catalog_retention_model\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .catalog_retention_model)
          << "\",\"runtime_bootstrap_failure_restart_semantics_unsupported_topology_model\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .unsupported_topology_model)
          << "\",\"runtime_bootstrap_failure_restart_semantics_translation_unit_identity_model\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .translation_unit_identity_model)
          << "\",\"runtime_bootstrap_failure_restart_semantics_translation_unit_identity_key\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .translation_unit_identity_key)
          << "\",\"runtime_bootstrap_failure_restart_semantics_runtime_state_snapshot_symbol\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .runtime_state_snapshot_symbol)
          << "\",\"runtime_bootstrap_failure_restart_semantics_replay_registered_images_symbol\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .replay_registered_images_symbol)
          << "\",\"runtime_bootstrap_failure_restart_semantics_reset_replay_state_snapshot_symbol\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .reset_replay_state_snapshot_symbol)
          << "\",\"runtime_bootstrap_failure_restart_semantics_invalid_descriptor_status_code\":"
          << runtime_bootstrap_failure_restart_semantics
                 .invalid_descriptor_status_code
          << ",\"runtime_bootstrap_failure_restart_semantics_translation_unit_registration_order_ordinal\":"
          << runtime_bootstrap_failure_restart_semantics
                 .translation_unit_registration_order_ordinal
          << ",\"runtime_bootstrap_failure_restart_semantics_fail_closed\":"
          << (runtime_bootstrap_failure_restart_semantics.fail_closed ? "true"
                                                                      : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_semantic_boundary_ready\":"
          << (runtime_bootstrap_failure_restart_semantics.semantic_boundary_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_bootstrap_legality_semantics_contract_ready\":"
          << (runtime_bootstrap_failure_restart_semantics
                      .bootstrap_legality_semantics_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_bootstrap_semantics_contract_ready\":"
          << (runtime_bootstrap_failure_restart_semantics
                      .bootstrap_semantics_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_bootstrap_reset_contract_ready\":"
          << (runtime_bootstrap_failure_restart_semantics
                      .bootstrap_reset_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_failure_mode_semantics_landed\":"
          << (runtime_bootstrap_failure_restart_semantics
                      .failure_mode_semantics_landed
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_restart_semantics_landed\":"
          << (runtime_bootstrap_failure_restart_semantics
                      .restart_semantics_landed
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_replay_semantics_landed\":"
          << (runtime_bootstrap_failure_restart_semantics
                      .replay_semantics_landed
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_unsupported_topology_semantics_landed\":"
          << (runtime_bootstrap_failure_restart_semantics
                      .unsupported_topology_semantics_landed
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_deterministic_recovery_semantics_landed\":"
          << (runtime_bootstrap_failure_restart_semantics
                      .deterministic_recovery_semantics_landed
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_runtime_restart_probe_required\":"
          << (runtime_bootstrap_failure_restart_semantics
                      .runtime_restart_probe_required
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_ready_for_lowering_and_runtime\":"
          << (runtime_bootstrap_failure_restart_semantics
                      .ready_for_lowering_and_runtime
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_semantic_boundary_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .semantic_boundary_replay_key)
          << "\",\"runtime_bootstrap_failure_restart_semantics_bootstrap_legality_semantics_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .bootstrap_legality_semantics_replay_key)
          << "\",\"runtime_bootstrap_failure_restart_semantics_bootstrap_semantics_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .bootstrap_semantics_replay_key)
          << "\",\"runtime_bootstrap_failure_restart_semantics_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .replay_key)
          << "\",\"runtime_bootstrap_failure_restart_semantics_failure_reason\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .failure_reason)
          << "\""
          << ",\"runtime_bootstrap_api_contract_id\":\""
          << EscapeJsonString(runtime_bootstrap_api.contract_id)
          << "\",\"runtime_bootstrap_api_public_header_path\":\""
          << EscapeJsonString(runtime_bootstrap_api.public_header_path)
          << "\",\"runtime_bootstrap_api_archive_relative_path\":\""
          << EscapeJsonString(runtime_bootstrap_api.archive_relative_path)
          << "\",\"runtime_bootstrap_api_registration_status_enum_type\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.registration_status_enum_type)
          << "\",\"runtime_bootstrap_api_image_descriptor_type\":\""
          << EscapeJsonString(runtime_bootstrap_api.image_descriptor_type)
          << "\",\"runtime_bootstrap_api_selector_handle_type\":\""
          << EscapeJsonString(runtime_bootstrap_api.selector_handle_type)
          << "\",\"runtime_bootstrap_api_registration_snapshot_type\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.registration_snapshot_type)
          << "\",\"runtime_bootstrap_api_registration_entrypoint_symbol\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.registration_entrypoint_symbol)
          << "\",\"runtime_bootstrap_api_selector_lookup_symbol\":\""
          << EscapeJsonString(runtime_bootstrap_api.selector_lookup_symbol)
          << "\",\"runtime_bootstrap_api_dispatch_entrypoint_symbol\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.dispatch_entrypoint_symbol)
          << "\",\"runtime_bootstrap_api_compatibility_dispatch_symbol\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.compatibility_dispatch_symbol)
          << "\",\"runtime_bootstrap_api_state_snapshot_symbol\":\""
          << EscapeJsonString(runtime_bootstrap_api.state_snapshot_symbol)
          << "\",\"runtime_bootstrap_api_reset_for_testing_symbol\":\""
          << EscapeJsonString(runtime_bootstrap_api.reset_for_testing_symbol)
          << "\",\"runtime_bootstrap_api_registration_result_model\":\""
          << EscapeJsonString(runtime_bootstrap_api.registration_result_model)
          << "\",\"runtime_bootstrap_api_registration_order_ordinal_model\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.registration_order_ordinal_model)
          << "\",\"runtime_bootstrap_api_runtime_state_locking_model\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.runtime_state_locking_model)
          << "\",\"runtime_bootstrap_api_startup_invocation_model\":\""
          << EscapeJsonString(runtime_bootstrap_api.startup_invocation_model)
          << "\",\"runtime_bootstrap_api_image_walk_lifecycle_model\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.image_walk_lifecycle_model)
          << "\",\"runtime_bootstrap_api_deterministic_reset_lifecycle_model\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.deterministic_reset_lifecycle_model)
          << "\",\"runtime_bootstrap_api_ready_for_registrar_implementation\":"
          << (runtime_bootstrap_api.ready_for_registrar_implementation ? "true"
                                                                       : "false")
          << ",\"runtime_bootstrap_api_support_library_core_feature_replay_key\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.support_library_core_feature_replay_key)
          << "\",\"runtime_bootstrap_api_support_library_link_wiring_replay_key\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.support_library_link_wiring_replay_key)
          << "\""
          << ",\"runtime_bootstrap_api_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_api.replay_key)
          << "\",\"runtime_bootstrap_api_failure_reason\":\""
          << EscapeJsonString(runtime_bootstrap_api.failure_reason)
          << "\""
          << ",\"runtime_bootstrap_semantics_contract_id\":\""
          << runtime_bootstrap_semantics.contract_id
           << "\",\"runtime_bootstrap_semantics_bootstrap_invariant_contract_id\":\""
           << runtime_bootstrap_semantics.bootstrap_invariant_contract_id
           << "\",\"runtime_bootstrap_semantics_registration_manifest_contract_id\":\""
           << runtime_bootstrap_semantics.registration_manifest_contract_id
           << "\",\"runtime_bootstrap_semantics_duplicate_registration_policy\":\""
           << runtime_bootstrap_semantics.duplicate_registration_policy
           << "\",\"runtime_bootstrap_semantics_realization_order_policy\":\""
           << runtime_bootstrap_semantics.realization_order_policy
           << "\",\"runtime_bootstrap_semantics_failure_mode\":\""
           << runtime_bootstrap_semantics.failure_mode
           << "\",\"runtime_bootstrap_semantics_result_model\":\""
           << runtime_bootstrap_semantics.registration_result_model
           << "\",\"runtime_bootstrap_semantics_registration_order_ordinal_model\":\""
           << runtime_bootstrap_semantics.registration_order_ordinal_model
           << "\",\"runtime_bootstrap_semantics_runtime_state_snapshot_symbol\":\""
           << runtime_bootstrap_semantics.runtime_state_snapshot_symbol
           << "\",\"runtime_bootstrap_semantics_runtime_library_archive_relative_path\":\""
           << runtime_bootstrap_semantics.runtime_library_archive_relative_path
           << "\",\"runtime_bootstrap_semantics_translation_unit_registration_order_ordinal\":"
           << runtime_bootstrap_semantics.translation_unit_registration_order_ordinal
           << ",\"runtime_bootstrap_semantics_success_status_code\":"
           << runtime_bootstrap_semantics.success_status_code
           << ",\"runtime_bootstrap_semantics_invalid_descriptor_status_code\":"
           << runtime_bootstrap_semantics.invalid_descriptor_status_code
           << ",\"runtime_bootstrap_semantics_duplicate_registration_status_code\":"
           << runtime_bootstrap_semantics.duplicate_registration_status_code
           << ",\"runtime_bootstrap_semantics_out_of_order_status_code\":"
           << runtime_bootstrap_semantics.out_of_order_status_code
           << ",\"runtime_bootstrap_semantics_fail_closed\":"
           << (runtime_bootstrap_semantics.fail_closed ? "true" : "false")
           << ",\"runtime_bootstrap_semantics_live_runtime_enforcement_landed\":"
           << (runtime_bootstrap_semantics.live_runtime_enforcement_landed ? "true"
                                                                          : "false")
           << ",\"runtime_bootstrap_semantics_no_partial_commit_on_failure\":"
           << (runtime_bootstrap_semantics.no_partial_commit_on_failure ? "true"
                                                                        : "false")
           << ",\"runtime_bootstrap_semantics_ready_for_constructor_root_implementation\":"
           << (runtime_bootstrap_semantics
                       .ready_for_constructor_root_implementation
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_semantics_replay_key\":\""
           << EscapeJsonString(runtime_bootstrap_semantics.replay_key)
           << "\",\"runtime_bootstrap_semantics_failure_reason\":\""
           << EscapeJsonString(runtime_bootstrap_semantics.failure_reason)
           << "\""
           << ",\"runtime_bootstrap_lowering_contract_id\":\""
           << runtime_bootstrap_lowering.contract_id
           << "\",\"runtime_bootstrap_lowering_registration_manifest_contract_id\":\""
           << runtime_bootstrap_lowering.registration_manifest_contract_id
           << "\",\"runtime_bootstrap_registrar_contract_id\":\""
           << kObjc3RuntimeBootstrapRegistrarContractId
           << "\",\"runtime_bootstrap_registrar_stage_registration_table_symbol\":\""
           << kObjc3RuntimeBootstrapStageRegistrationTableSymbol
           << "\",\"runtime_bootstrap_registrar_image_walk_snapshot_symbol\":\""
           << kObjc3RuntimeBootstrapImageWalkSnapshotSymbol
           << "\",\"runtime_bootstrap_registrar_image_walk_model\":\""
           << kObjc3RuntimeBootstrapImageWalkModel
           << "\",\"runtime_bootstrap_registrar_discovery_root_validation_model\":\""
           << kObjc3RuntimeBootstrapDiscoveryRootValidationModel
           << "\",\"runtime_bootstrap_registrar_selector_pool_interning_model\":\""
           << kObjc3RuntimeBootstrapSelectorPoolInterningModel
           << "\",\"runtime_bootstrap_registrar_realization_staging_model\":\""
           << kObjc3RuntimeBootstrapRealizationStagingModel
           << "\",\"runtime_bootstrap_reset_contract_id\":\""
           << kObjc3RuntimeBootstrapResetContractId
           << "\",\"runtime_bootstrap_reset_replay_registered_images_symbol\":\""
           << kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol
           << "\",\"runtime_bootstrap_reset_reset_replay_state_snapshot_symbol\":\""
           << kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol
           << "\",\"runtime_bootstrap_reset_lifecycle_model\":\""
           << kObjc3RuntimeBootstrapResetLifecycleModel
           << "\",\"runtime_bootstrap_reset_replay_order_model\":\""
           << kObjc3RuntimeBootstrapReplayOrderModel
           << "\",\"runtime_bootstrap_reset_image_local_init_state_reset_model\":\""
           << kObjc3RuntimeBootstrapImageLocalInitStateResetModel
           << "\",\"runtime_bootstrap_reset_bootstrap_catalog_retention_model\":\""
           << kObjc3RuntimeBootstrapCatalogRetentionModel
           << "\",\"runtime_bootstrap_lowering_bootstrap_semantics_contract_id\":\""
           << runtime_bootstrap_lowering.bootstrap_semantics_contract_id
           << "\",\"runtime_bootstrap_lowering_registration_descriptor_frontend_closure_contract_id\":\""
           << runtime_bootstrap_lowering
                  .registration_descriptor_frontend_closure_contract_id
           << "\",\"runtime_bootstrap_lowering_registration_descriptor_artifact\":\""
           << runtime_bootstrap_lowering.registration_descriptor_artifact
           << "\",\"runtime_bootstrap_lowering_boundary_model\":\""
           << runtime_bootstrap_lowering.lowering_boundary_model
           << "\",\"runtime_bootstrap_lowering_registration_descriptor_handoff_model\":\""
           << runtime_bootstrap_lowering.registration_descriptor_handoff_model
           << "\",\"runtime_bootstrap_lowering_constructor_root_symbol\":\""
           << runtime_bootstrap_lowering.constructor_root_symbol
           << "\",\"runtime_bootstrap_lowering_init_stub_symbol_prefix\":\""
           << runtime_bootstrap_lowering.constructor_init_stub_symbol_prefix
           << "\",\"runtime_bootstrap_lowering_registration_table_symbol_prefix\":\""
           << runtime_bootstrap_lowering.registration_table_symbol_prefix
           << "\",\"runtime_bootstrap_lowering_registration_entrypoint_symbol\":\""
           << runtime_bootstrap_lowering.registration_entrypoint_symbol
           << "\",\"runtime_bootstrap_lowering_global_ctor_list_model\":\""
           << runtime_bootstrap_lowering.global_ctor_list_model
           << "\",\"runtime_bootstrap_lowering_constructor_root_emission_state\":\""
           << runtime_bootstrap_lowering.constructor_root_emission_state
           << "\",\"runtime_bootstrap_lowering_init_stub_emission_state\":\""
           << runtime_bootstrap_lowering.init_stub_emission_state
           << "\",\"runtime_bootstrap_lowering_registration_table_emission_state\":\""
           << runtime_bootstrap_lowering.registration_table_emission_state
           << "\",\"runtime_bootstrap_lowering_fail_closed\":"
           << (runtime_bootstrap_lowering.fail_closed ? "true" : "false")
           << ",\"runtime_bootstrap_lowering_registration_descriptor_frontend_closure_contract_ready\":"
           << (runtime_bootstrap_lowering
                       .registration_descriptor_frontend_closure_contract_ready
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_lowering_no_bootstrap_ir_materialization_yet\":"
           << (runtime_bootstrap_lowering.no_bootstrap_ir_materialization_yet
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_lowering_ready_for_bootstrap_materialization\":"
           << (runtime_bootstrap_lowering.ready_for_bootstrap_materialization
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_lowering_replay_key\":\""
           << EscapeJsonString(runtime_bootstrap_lowering.replay_key)
           << "\",\"runtime_bootstrap_lowering_registration_descriptor_frontend_closure_replay_key\":\""
           << EscapeJsonString(runtime_bootstrap_lowering
                                   .registration_descriptor_frontend_closure_replay_key)
           << "\",\"runtime_bootstrap_lowering_failure_reason\":\""
           << EscapeJsonString(runtime_bootstrap_lowering.failure_reason)
           << "\""
           << ",\"runtime_startup_bootstrap_invariant_contract_id\":\""
           << runtime_startup_bootstrap_invariants.contract_id
           << "\",\"runtime_startup_bootstrap_invariant_duplicate_registration_policy\":\""
           << runtime_startup_bootstrap_invariants
                  .duplicate_registration_policy
           << "\",\"runtime_startup_bootstrap_invariant_realization_order_policy\":\""
           << runtime_startup_bootstrap_invariants.realization_order_policy
           << "\",\"runtime_startup_bootstrap_invariant_failure_mode\":\""
           << runtime_startup_bootstrap_invariants.failure_mode
           << "\",\"runtime_startup_bootstrap_invariant_image_local_initialization_scope\":\""
           << runtime_startup_bootstrap_invariants
                  .image_local_initialization_scope
           << "\",\"runtime_startup_bootstrap_invariant_constructor_root_uniqueness_policy\":\""
           << runtime_startup_bootstrap_invariants
                  .constructor_root_uniqueness_policy
           << "\",\"runtime_startup_bootstrap_invariant_constructor_root_consumption_model\":\""
           << runtime_startup_bootstrap_invariants
                  .constructor_root_consumption_model
           << "\",\"runtime_startup_bootstrap_invariant_startup_execution_mode\":\""
           << runtime_startup_bootstrap_invariants.startup_execution_mode
           << "\",\"runtime_startup_bootstrap_invariant_constructor_root_symbol\":\""
           << runtime_startup_bootstrap_invariants.constructor_root_symbol
           << "\",\"runtime_startup_bootstrap_invariant_registration_entrypoint_symbol\":\""
           << runtime_startup_bootstrap_invariants
                  .registration_entrypoint_symbol
           << "\",\"runtime_startup_bootstrap_invariant_manifest_authority_model\":\""
           << runtime_startup_bootstrap_invariants.manifest_authority_model
           << "\",\"runtime_startup_bootstrap_invariant_translation_unit_identity_model\":\""
           << runtime_startup_bootstrap_invariants
                  .translation_unit_identity_model
           << "\",\"runtime_startup_bootstrap_invariant_fail_closed\":"
           << (runtime_startup_bootstrap_invariants.fail_closed ? "true"
                                                                : "false")
           << ",\"runtime_startup_bootstrap_invariant_registration_manifest_contract_ready\":"
           << (runtime_startup_bootstrap_invariants
                       .registration_manifest_contract_ready
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_duplicate_registration_semantics_frozen\":"
           << (runtime_startup_bootstrap_invariants
                       .duplicate_registration_semantics_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_realization_order_semantics_frozen\":"
           << (runtime_startup_bootstrap_invariants
                       .realization_order_semantics_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_failure_mode_semantics_frozen\":"
           << (runtime_startup_bootstrap_invariants
                       .failure_mode_semantics_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_image_local_initialization_scope_frozen\":"
           << (runtime_startup_bootstrap_invariants
                       .image_local_initialization_scope_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_constructor_root_uniqueness_frozen\":"
           << (runtime_startup_bootstrap_invariants
                       .constructor_root_uniqueness_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_startup_execution_not_yet_landed\":"
           << (runtime_startup_bootstrap_invariants
                       .startup_execution_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_live_duplicate_registration_enforcement_not_yet_landed\":"
           << (runtime_startup_bootstrap_invariants
                       .live_duplicate_registration_enforcement_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_image_local_realization_not_yet_landed\":"
           << (runtime_startup_bootstrap_invariants
                       .image_local_realization_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_ready_for_bootstrap_implementation\":"
           << (runtime_startup_bootstrap_invariants
                       .ready_for_bootstrap_implementation
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_registration_manifest_replay_key\":\""
           << EscapeJsonString(
                  runtime_startup_bootstrap_invariants
                      .registration_manifest_replay_key)
           << "\",\"runtime_startup_bootstrap_invariant_replay_key\":\""
           << EscapeJsonString(runtime_startup_bootstrap_invariants.replay_key)
           << "\",\"runtime_startup_bootstrap_invariant_failure_reason\":\""
           << EscapeJsonString(
                  runtime_startup_bootstrap_invariants.failure_reason)
           << "\""
           << ",\"deterministic_property_synthesis_ivar_binding_handoff\":"
           << (property_synthesis_ivar_binding_handoff_deterministic ? "true" : "false")
           << ",\"property_synthesis_sites\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_sites
           << ",\"property_synthesis_explicit_ivar_bindings\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings
           << ",\"property_synthesis_default_ivar_bindings\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings
           << ",\"interface_owned_property_synthesis_sites\":"
           << property_synthesis_ivar_binding_summary.interface_owned_property_synthesis_sites
           << ",\"implementation_property_redeclaration_sites\":"
           << property_synthesis_ivar_binding_summary.implementation_property_redeclaration_sites
           << ",\"ivar_binding_sites\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_sites
           << ",\"ivar_binding_resolved\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_resolved
           << ",\"ivar_binding_missing\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_missing
           << ",\"ivar_binding_conflicts\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_conflicts
           << ",\"lowering_property_synthesis_ivar_binding_replay_key\":\""
           << property_synthesis_ivar_binding_replay_key
           << "\""
           << ",\"deterministic_id_class_sel_object_pointer_typecheck_handoff\":"
           << (id_class_sel_object_pointer_typecheck_contract.deterministic ? "true" : "false")
           << ",\"id_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.id_typecheck_sites
           << ",\"class_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.class_typecheck_sites
           << ",\"sel_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.sel_typecheck_sites
           << ",\"object_pointer_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.object_pointer_typecheck_sites
           << ",\"id_class_sel_object_pointer_typecheck_sites_total\":"
           << id_class_sel_object_pointer_typecheck_contract.total_typecheck_sites
           << ",\"lowering_id_class_sel_object_pointer_typecheck_replay_key\":\""
           << id_class_sel_object_pointer_typecheck_replay_key
           << "\""
           << ",\"deterministic_message_send_selector_lowering_handoff\":"
           << (message_send_selector_lowering_contract.deterministic ? "true" : "false")
           << ",\"message_send_selector_lowering_sites\":"
           << message_send_selector_lowering_contract.message_send_sites
           << ",\"message_send_selector_lowering_unary_sites\":"
           << message_send_selector_lowering_contract.unary_selector_sites
           << ",\"message_send_selector_lowering_keyword_sites\":"
           << message_send_selector_lowering_contract.keyword_selector_sites
           << ",\"message_send_selector_lowering_selector_piece_sites\":"
           << message_send_selector_lowering_contract.selector_piece_sites
           << ",\"message_send_selector_lowering_argument_expression_sites\":"
           << message_send_selector_lowering_contract.argument_expression_sites
           << ",\"message_send_selector_lowering_receiver_sites\":"
           << message_send_selector_lowering_contract.receiver_expression_sites
           << ",\"message_send_selector_lowering_selector_literal_entries\":"
           << message_send_selector_lowering_contract.selector_literal_entries
           << ",\"message_send_selector_lowering_selector_literal_characters\":"
           << message_send_selector_lowering_contract.selector_literal_characters
           << ",\"lowering_message_send_selector_lowering_replay_key\":\""
           << message_send_selector_lowering_replay_key
           << "\""
           << ",\"deterministic_dispatch_abi_marshalling_handoff\":"
           << (dispatch_abi_marshalling_contract.deterministic ? "true" : "false")
           << ",\"dispatch_abi_marshalling_message_send_sites\":"
           << dispatch_abi_marshalling_contract.message_send_sites
           << ",\"dispatch_abi_marshalling_receiver_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.receiver_slots_marshaled
           << ",\"dispatch_abi_marshalling_selector_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.selector_slots_marshaled
           << ",\"dispatch_abi_marshalling_argument_value_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_value_slots_marshaled
           << ",\"dispatch_abi_marshalling_argument_padding_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_padding_slots_marshaled
           << ",\"dispatch_abi_marshalling_argument_total_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_total_slots_marshaled
           << ",\"dispatch_abi_marshalling_total_marshaled_slots\":"
           << dispatch_abi_marshalling_contract.total_marshaled_slots
           << ",\"dispatch_abi_marshalling_runtime_dispatch_arg_slots\":"
           << dispatch_abi_marshalling_contract.runtime_dispatch_arg_slots
           << ",\"lowering_dispatch_abi_marshalling_replay_key\":\""
           << dispatch_abi_marshalling_replay_key
           << "\""
           << ",\"deterministic_nil_receiver_semantics_foldability_handoff\":"
           << (nil_receiver_semantics_foldability_contract.deterministic ? "true" : "false")
           << ",\"nil_receiver_semantics_foldability_message_send_sites\":"
           << nil_receiver_semantics_foldability_contract.message_send_sites
           << ",\"nil_receiver_semantics_foldability_receiver_nil_literal_sites\":"
           << nil_receiver_semantics_foldability_contract.receiver_nil_literal_sites
           << ",\"nil_receiver_semantics_foldability_enabled_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_semantics_enabled_sites
           << ",\"nil_receiver_semantics_foldability_foldable_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_foldable_sites
           << ",\"nil_receiver_semantics_foldability_runtime_dispatch_required_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_runtime_dispatch_required_sites
           << ",\"nil_receiver_semantics_foldability_non_nil_receiver_sites\":"
           << nil_receiver_semantics_foldability_contract.non_nil_receiver_sites
           << ",\"nil_receiver_semantics_foldability_contract_violation_sites\":"
           << nil_receiver_semantics_foldability_contract.contract_violation_sites
           << ",\"lowering_nil_receiver_semantics_foldability_replay_key\":\""
           << nil_receiver_semantics_foldability_replay_key
           << "\""
           << ",\"deterministic_super_dispatch_method_family_handoff\":"
           << (super_dispatch_method_family_contract.deterministic ? "true" : "false")
           << ",\"super_dispatch_method_family_message_send_sites\":"
           << super_dispatch_method_family_contract.message_send_sites
           << ",\"super_dispatch_method_family_receiver_super_identifier_sites\":"
           << super_dispatch_method_family_contract.receiver_super_identifier_sites
           << ",\"super_dispatch_method_family_enabled_sites\":"
           << super_dispatch_method_family_contract.super_dispatch_enabled_sites
           << ",\"super_dispatch_method_family_requires_class_context_sites\":"
           << super_dispatch_method_family_contract.super_dispatch_requires_class_context_sites
           << ",\"super_dispatch_method_family_init_sites\":"
           << super_dispatch_method_family_contract.method_family_init_sites
           << ",\"super_dispatch_method_family_copy_sites\":"
           << super_dispatch_method_family_contract.method_family_copy_sites
           << ",\"super_dispatch_method_family_mutable_copy_sites\":"
           << super_dispatch_method_family_contract.method_family_mutable_copy_sites
           << ",\"super_dispatch_method_family_new_sites\":"
           << super_dispatch_method_family_contract.method_family_new_sites
           << ",\"super_dispatch_method_family_none_sites\":"
           << super_dispatch_method_family_contract.method_family_none_sites
           << ",\"super_dispatch_method_family_returns_retained_result_sites\":"
           << super_dispatch_method_family_contract.method_family_returns_retained_result_sites
           << ",\"super_dispatch_method_family_returns_related_result_sites\":"
           << super_dispatch_method_family_contract.method_family_returns_related_result_sites
           << ",\"super_dispatch_method_family_contract_violation_sites\":"
           << super_dispatch_method_family_contract.contract_violation_sites
           << ",\"lowering_super_dispatch_method_family_replay_key\":\""
           << super_dispatch_method_family_replay_key
           << "\""
           << ",\"deterministic_runtime_link_host_link_handoff\":"
           << (runtime_link_host_link_contract.deterministic ? "true" : "false")
           << ",\"runtime_link_host_link_message_send_sites\":"
           << runtime_link_host_link_contract.message_send_sites
           << ",\"runtime_link_host_link_required_runtime_link_sites\":"
           << runtime_link_host_link_contract.runtime_link_required_sites
           << ",\"runtime_link_host_link_elided_runtime_link_sites\":"
           << runtime_link_host_link_contract.runtime_link_elided_sites
           << ",\"runtime_link_host_link_runtime_dispatch_arg_slots\":"
           << runtime_link_host_link_contract.runtime_dispatch_arg_slots
           << ",\"runtime_link_host_link_runtime_dispatch_declaration_parameter_count\":"
           << runtime_link_host_link_contract.runtime_dispatch_declaration_parameter_count
           << ",\"runtime_link_host_link_runtime_dispatch_symbol\":\""
           << runtime_link_host_link_contract.runtime_dispatch_symbol
           << "\""
           << ",\"runtime_link_host_link_default_runtime_dispatch_symbol_binding\":"
           << (runtime_link_host_link_contract.default_runtime_dispatch_symbol_binding ? "true" : "false")
           << ",\"runtime_link_host_link_contract_violation_sites\":"
           << runtime_link_host_link_contract.contract_violation_sites
           << ",\"lowering_runtime_link_host_link_replay_key\":\""
           << runtime_link_host_link_replay_key
           << "\""
           << ",\"deterministic_ownership_qualifier_lowering_handoff\":"
           << (ownership_qualifier_lowering_contract.deterministic ? "true" : "false")
           << ",\"ownership_qualifier_lowering_type_annotation_ownership_qualifier_sites\":"
           << ownership_qualifier_lowering_contract.ownership_qualifier_sites
           << ",\"ownership_qualifier_lowering_type_annotation_invalid_ownership_qualifier_sites\":"
           << ownership_qualifier_lowering_contract.invalid_ownership_qualifier_sites
           << ",\"ownership_qualifier_lowering_type_annotation_object_pointer_type_sites\":"
           << ownership_qualifier_lowering_contract.object_pointer_type_annotation_sites
           << ",\"lowering_ownership_qualifier_replay_key\":\""
           << ownership_qualifier_lowering_replay_key
           << "\""
           << ",\"deterministic_retain_release_operation_lowering_handoff\":"
           << (retain_release_operation_lowering_contract.deterministic ? "true" : "false")
           << ",\"retain_release_operation_lowering_ownership_qualified_sites\":"
           << retain_release_operation_lowering_contract.ownership_qualified_sites
           << ",\"retain_release_operation_lowering_retain_insertion_sites\":"
           << retain_release_operation_lowering_contract.retain_insertion_sites
           << ",\"retain_release_operation_lowering_release_insertion_sites\":"
           << retain_release_operation_lowering_contract.release_insertion_sites
           << ",\"retain_release_operation_lowering_autorelease_insertion_sites\":"
           << retain_release_operation_lowering_contract.autorelease_insertion_sites
           << ",\"retain_release_operation_lowering_contract_violation_sites\":"
           << retain_release_operation_lowering_contract.contract_violation_sites
           << ",\"lowering_retain_release_operation_replay_key\":\""
           << retain_release_operation_lowering_replay_key
           << "\""
           << ",\"deterministic_autoreleasepool_scope_lowering_handoff\":"
           << (autoreleasepool_scope_lowering_contract.deterministic ? "true" : "false")
           << ",\"autoreleasepool_scope_lowering_scope_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_sites
           << ",\"autoreleasepool_scope_lowering_scope_symbolized_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_symbolized_sites
           << ",\"autoreleasepool_scope_lowering_max_scope_depth\":"
           << autoreleasepool_scope_lowering_contract.max_scope_depth
           << ",\"autoreleasepool_scope_lowering_scope_entry_transition_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_entry_transition_sites
           << ",\"autoreleasepool_scope_lowering_scope_exit_transition_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_exit_transition_sites
           << ",\"autoreleasepool_scope_lowering_contract_violation_sites\":"
           << autoreleasepool_scope_lowering_contract.contract_violation_sites
           << ",\"lowering_autoreleasepool_scope_replay_key\":\""
           << autoreleasepool_scope_lowering_replay_key
           << "\""
           << ",\"deterministic_weak_unowned_semantics_lowering_handoff\":"
           << (weak_unowned_semantics_lowering_contract.deterministic ? "true" : "false")
           << ",\"weak_unowned_semantics_lowering_ownership_candidate_sites\":"
           << weak_unowned_semantics_lowering_contract.ownership_candidate_sites
           << ",\"weak_unowned_semantics_lowering_weak_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.weak_reference_sites
           << ",\"weak_unowned_semantics_lowering_unowned_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.unowned_reference_sites
           << ",\"weak_unowned_semantics_lowering_unowned_safe_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.unowned_safe_reference_sites
           << ",\"weak_unowned_semantics_lowering_conflict_sites\":"
           << weak_unowned_semantics_lowering_contract.weak_unowned_conflict_sites
           << ",\"weak_unowned_semantics_lowering_contract_violation_sites\":"
           << weak_unowned_semantics_lowering_contract.contract_violation_sites
           << ",\"lowering_weak_unowned_semantics_replay_key\":\""
           << weak_unowned_semantics_lowering_replay_key
           << "\""
           << ",\"deterministic_arc_diagnostics_fixit_lowering_handoff\":"
           << (arc_diagnostics_fixit_lowering_contract.deterministic ? "true" : "false")
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_diagnostic_candidate_sites
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_fixit_available_sites
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_profiled_sites
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_weak_unowned_conflict_diagnostic_sites
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_empty_fixit_hint_sites
           << ",\"arc_diagnostics_fixit_lowering_contract_violation_sites\":"
           << arc_diagnostics_fixit_lowering_contract.contract_violation_sites
           << ",\"lowering_arc_diagnostics_fixit_replay_key\":\""
           << arc_diagnostics_fixit_lowering_replay_key
           << "\""
           << ",\"deterministic_block_literal_capture_lowering_handoff\":"
           << (block_literal_capture_lowering_contract.deterministic ? "true" : "false")
           << ",\"block_literal_capture_lowering_block_literal_sites\":"
           << block_literal_capture_lowering_contract.block_literal_sites
           << ",\"block_literal_capture_lowering_block_parameter_entries\":"
           << block_literal_capture_lowering_contract.block_parameter_entries
           << ",\"block_literal_capture_lowering_block_capture_entries\":"
           << block_literal_capture_lowering_contract.block_capture_entries
           << ",\"block_literal_capture_lowering_block_body_statement_entries\":"
           << block_literal_capture_lowering_contract.block_body_statement_entries
           << ",\"block_literal_capture_lowering_block_empty_capture_sites\":"
           << block_literal_capture_lowering_contract.block_empty_capture_sites
           << ",\"block_literal_capture_lowering_block_nondeterministic_capture_sites\":"
           << block_literal_capture_lowering_contract.block_nondeterministic_capture_sites
           << ",\"block_literal_capture_lowering_block_non_normalized_sites\":"
           << block_literal_capture_lowering_contract.block_non_normalized_sites
           << ",\"block_literal_capture_lowering_contract_violation_sites\":"
           << block_literal_capture_lowering_contract.contract_violation_sites
           << ",\"lowering_block_literal_capture_replay_key\":\""
           << block_literal_capture_lowering_replay_key
           << "\""
           << ",\"deterministic_block_source_model_completion_handoff\":"
           << (block_source_model_completion_contract.deterministic ? "true"
                                                                   : "false")
           << ",\"block_source_model_completion_block_literal_sites\":"
           << block_source_model_completion_contract.block_literal_sites
           << ",\"block_source_model_completion_signature_entries_total\":"
           << block_source_model_completion_contract.signature_entries_total
           << ",\"block_source_model_completion_explicit_typed_parameter_entries_total\":"
           << block_source_model_completion_contract
                  .explicit_typed_parameter_entries_total
           << ",\"block_source_model_completion_implicit_parameter_entries_total\":"
           << block_source_model_completion_contract
                  .implicit_parameter_entries_total
           << ",\"block_source_model_completion_capture_inventory_entries_total\":"
           << block_source_model_completion_contract
                  .capture_inventory_entries_total
           << ",\"block_source_model_completion_byvalue_readonly_capture_entries_total\":"
           << block_source_model_completion_contract
                  .byvalue_readonly_capture_entries_total
           << ",\"block_source_model_completion_invoke_surface_entries_total\":"
           << block_source_model_completion_contract
                  .invoke_surface_entries_total
           << ",\"block_source_model_completion_non_normalized_sites\":"
           << block_source_model_completion_contract.non_normalized_sites
           << ",\"block_source_model_completion_contract_violation_sites\":"
           << block_source_model_completion_contract.contract_violation_sites
           << ",\"lowering_block_source_model_completion_replay_key\":\""
           << block_source_model_completion_replay_key
           << "\""
           << ",\"deterministic_block_source_storage_annotation_handoff\":"
           << (block_source_storage_annotation_contract.deterministic ? "true"
                                                                     : "false")
           << ",\"block_source_storage_annotation_block_literal_sites\":"
           << block_source_storage_annotation_contract.block_literal_sites
           << ",\"block_source_storage_annotation_capture_entries_total\":"
           << block_source_storage_annotation_contract.capture_entries_total
           << ",\"block_source_storage_annotation_mutated_capture_entries_total\":"
           << block_source_storage_annotation_contract
                  .mutated_capture_entries_total
           << ",\"block_source_storage_annotation_byref_capture_entries_total\":"
           << block_source_storage_annotation_contract.byref_capture_entries_total
           << ",\"block_source_storage_annotation_copy_helper_intent_sites\":"
           << block_source_storage_annotation_contract.copy_helper_intent_sites
           << ",\"block_source_storage_annotation_dispose_helper_intent_sites\":"
           << block_source_storage_annotation_contract
                  .dispose_helper_intent_sites
           << ",\"block_source_storage_annotation_heap_candidate_sites\":"
           << block_source_storage_annotation_contract.heap_candidate_sites
           << ",\"block_source_storage_annotation_expression_sites\":"
           << block_source_storage_annotation_contract.expression_sites
           << ",\"block_source_storage_annotation_global_initializer_sites\":"
           << block_source_storage_annotation_contract
                  .global_initializer_sites
           << ",\"block_source_storage_annotation_binding_initializer_sites\":"
           << block_source_storage_annotation_contract
                  .binding_initializer_sites
           << ",\"block_source_storage_annotation_assignment_value_sites\":"
           << block_source_storage_annotation_contract.assignment_value_sites
           << ",\"block_source_storage_annotation_return_value_sites\":"
           << block_source_storage_annotation_contract.return_value_sites
           << ",\"block_source_storage_annotation_call_argument_sites\":"
           << block_source_storage_annotation_contract.call_argument_sites
           << ",\"block_source_storage_annotation_message_argument_sites\":"
           << block_source_storage_annotation_contract.message_argument_sites
           << ",\"block_source_storage_annotation_non_normalized_sites\":"
           << block_source_storage_annotation_contract.non_normalized_sites
           << ",\"block_source_storage_annotation_contract_violation_sites\":"
           << block_source_storage_annotation_contract.contract_violation_sites
           << ",\"lowering_block_source_storage_annotation_replay_key\":\""
           << block_source_storage_annotation_replay_key
           << "\""
           << ",\"deterministic_block_abi_invoke_trampoline_lowering_handoff\":"
           << (block_abi_invoke_trampoline_lowering_contract.deterministic ? "true" : "false")
           << ",\"block_abi_invoke_trampoline_lowering_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.block_literal_sites
           << ",\"block_abi_invoke_trampoline_lowering_invoke_argument_slots\":"
           << block_abi_invoke_trampoline_lowering_contract.invoke_argument_slots_total
           << ",\"block_abi_invoke_trampoline_lowering_capture_word_count\":"
           << block_abi_invoke_trampoline_lowering_contract.capture_word_count_total
           << ",\"block_abi_invoke_trampoline_lowering_parameter_entries\":"
           << block_abi_invoke_trampoline_lowering_contract.parameter_entries_total
           << ",\"block_abi_invoke_trampoline_lowering_capture_entries\":"
           << block_abi_invoke_trampoline_lowering_contract.capture_entries_total
           << ",\"block_abi_invoke_trampoline_lowering_body_statement_entries\":"
           << block_abi_invoke_trampoline_lowering_contract.body_statement_entries_total
           << ",\"block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.descriptor_symbolized_sites
           << ",\"block_abi_invoke_trampoline_lowering_invoke_symbolized_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.invoke_trampoline_symbolized_sites
           << ",\"block_abi_invoke_trampoline_lowering_missing_invoke_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.missing_invoke_trampoline_sites
           << ",\"block_abi_invoke_trampoline_lowering_non_normalized_layout_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.non_normalized_layout_sites
           << ",\"block_abi_invoke_trampoline_lowering_contract_violation_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.contract_violation_sites
           << ",\"lowering_block_abi_invoke_trampoline_replay_key\":\""
           << block_abi_invoke_trampoline_lowering_replay_key
           << "\""
           << ",\"deterministic_block_storage_escape_lowering_handoff\":"
           << (block_storage_escape_lowering_contract.deterministic ? "true" : "false")
           << ",\"block_storage_escape_lowering_sites\":"
           << block_storage_escape_lowering_contract.block_literal_sites
           << ",\"block_storage_escape_lowering_mutable_capture_count\":"
           << block_storage_escape_lowering_contract.mutable_capture_count_total
           << ",\"block_storage_escape_lowering_byref_slot_count\":"
           << block_storage_escape_lowering_contract.byref_slot_count_total
           << ",\"block_storage_escape_lowering_parameter_entries\":"
           << block_storage_escape_lowering_contract.parameter_entries_total
           << ",\"block_storage_escape_lowering_capture_entries\":"
           << block_storage_escape_lowering_contract.capture_entries_total
           << ",\"block_storage_escape_lowering_body_statement_entries\":"
           << block_storage_escape_lowering_contract.body_statement_entries_total
           << ",\"block_storage_escape_lowering_requires_byref_cells_sites\":"
           << block_storage_escape_lowering_contract.requires_byref_cells_sites
           << ",\"block_storage_escape_lowering_escape_analysis_enabled_sites\":"
           << block_storage_escape_lowering_contract.escape_analysis_enabled_sites
           << ",\"block_storage_escape_lowering_escape_to_heap_sites\":"
           << block_storage_escape_lowering_contract.escape_to_heap_sites
           << ",\"block_storage_escape_lowering_escape_profile_normalized_sites\":"
           << block_storage_escape_lowering_contract.escape_profile_normalized_sites
           << ",\"block_storage_escape_lowering_byref_layout_symbolized_sites\":"
           << block_storage_escape_lowering_contract.byref_layout_symbolized_sites
           << ",\"block_storage_escape_lowering_contract_violation_sites\":"
           << block_storage_escape_lowering_contract.contract_violation_sites
           << ",\"lowering_block_storage_escape_replay_key\":\""
           << block_storage_escape_lowering_replay_key
           << "\""
           << ",\"deterministic_block_copy_dispose_lowering_handoff\":"
           << (block_copy_dispose_lowering_contract.deterministic ? "true" : "false")
           << ",\"block_copy_dispose_lowering_sites\":"
           << block_copy_dispose_lowering_contract.block_literal_sites
           << ",\"block_copy_dispose_lowering_mutable_capture_count\":"
           << block_copy_dispose_lowering_contract.mutable_capture_count_total
           << ",\"block_copy_dispose_lowering_byref_slot_count\":"
           << block_copy_dispose_lowering_contract.byref_slot_count_total
           << ",\"block_copy_dispose_lowering_parameter_entries\":"
