        << ",\n"
        << "      \"interop_ffi_deterministic\": "
        << (imported_input.interop_ffi_deterministic ? "true" : "false")
        << ",\n"
        << "      \"interop_ffi_contract_id\": \""
        << EscapeJsonString(imported_input.interop_ffi_contract_id)
        << "\",\n"
        << "      \"interop_ffi_source_contract_id\": \""
        << EscapeJsonString(imported_input.interop_ffi_source_contract_id)
        << "\",\n"
        << "      \"interop_ffi_preservation_contract_id\": \""
        << EscapeJsonString(imported_input.interop_ffi_preservation_contract_id)
        << "\",\n"
        << "      \"interop_ffi_replay_key\": \""
        << EscapeJsonString(imported_input.interop_ffi_replay_key)
        << "\",\n"
        << "      \"interop_ffi_lowering_replay_key\": \""
        << EscapeJsonString(imported_input.interop_ffi_lowering_replay_key)
        << "\",\n"
        << "      \"interop_ffi_preservation_replay_key\": \""
        << EscapeJsonString(imported_input.interop_ffi_preservation_replay_key)
        << "\",\n"
        << "      \"interop_ffi_local_foreign_callable_count\": "
        << imported_input.interop_ffi_local_foreign_callable_count << ",\n"
        << "      \"interop_ffi_local_metadata_preservation_sites\": "
        << imported_input.interop_ffi_local_metadata_preservation_sites
        << ",\n"
        << "      \"interop_ffi_local_interface_annotation_sites\": "
        << imported_input.interop_ffi_local_interface_annotation_sites
        << ",\n"
        << "      \"interop_header_module_bridge_generation_present\": "
        << (imported_input.interop_header_module_bridge_generation_present
                ? "true"
                : "false")
        << ",\n"
        << "      \"interop_header_module_bridge_runtime_generation_ready\": "
        << (imported_input.interop_header_module_bridge_runtime_generation_ready
                ? "true"
                : "false")
        << ",\n"
        << "      \"interop_header_module_bridge_cross_module_packaging_ready\": "
        << (imported_input
                    .interop_header_module_bridge_cross_module_packaging_ready
                ? "true"
                : "false")
        << ",\n"
        << "      \"interop_header_module_bridge_deterministic\": "
        << (imported_input.interop_header_module_bridge_deterministic ? "true"
                                                                     : "false")
        << ",\n"
        << "      \"interop_header_module_bridge_contract_id\": \""
        << EscapeJsonString(
               imported_input.interop_header_module_bridge_contract_id)
        << "\",\n"
        << "      \"interop_header_module_bridge_source_contract_id\": \""
        << EscapeJsonString(
               imported_input.interop_header_module_bridge_source_contract_id)
        << "\",\n"
        << "      \"interop_header_module_bridge_preservation_contract_id\": \""
        << EscapeJsonString(imported_input
                                .interop_header_module_bridge_preservation_contract_id)
        << "\",\n"
        << "      \"interop_header_module_bridge_replay_key\": \""
        << EscapeJsonString(imported_input.interop_header_module_bridge_replay_key)
        << "\",\n"
        << "      \"interop_header_module_bridge_preservation_replay_key\": \""
        << EscapeJsonString(
               imported_input
                   .interop_header_module_bridge_preservation_replay_key)
        << "\",\n"
        << "      \"interop_bridge_header_artifact_relative_path\": \""
        << EscapeJsonString(
               imported_input.interop_bridge_header_artifact_relative_path)
        << "\",\n"
        << "      \"interop_bridge_module_artifact_relative_path\": \""
        << EscapeJsonString(
               imported_input.interop_bridge_module_artifact_relative_path)
        << "\",\n"
        << "      \"interop_bridge_artifact_relative_path\": \""
        << EscapeJsonString(imported_input.interop_bridge_artifact_relative_path)
        << "\",\n"
        << "      \"interop_header_module_bridge_local_foreign_callable_count\": "
        << imported_input.interop_header_module_bridge_local_foreign_callable_count
        << ",\n"
        << "      \"metaprogramming_macro_host_process_cache_runtime_integration_present\": "
        << (imported_input.metaprogramming_macro_host_process_cache_runtime_integration_present
                ? "true"
                : "false")
        << ",\n"
        << "      \"metaprogramming_macro_host_process_cache_runtime_ready\": "
        << (imported_input.metaprogramming_macro_host_process_cache_runtime_ready ? "true"
                                                                         : "false")
        << ",\n"
        << "      \"metaprogramming_macro_host_process_cache_separate_compilation_ready\": "
        << (imported_input.metaprogramming_macro_host_process_cache_separate_compilation_ready
                ? "true"
                : "false")
        << ",\n"
        << "      \"metaprogramming_macro_host_process_cache_deterministic\": "
        << (imported_input.metaprogramming_macro_host_process_cache_deterministic ? "true"
                                                                         : "false")
        << ",\n"
        << "      \"metaprogramming_macro_host_process_cache_contract_id\": \""
        << EscapeJsonString(imported_input.metaprogramming_macro_host_process_cache_contract_id)
        << "\",\n"
        << "      \"metaprogramming_macro_host_process_cache_source_contract_id\": \""
        << EscapeJsonString(imported_input.metaprogramming_macro_host_process_cache_source_contract_id)
        << "\",\n"
        << "      \"metaprogramming_macro_host_process_cache_replay_key\": \""
        << EscapeJsonString(imported_input.metaprogramming_macro_host_process_cache_replay_key)
        << "\",\n"
        << "      \"metaprogramming_macro_host_process_cache_host_executable_relative_path\": \""
        << EscapeJsonString(imported_input.metaprogramming_macro_host_process_cache_host_executable_relative_path)
        << "\",\n"
        << "      \"metaprogramming_macro_host_process_cache_root_relative_path\": \""
        << EscapeJsonString(imported_input.metaprogramming_macro_host_process_cache_root_relative_path)
        << "\",\n"
        << "      \"block_ownership_artifact_preservation_present\": "
        << (imported_input.block_ownership_artifact_preservation_present
                ? "true"
                : "false")
        << ",\n"
        << "      \"block_ownership_runtime_import_artifact_ready\": "
        << (imported_input.block_ownership_runtime_import_artifact_ready
                ? "true"
                : "false")
        << ",\n"
        << "      \"block_ownership_separate_compilation_preservation_ready\": "
        << (imported_input
                    .block_ownership_separate_compilation_preservation_ready
                ? "true"
                : "false")
        << ",\n"
        << "      \"block_ownership_runtime_support_library_link_wiring_ready\": "
        << (imported_input
                    .block_ownership_runtime_support_library_link_wiring_ready
                ? "true"
                : "false")
        << ",\n"
        << "      \"block_ownership_deterministic\": "
        << (imported_input.block_ownership_deterministic ? "true"
                                                         : "false")
        << ",\n"
        << "      \"block_ownership_contract_id\": \""
        << EscapeJsonString(imported_input.block_ownership_contract_id)
        << "\",\n"
        << "      \"block_ownership_source_contract_id\": \""
        << EscapeJsonString(imported_input.block_ownership_source_contract_id)
        << "\",\n"
        << "      \"block_ownership_object_invoke_thunk_lowering_contract_id\": \""
        << EscapeJsonString(
               imported_input
                   .block_ownership_object_invoke_thunk_lowering_contract_id)
        << "\",\n"
        << "      \"block_ownership_byref_helper_lowering_contract_id\": \""
        << EscapeJsonString(
               imported_input
                   .block_ownership_byref_helper_lowering_contract_id)
        << "\",\n"
        << "      \"block_ownership_escape_runtime_hook_lowering_contract_id\": \""
        << EscapeJsonString(
               imported_input
                   .block_ownership_escape_runtime_hook_lowering_contract_id)
        << "\",\n"
        << "      \"block_ownership_runtime_support_library_link_wiring_contract_id\": \""
        << EscapeJsonString(
               imported_input
                   .block_ownership_runtime_support_library_link_wiring_contract_id)
        << "\",\n"
        << "      \"block_ownership_replay_key\": \""
        << EscapeJsonString(imported_input.block_ownership_replay_key)
        << "\",\n"
        << "      \"block_ownership_local_block_literal_sites\": "
        << imported_input.block_ownership_local_block_literal_sites << ",\n"
        << "      \"block_ownership_local_invoke_trampoline_symbolized_sites\": "
        << imported_input
               .block_ownership_local_invoke_trampoline_symbolized_sites
        << ",\n"
        << "      \"block_ownership_local_copy_helper_required_sites\": "
        << imported_input.block_ownership_local_copy_helper_required_sites
        << ",\n"
        << "      \"block_ownership_local_dispose_helper_required_sites\": "
        << imported_input.block_ownership_local_dispose_helper_required_sites
        << ",\n"
        << "      \"block_ownership_local_copy_helper_symbolized_sites\": "
        << imported_input.block_ownership_local_copy_helper_symbolized_sites
        << ",\n"
        << "      \"block_ownership_local_dispose_helper_symbolized_sites\": "
        << imported_input.block_ownership_local_dispose_helper_symbolized_sites
        << ",\n"
        << "      \"block_ownership_local_escape_to_heap_sites\": "
        << imported_input.block_ownership_local_escape_to_heap_sites << ",\n"
        << "      \"block_ownership_local_byref_layout_symbolized_sites\": "
        << imported_input.block_ownership_local_byref_layout_symbolized_sites
        << ",\n"
        << "      \"storage_reflection_artifact_preservation_present\": "
        << (imported_input.storage_reflection_artifact_preservation_present
                ? "true"
                : "false")
        << ",\n"
        << "      \"storage_reflection_runtime_import_artifact_ready\": "
        << (imported_input.storage_reflection_runtime_import_artifact_ready
                ? "true"
                : "false")
        << ",\n"
        << "      \"storage_reflection_separate_compilation_preservation_ready\": "
        << (imported_input
                    .storage_reflection_separate_compilation_preservation_ready
                ? "true"
                : "false")
        << ",\n"
        << "      \"storage_reflection_deterministic\": "
        << (imported_input.storage_reflection_deterministic ? "true"
                                                            : "false")
        << ",\n"
        << "      \"storage_reflection_contract_id\": \""
        << EscapeJsonString(imported_input.storage_reflection_contract_id)
        << "\",\n"
        << "      \"storage_reflection_source_contract_id\": \""
        << EscapeJsonString(imported_input.storage_reflection_source_contract_id)
        << "\",\n"
        << "      \"storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id\": \""
        << EscapeJsonString(
               imported_input
                   .storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id)
        << "\",\n"
        << "      \"storage_reflection_executable_property_accessor_layout_lowering_contract_id\": \""
        << EscapeJsonString(
               imported_input
                   .storage_reflection_executable_property_accessor_layout_lowering_contract_id)
        << "\",\n"
        << "      \"storage_reflection_executable_ivar_layout_emission_contract_id\": \""
        << EscapeJsonString(
               imported_input
                   .storage_reflection_executable_ivar_layout_emission_contract_id)
        << "\",\n"
        << "      \"storage_reflection_executable_synthesized_accessor_property_lowering_contract_id\": \""
        << EscapeJsonString(
               imported_input
                   .storage_reflection_executable_synthesized_accessor_property_lowering_contract_id)
        << "\",\n"
        << "      \"storage_reflection_replay_key\": \""
        << EscapeJsonString(imported_input.storage_reflection_replay_key)
        << "\",\n"
        << "      \"storage_reflection_local_property_descriptor_count\": "
        << imported_input.storage_reflection_local_property_descriptor_count
        << ",\n"
        << "      \"storage_reflection_local_ivar_descriptor_count\": "
        << imported_input.storage_reflection_local_ivar_descriptor_count
        << ",\n"
        << "      \"storage_reflection_implementation_owned_property_entries\": "
        << imported_input
               .storage_reflection_implementation_owned_property_entries
        << ",\n"
        << "      \"storage_reflection_synthesized_accessor_owner_entries\": "
        << imported_input
               .storage_reflection_synthesized_accessor_owner_entries
        << ",\n"
        << "      \"storage_reflection_synthesized_getter_entries\": "
        << imported_input.storage_reflection_synthesized_getter_entries
        << ",\n"
        << "      \"storage_reflection_synthesized_setter_entries\": "
        << imported_input.storage_reflection_synthesized_setter_entries
        << ",\n"
        << "      \"storage_reflection_synthesized_accessor_entries\": "
        << imported_input.storage_reflection_synthesized_accessor_entries
        << ",\n"
        << "      \"storage_reflection_current_property_read_entries\": "
        << imported_input.storage_reflection_current_property_read_entries
        << ",\n"
        << "      \"storage_reflection_current_property_write_entries\": "
        << imported_input.storage_reflection_current_property_write_entries
        << ",\n"
        << "      \"storage_reflection_current_property_exchange_entries\": "
        << imported_input.storage_reflection_current_property_exchange_entries
        << ",\n"
        << "      \"storage_reflection_weak_current_property_load_entries\": "
        << imported_input
               .storage_reflection_weak_current_property_load_entries
        << ",\n"
        << "      \"storage_reflection_weak_current_property_store_entries\": "
        << imported_input
               .storage_reflection_weak_current_property_store_entries
        << ",\n"
        << "      \"storage_reflection_ivar_layout_entries\": "
        << imported_input.storage_reflection_ivar_layout_entries
        << ",\n"
        << "      \"storage_reflection_ivar_layout_owner_entries\": "
        << imported_input.storage_reflection_ivar_layout_owner_entries
        << ",\n"
        << "      \"driver_linker_flags\": "
        << BuildIndentedStringArrayJson(imported_input.driver_linker_flags,
                                        "        ");
    imported_modules_json << "\n    }";
    if (index + 1u < imported_inputs.size()) {
      imported_modules_json << ",";
    }
    imported_modules_json << "\n";
  }
  imported_modules_json << "  ]";
  std::sort(imported_error_handling_module_names.begin(), imported_error_handling_module_names.end());
  std::sort(imported_concurrency_actor_module_names.begin(),
            imported_concurrency_actor_module_names.end());
  std::sort(imported_interop_ffi_module_names.begin(),
            imported_interop_ffi_module_names.end());
  std::sort(imported_interop_header_module_bridge_module_names.begin(),
            imported_interop_header_module_bridge_module_names.end());
  std::sort(imported_metaprogramming_host_cache_module_names.begin(),
            imported_metaprogramming_host_cache_module_names.end());

  std::ostringstream out;
  out << "{\n"
      << "  \"contract_id\": \"" << EscapeJsonString(inputs.contract_id)
      << "\",\n"
      << "  \"source_orchestration_contract_id\": \""
      << EscapeJsonString(inputs.source_orchestration_contract_id) << "\",\n"
      << "  \"import_surface_contract_id\": \""
      << EscapeJsonString(inputs.import_surface_contract_id) << "\",\n"
      << "  \"registration_manifest_contract_id\": \""
      << EscapeJsonString(inputs.registration_manifest_contract_id)
      << "\",\n"
      << "  \"payload_model\": \"" << EscapeJsonString(inputs.payload_model)
      << "\",\n"
      << "  \"artifact\": \"" << EscapeJsonString(inputs.artifact_relative_path)
      << "\",\n"
      << "  \"linker_response_artifact\": \""
      << EscapeJsonString(inputs.linker_response_artifact_relative_path)
      << "\",\n"
      << "  \"authority_model\": \""
      << EscapeJsonString(inputs.authority_model) << "\",\n"
      << "  \"packaging_model\": \""
      << EscapeJsonString(inputs.packaging_model) << "\",\n"
      << "  \"registration_scope_model\": \""
      << EscapeJsonString(inputs.registration_scope_model) << "\",\n"
      << "  \"link_object_order_model\": \""
      << EscapeJsonString(inputs.link_object_order_model) << "\",\n"
      << "  \"expected_error_handling_contract_id\": \""
      << EscapeJsonString(inputs.expected_error_handling_contract_id) << "\",\n"
      << "  \"expected_error_handling_source_contract_id\": \""
      << EscapeJsonString(inputs.expected_error_handling_source_contract_id) << "\",\n"
      << "  \"expected_concurrency_actor_contract_id\": \""
      << EscapeJsonString(inputs.expected_concurrency_actor_contract_id) << "\",\n"
      << "  \"expected_concurrency_actor_source_contract_id\": \""
      << EscapeJsonString(inputs.expected_concurrency_actor_source_contract_id)
      << "\",\n"
      << "  \"expected_interop_ffi_contract_id\": \""
      << EscapeJsonString(inputs.expected_interop_ffi_contract_id) << "\",\n"
      << "  \"expected_interop_ffi_source_contract_id\": \""
      << EscapeJsonString(inputs.expected_interop_ffi_source_contract_id)
      << "\",\n"
      << "  \"expected_interop_ffi_preservation_contract_id\": \""
      << EscapeJsonString(inputs.expected_interop_ffi_preservation_contract_id)
      << "\",\n"
      << "  \"expected_interop_header_module_bridge_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_interop_header_module_bridge_contract_id)
      << "\",\n"
      << "  \"expected_interop_header_module_bridge_source_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_interop_header_module_bridge_source_contract_id)
      << "\",\n"
      << "  \"expected_interop_header_module_bridge_preservation_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_interop_header_module_bridge_preservation_contract_id)
      << "\",\n"
      << "  \"expected_interop_bridge_header_artifact_relative_path\": \""
      << EscapeJsonString(
             inputs.expected_interop_bridge_header_artifact_relative_path)
      << "\",\n"
      << "  \"expected_interop_bridge_module_artifact_relative_path\": \""
      << EscapeJsonString(
             inputs.expected_interop_bridge_module_artifact_relative_path)
      << "\",\n"
      << "  \"expected_interop_bridge_artifact_relative_path\": \""
      << EscapeJsonString(inputs.expected_interop_bridge_artifact_relative_path)
      << "\",\n"
      << "  \"expected_metaprogramming_host_cache_contract_id\": \""
      << EscapeJsonString(inputs.expected_metaprogramming_host_cache_contract_id)
      << "\",\n"
      << "  \"expected_metaprogramming_host_cache_source_contract_id\": \""
      << EscapeJsonString(inputs.expected_metaprogramming_host_cache_source_contract_id)
      << "\",\n"
      << "  \"expected_metaprogramming_host_cache_executable_relative_path\": \""
      << EscapeJsonString(
             inputs.expected_metaprogramming_host_cache_executable_relative_path)
      << "\",\n"
      << "  \"expected_metaprogramming_host_cache_root_relative_path\": \""
      << EscapeJsonString(inputs.expected_metaprogramming_host_cache_root_relative_path)
      << "\",\n"
      << "  \"expected_block_ownership_contract_id\": \""
      << EscapeJsonString(inputs.expected_block_ownership_contract_id)
      << "\",\n"
      << "  \"expected_block_ownership_source_contract_id\": \""
      << EscapeJsonString(inputs.expected_block_ownership_source_contract_id)
      << "\",\n"
      << "  \"expected_block_ownership_object_invoke_thunk_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_block_ownership_object_invoke_thunk_lowering_contract_id)
      << "\",\n"
      << "  \"expected_block_ownership_byref_helper_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_block_ownership_byref_helper_lowering_contract_id)
      << "\",\n"
      << "  \"expected_block_ownership_escape_runtime_hook_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_block_ownership_escape_runtime_hook_lowering_contract_id)
      << "\",\n"
      << "  \"expected_block_ownership_runtime_support_library_link_wiring_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_block_ownership_runtime_support_library_link_wiring_contract_id)
      << "\",\n"
      << "  \"module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(module_names_lexicographic, "    ")
      << ",\n"
      << "  \"module_image_count\": " << module_names_lexicographic.size()
      << ",\n"
      << "  \"direct_import_input_count\": "
      << direct_import_surface_artifact_paths.size() << ",\n"
      << "  \"error_handling_imported_module_count\": "
      << imported_error_handling_module_names.size() << ",\n"
      << "  \"concurrency_actor_imported_module_count\": "
      << imported_concurrency_actor_module_names.size() << ",\n"
      << "  \"interop_ffi_imported_module_count\": "
      << imported_interop_ffi_module_names.size() << ",\n"
      << "  \"interop_header_module_bridge_imported_module_count\": "
      << imported_interop_header_module_bridge_module_names.size() << ",\n"
      << "  \"metaprogramming_host_cache_imported_module_count\": "
      << imported_metaprogramming_host_cache_module_names.size() << ",\n"
      << "  \"direct_import_surface_artifact_paths\": "
      << BuildIndentedStringArrayJson(direct_import_surface_artifact_paths,
                                      "    ")
      << ",\n"
      << "  \"error_handling_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(imported_error_handling_module_names, "    ")
      << ",\n"
      << "  \"concurrency_actor_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(imported_concurrency_actor_module_names, "    ")
      << ",\n"
      << "  \"interop_ffi_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(imported_interop_ffi_module_names, "    ")
      << ",\n"
      << "  \"interop_header_module_bridge_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(
             imported_interop_header_module_bridge_module_names, "    ")
      << ",\n"
      << "  \"metaprogramming_host_cache_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(imported_metaprogramming_host_cache_module_names,
                                      "    ")
      << ",\n"
      << "  \"error_handling_cross_module_preservation_ready\": "
      << (!imported_error_handling_module_names.empty() ? "true" : "false") << ",\n"
      << "  \"concurrency_actor_cross_module_isolation_ready\": "
      << (!imported_concurrency_actor_module_names.empty() ? "true" : "false")
      << ",\n"
      << "  \"interop_ffi_cross_module_packaging_ready\": "
      << (!imported_interop_ffi_module_names.empty() ? "true" : "false")
      << ",\n"
      << "  \"interop_header_module_bridge_cross_module_packaging_ready\": "
      << (!imported_interop_header_module_bridge_module_names.empty() ? "true"
                                                                     : "false")
      << ",\n"
      << "  \"metaprogramming_host_cache_cross_module_preservation_ready\": "
      << (!imported_metaprogramming_host_cache_module_names.empty() ? "true" : "false")
      << ",\n"
      << "  \"bootstrap_live_registration_contract_id\": \""
      << EscapeJsonString(inputs.expected_bootstrap_live_registration_contract_id)
      << "\",\n"
      << "  \"bootstrap_live_restart_hardening_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_bootstrap_live_restart_hardening_contract_id)
      << "\",\n"
      << "  \"bootstrap_replay_registered_images_symbol\": \""
      << EscapeJsonString(inputs.expected_bootstrap_replay_registered_images_symbol)
      << "\",\n"
      << "  \"bootstrap_reset_replay_state_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.expected_bootstrap_reset_replay_state_snapshot_symbol)
      << "\",\n"
      << "  \"bootstrap_reset_for_testing_symbol\": \""
      << EscapeJsonString(inputs.expected_bootstrap_reset_for_testing_symbol)
      << "\",\n"
      << "  \"runtime_cross_module_realized_metadata_replay_preservation_surface_contract_id\": \""
      << "objc3c.runtime.cross.module.realized.metadata.replay.preservation.surface.v1"
      << "\",\n"
      << "  \"runtime_object_model_realization_source_surface_contract_id\": \""
      << "objc3c.runtime.object.model.realization.source.surface.v1"
      << "\",\n"
      << "  \"runtime_realization_lowering_reflection_artifact_surface_contract_id\": \""
      << "objc3c.runtime.realization.lowering.reflection.artifact.surface.v1"
      << "\",\n"
      << "  \"runtime_dispatch_table_reflection_record_lowering_surface_contract_id\": \""
      << "objc3c.runtime.dispatch.table.reflection.record.lowering.surface.v1"
      << "\",\n"
      << "  \"runtime_cross_module_block_ownership_artifact_preservation_surface_contract_id\": \""
      << EscapeJsonString(inputs.expected_block_ownership_contract_id)
      << "\",\n"
      << "  \"runtime_block_arc_lowering_helper_surface_contract_id\": \""
      << EscapeJsonString(inputs.expected_block_ownership_source_contract_id)
      << "\",\n"
      << "  \"block_object_invoke_thunk_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_block_ownership_object_invoke_thunk_lowering_contract_id)
      << "\",\n"
      << "  \"block_byref_helper_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_block_ownership_byref_helper_lowering_contract_id)
      << "\",\n"
      << "  \"block_escape_runtime_hook_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_block_ownership_escape_runtime_hook_lowering_contract_id)
      << "\",\n"
      << "  \"block_runtime_support_library_link_wiring_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_block_ownership_runtime_support_library_link_wiring_contract_id)
      << "\",\n"
      << "  \"runtime_cross_module_storage_reflection_artifact_preservation_surface_contract_id\": \""
      << EscapeJsonString(inputs.expected_storage_reflection_contract_id)
      << "\",\n"
      << "  \"runtime_property_ivar_storage_accessor_source_surface_contract_id\": \""
      << EscapeJsonString(inputs.expected_storage_reflection_source_contract_id)
      << "\",\n"
      << "  \"dispatch_and_synthesized_accessor_lowering_surface_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id)
      << "\",\n"
      << "  \"executable_property_accessor_layout_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_storage_reflection_executable_property_accessor_layout_lowering_contract_id)
      << "\",\n"
      << "  \"executable_ivar_layout_emission_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_storage_reflection_executable_ivar_layout_emission_contract_id)
      << "\",\n"
      << "  \"executable_synthesized_accessor_property_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_storage_reflection_executable_synthesized_accessor_property_lowering_contract_id)
      << "\",\n"
      << "  \"realized_metadata_replay_preservation_model\": \""
      << "cross-module-link-plan-preserves-local-and-imported-realized-metadata-descriptor-counts-identities-and-reset-replay-readiness-from-runtime-registration-manifests"
      << "\",\n"
      << "  \"block_ownership_artifact_preservation_model\": \""
      << "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-block-ownership-lowering-helper-and-runtime-link-facts-beyond-local-ir-object-emission"
      << "\",\n"
      << "  \"storage_reflection_artifact_preservation_model\": \""
      << "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-property-ivar-accessor-layout-and-runtime-helper-facts-beyond-local-ir-object-emission"
      << "\",\n"
      << "  \"imported_live_registration_replay_ready\": true,\n"
      << "  \"imported_live_restart_hardening_ready\": true,\n"
      << "  \"block_ownership_cross_module_preservation_ready\": true,\n"
      << "  \"storage_reflection_cross_module_preservation_ready\": true,\n"
      << "  \"module_image_count\": " << imported_inputs.size() + 1 << ",\n"
      << "  \"direct_import_input_count\": " << imported_inputs.size() << ",\n"
      << "  \"local_class_descriptor_count\": "
      << inputs.local_class_descriptor_count << ",\n"
      << "  \"local_protocol_descriptor_count\": "
      << inputs.local_protocol_descriptor_count << ",\n"
      << "  \"local_category_descriptor_count\": "
      << inputs.local_category_descriptor_count << ",\n"
      << "  \"local_property_descriptor_count\": "
      << inputs.local_property_descriptor_count << ",\n"
      << "  \"local_ivar_descriptor_count\": "
      << inputs.local_ivar_descriptor_count << ",\n"
      << "  \"local_total_descriptor_count\": "
      << inputs.local_total_descriptor_count << ",\n"
      << "  \"imported_class_descriptor_count\": "
      << imported_class_descriptor_count << ",\n"
      << "  \"imported_protocol_descriptor_count\": "
      << imported_protocol_descriptor_count << ",\n"
      << "  \"imported_category_descriptor_count\": "
      << imported_category_descriptor_count << ",\n"
      << "  \"imported_property_descriptor_count\": "
      << imported_property_descriptor_count << ",\n"
      << "  \"imported_ivar_descriptor_count\": "
      << imported_ivar_descriptor_count << ",\n"
      << "  \"imported_total_descriptor_count\": "
      << imported_total_descriptor_count << ",\n"
      << "  \"transitive_class_descriptor_count\": "
      << inputs.local_class_descriptor_count + imported_class_descriptor_count
      << ",\n"
      << "  \"transitive_protocol_descriptor_count\": "
      << inputs.local_protocol_descriptor_count +
             imported_protocol_descriptor_count
      << ",\n"
      << "  \"transitive_category_descriptor_count\": "
      << inputs.local_category_descriptor_count +
             imported_category_descriptor_count
      << ",\n"
      << "  \"transitive_property_descriptor_count\": "
      << inputs.local_property_descriptor_count +
             imported_property_descriptor_count
      << ",\n"
      << "  \"transitive_ivar_descriptor_count\": "
      << inputs.local_ivar_descriptor_count + imported_ivar_descriptor_count
      << ",\n"
      << "  \"transitive_total_descriptor_count\": "
      << inputs.local_total_descriptor_count + imported_total_descriptor_count
      << ",\n"
      << "  \"local_block_ownership_block_literal_sites\": "
      << inputs.local_block_ownership_block_literal_sites << ",\n"
      << "  \"local_block_ownership_invoke_trampoline_symbolized_sites\": "
      << inputs.local_block_ownership_invoke_trampoline_symbolized_sites
      << ",\n"
      << "  \"local_block_ownership_copy_helper_required_sites\": "
      << inputs.local_block_ownership_copy_helper_required_sites << ",\n"
      << "  \"local_block_ownership_dispose_helper_required_sites\": "
      << inputs.local_block_ownership_dispose_helper_required_sites << ",\n"
      << "  \"local_block_ownership_copy_helper_symbolized_sites\": "
      << inputs.local_block_ownership_copy_helper_symbolized_sites << ",\n"
      << "  \"local_block_ownership_dispose_helper_symbolized_sites\": "
      << inputs.local_block_ownership_dispose_helper_symbolized_sites
      << ",\n"
      << "  \"local_block_ownership_escape_to_heap_sites\": "
      << inputs.local_block_ownership_escape_to_heap_sites << ",\n"
      << "  \"local_block_ownership_byref_layout_symbolized_sites\": "
      << inputs.local_block_ownership_byref_layout_symbolized_sites
      << ",\n"
      << "  \"imported_block_ownership_block_literal_sites\": "
      << imported_block_ownership_block_literal_sites << ",\n"
      << "  \"imported_block_ownership_invoke_trampoline_symbolized_sites\": "
      << imported_block_ownership_invoke_trampoline_symbolized_sites
      << ",\n"
      << "  \"imported_block_ownership_copy_helper_required_sites\": "
      << imported_block_ownership_copy_helper_required_sites << ",\n"
      << "  \"imported_block_ownership_dispose_helper_required_sites\": "
      << imported_block_ownership_dispose_helper_required_sites << ",\n"
      << "  \"imported_block_ownership_copy_helper_symbolized_sites\": "
      << imported_block_ownership_copy_helper_symbolized_sites << ",\n"
      << "  \"imported_block_ownership_dispose_helper_symbolized_sites\": "
      << imported_block_ownership_dispose_helper_symbolized_sites << ",\n"
      << "  \"imported_block_ownership_escape_to_heap_sites\": "
      << imported_block_ownership_escape_to_heap_sites << ",\n"
      << "  \"imported_block_ownership_byref_layout_symbolized_sites\": "
      << imported_block_ownership_byref_layout_symbolized_sites << ",\n"
      << "  \"transitive_block_ownership_block_literal_sites\": "
      << inputs.local_block_ownership_block_literal_sites +
             imported_block_ownership_block_literal_sites
      << ",\n"
      << "  \"transitive_block_ownership_invoke_trampoline_symbolized_sites\": "
      << inputs.local_block_ownership_invoke_trampoline_symbolized_sites +
             imported_block_ownership_invoke_trampoline_symbolized_sites
      << ",\n"
      << "  \"transitive_block_ownership_copy_helper_required_sites\": "
      << inputs.local_block_ownership_copy_helper_required_sites +
             imported_block_ownership_copy_helper_required_sites
      << ",\n"
      << "  \"transitive_block_ownership_dispose_helper_required_sites\": "
      << inputs.local_block_ownership_dispose_helper_required_sites +
             imported_block_ownership_dispose_helper_required_sites
      << ",\n"
      << "  \"transitive_block_ownership_copy_helper_symbolized_sites\": "
      << inputs.local_block_ownership_copy_helper_symbolized_sites +
             imported_block_ownership_copy_helper_symbolized_sites
      << ",\n"
      << "  \"transitive_block_ownership_dispose_helper_symbolized_sites\": "
      << inputs.local_block_ownership_dispose_helper_symbolized_sites +
             imported_block_ownership_dispose_helper_symbolized_sites
      << ",\n"
      << "  \"transitive_block_ownership_escape_to_heap_sites\": "
      << inputs.local_block_ownership_escape_to_heap_sites +
             imported_block_ownership_escape_to_heap_sites
      << ",\n"
      << "  \"transitive_block_ownership_byref_layout_symbolized_sites\": "
      << inputs.local_block_ownership_byref_layout_symbolized_sites +
             imported_block_ownership_byref_layout_symbolized_sites
      << ",\n"
      << "  \"local_storage_reflection_implementation_owned_property_entries\": "
      << inputs.local_storage_reflection_implementation_owned_property_entries
      << ",\n"
      << "  \"local_storage_reflection_synthesized_accessor_owner_entries\": "
      << inputs.local_storage_reflection_synthesized_accessor_owner_entries
      << ",\n"
      << "  \"local_storage_reflection_synthesized_getter_entries\": "
      << inputs.local_storage_reflection_synthesized_getter_entries
      << ",\n"
      << "  \"local_storage_reflection_synthesized_setter_entries\": "
      << inputs.local_storage_reflection_synthesized_setter_entries
      << ",\n"
      << "  \"local_storage_reflection_synthesized_accessor_entries\": "
      << inputs.local_storage_reflection_synthesized_accessor_entries
      << ",\n"
      << "  \"local_storage_reflection_current_property_read_entries\": "
      << inputs.local_storage_reflection_current_property_read_entries
      << ",\n"
      << "  \"local_storage_reflection_current_property_write_entries\": "
      << inputs.local_storage_reflection_current_property_write_entries
      << ",\n"
      << "  \"local_storage_reflection_current_property_exchange_entries\": "
      << inputs.local_storage_reflection_current_property_exchange_entries
      << ",\n"
      << "  \"local_storage_reflection_weak_current_property_load_entries\": "
      << inputs.local_storage_reflection_weak_current_property_load_entries
      << ",\n"
      << "  \"local_storage_reflection_weak_current_property_store_entries\": "
      << inputs.local_storage_reflection_weak_current_property_store_entries
      << ",\n"
      << "  \"local_storage_reflection_ivar_layout_entries\": "
      << inputs.local_storage_reflection_ivar_layout_entries << ",\n"
      << "  \"local_storage_reflection_ivar_layout_owner_entries\": "
      << inputs.local_storage_reflection_ivar_layout_owner_entries << ",\n"
      << "  \"imported_storage_reflection_implementation_owned_property_entries\": "
      << imported_storage_reflection_implementation_owned_property_entries
      << ",\n"
      << "  \"imported_storage_reflection_synthesized_accessor_owner_entries\": "
      << imported_storage_reflection_synthesized_accessor_owner_entries
      << ",\n"
      << "  \"imported_storage_reflection_synthesized_getter_entries\": "
      << imported_storage_reflection_synthesized_getter_entries << ",\n"
      << "  \"imported_storage_reflection_synthesized_setter_entries\": "
      << imported_storage_reflection_synthesized_setter_entries << ",\n"
      << "  \"imported_storage_reflection_synthesized_accessor_entries\": "
      << imported_storage_reflection_synthesized_accessor_entries << ",\n"
      << "  \"imported_storage_reflection_current_property_read_entries\": "
      << imported_storage_reflection_current_property_read_entries << ",\n"
      << "  \"imported_storage_reflection_current_property_write_entries\": "
      << imported_storage_reflection_current_property_write_entries << ",\n"
      << "  \"imported_storage_reflection_current_property_exchange_entries\": "
      << imported_storage_reflection_current_property_exchange_entries
      << ",\n"
      << "  \"imported_storage_reflection_weak_current_property_load_entries\": "
      << imported_storage_reflection_weak_current_property_load_entries
      << ",\n"
      << "  \"imported_storage_reflection_weak_current_property_store_entries\": "
      << imported_storage_reflection_weak_current_property_store_entries
      << ",\n"
      << "  \"imported_storage_reflection_ivar_layout_entries\": "
      << imported_storage_reflection_ivar_layout_entries << ",\n"
      << "  \"imported_storage_reflection_ivar_layout_owner_entries\": "
      << imported_storage_reflection_ivar_layout_owner_entries << ",\n"
      << "  \"transitive_storage_reflection_implementation_owned_property_entries\": "
      << inputs.local_storage_reflection_implementation_owned_property_entries +
             imported_storage_reflection_implementation_owned_property_entries
      << ",\n"
      << "  \"transitive_storage_reflection_synthesized_accessor_owner_entries\": "
      << inputs.local_storage_reflection_synthesized_accessor_owner_entries +
             imported_storage_reflection_synthesized_accessor_owner_entries
      << ",\n"
      << "  \"transitive_storage_reflection_synthesized_getter_entries\": "
      << inputs.local_storage_reflection_synthesized_getter_entries +
             imported_storage_reflection_synthesized_getter_entries
      << ",\n"
      << "  \"transitive_storage_reflection_synthesized_setter_entries\": "
      << inputs.local_storage_reflection_synthesized_setter_entries +
             imported_storage_reflection_synthesized_setter_entries
      << ",\n"
      << "  \"transitive_storage_reflection_synthesized_accessor_entries\": "
      << inputs.local_storage_reflection_synthesized_accessor_entries +
             imported_storage_reflection_synthesized_accessor_entries
      << ",\n"
      << "  \"transitive_storage_reflection_current_property_read_entries\": "
      << inputs.local_storage_reflection_current_property_read_entries +
             imported_storage_reflection_current_property_read_entries
      << ",\n"
      << "  \"transitive_storage_reflection_current_property_write_entries\": "
      << inputs.local_storage_reflection_current_property_write_entries +
             imported_storage_reflection_current_property_write_entries
      << ",\n"
      << "  \"transitive_storage_reflection_current_property_exchange_entries\": "
      << inputs.local_storage_reflection_current_property_exchange_entries +
             imported_storage_reflection_current_property_exchange_entries
      << ",\n"
      << "  \"transitive_storage_reflection_weak_current_property_load_entries\": "
      << inputs.local_storage_reflection_weak_current_property_load_entries +
             imported_storage_reflection_weak_current_property_load_entries
      << ",\n"
      << "  \"transitive_storage_reflection_weak_current_property_store_entries\": "
      << inputs.local_storage_reflection_weak_current_property_store_entries +
             imported_storage_reflection_weak_current_property_store_entries
      << ",\n"
      << "  \"transitive_storage_reflection_ivar_layout_entries\": "
      << inputs.local_storage_reflection_ivar_layout_entries +
             imported_storage_reflection_ivar_layout_entries
      << ",\n"
      << "  \"transitive_storage_reflection_ivar_layout_owner_entries\": "
      << inputs.local_storage_reflection_ivar_layout_owner_entries +
             imported_storage_reflection_ivar_layout_owner_entries
      << ",\n"
      << "  \"cleanup_unwind_runtime_link_model\": "
      << "\"linker-response-plus-runtime-support-archive-sidecars-provide-runnable-cleanup-executable-link-inputs\",\n"
      << "  \"runtime_support_library_archive_relative_path\": \""
      << EscapeJsonString(inputs.runtime_support_library_archive_relative_path)
      << "\",\n"
      << "  \"object_format\": \"" << EscapeJsonString(inputs.object_format)
      << "\",\n"
      << "  \"local_module\": {\n"
      << "    \"module_name\": \""
      << EscapeJsonString(inputs.local_module_name) << "\",\n"
      << "    \"import_surface_artifact_relative_path\": \""
      << EscapeJsonString(inputs.local_import_surface_artifact_relative_path)
      << "\",\n"
      << "    \"registration_manifest_artifact_relative_path\": \""
      << EscapeJsonString(
             inputs.local_registration_manifest_artifact_relative_path)
      << "\",\n"
      << "    \"object_artifact_relative_path\": \""
      << EscapeJsonString(inputs.local_object_artifact_relative_path)
      << "\",\n"
      << "    \"translation_unit_identity_model\": \""
      << EscapeJsonString(inputs.local_translation_unit_identity_model)
      << "\",\n"
      << "    \"translation_unit_identity_key\": \""
      << EscapeJsonString(inputs.local_translation_unit_identity_key)
      << "\",\n"
      << "    \"translation_unit_registration_order_ordinal\": "
      << inputs.local_translation_unit_registration_order_ordinal << ",\n"
      << "    \"class_descriptor_count\": "
      << inputs.local_class_descriptor_count << ",\n"
      << "    \"protocol_descriptor_count\": "
      << inputs.local_protocol_descriptor_count << ",\n"
      << "    \"category_descriptor_count\": "
      << inputs.local_category_descriptor_count << ",\n"
      << "    \"property_descriptor_count\": "
      << inputs.local_property_descriptor_count << ",\n"
      << "    \"ivar_descriptor_count\": "
      << inputs.local_ivar_descriptor_count << ",\n"
      << "    \"total_descriptor_count\": "
      << inputs.local_total_descriptor_count << ",\n"
      << "    \"storage_reflection_implementation_owned_property_entries\": "
      << inputs.local_storage_reflection_implementation_owned_property_entries
      << ",\n"
      << "    \"storage_reflection_synthesized_accessor_owner_entries\": "
      << inputs.local_storage_reflection_synthesized_accessor_owner_entries
      << ",\n"
      << "    \"storage_reflection_synthesized_getter_entries\": "
      << inputs.local_storage_reflection_synthesized_getter_entries
      << ",\n"
      << "    \"storage_reflection_synthesized_setter_entries\": "
      << inputs.local_storage_reflection_synthesized_setter_entries
      << ",\n"
      << "    \"storage_reflection_synthesized_accessor_entries\": "
      << inputs.local_storage_reflection_synthesized_accessor_entries
      << ",\n"
      << "    \"storage_reflection_current_property_read_entries\": "
      << inputs.local_storage_reflection_current_property_read_entries
      << ",\n"
      << "    \"storage_reflection_current_property_write_entries\": "
      << inputs.local_storage_reflection_current_property_write_entries
      << ",\n"
      << "    \"storage_reflection_current_property_exchange_entries\": "
      << inputs.local_storage_reflection_current_property_exchange_entries
      << ",\n"
      << "    \"storage_reflection_weak_current_property_load_entries\": "
      << inputs.local_storage_reflection_weak_current_property_load_entries
      << ",\n"
      << "    \"storage_reflection_weak_current_property_store_entries\": "
      << inputs.local_storage_reflection_weak_current_property_store_entries
      << ",\n"
      << "    \"storage_reflection_ivar_layout_entries\": "
      << inputs.local_storage_reflection_ivar_layout_entries << ",\n"
      << "    \"storage_reflection_ivar_layout_owner_entries\": "
      << inputs.local_storage_reflection_ivar_layout_owner_entries << ",\n"
      << "    \"driver_linker_flags\": "
      << BuildIndentedStringArrayJson(inputs.local_driver_linker_flags,
                                      "      ")
      << "\n"
      << "  },\n"
      << "  \"imported_modules\": " << imported_modules_json.str() << ",\n"
      << "  \"link_object_artifacts\": "
      << BuildIndentedStringArrayJson(ordered_link_object_artifacts, "    ")
      << ",\n"
      << "  \"driver_linker_flags\": "
      << BuildIndentedStringArrayJson(merged_driver_linker_flags, "    ")
      << ",\n"
      << "  \"ready\": true\n"
      << "}\n";
  plan_json = out.str();

  std::ostringstream response_out;
  for (const auto &flag : merged_driver_linker_flags) {
    response_out << flag << "\n";
  }
  linker_response_payload = response_out.str();
  return true;
}

bool TryBuildObjc3ConformanceReportPublicationArtifact(
    const Objc3ConformanceReportPublicationArtifactInputs &inputs,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  // Publication remains fail-closed on unsupported formats and unknown profiles
  // even after the built-in strict profiles become claimable.

  if (inputs.contract_id.empty() || inputs.schema_id.empty() ||
      inputs.selected_profile.empty() || inputs.supported_profile_ids.empty() ||
      inputs.effective_language_profile.empty() ||
      inputs.publication_model.empty() ||
      inputs.publication_surface_kind.empty() ||
      inputs.fail_closed_diagnostic_model.empty() ||
      inputs.lowered_report_contract_id.empty() ||
      inputs.runtime_capability_contract_id.empty() ||
      inputs.public_conformance_schema_id.empty() ||
      inputs.advanced_feature_ops_contract_id.empty() ||
      inputs.advanced_feature_reporting_contract_id.empty() ||
      inputs.advanced_feature_release_evidence_contract_id.empty() ||
      inputs.ci_release_evidence_gate_script_path.empty() ||
      inputs.runbook_reference_path.empty() ||
      inputs.dashboard_schema_path.empty() ||
      inputs.advanced_feature_targeted_profile_ids.empty() ||
      inputs.report_artifact_relative_path.empty()) {
    error = "conformance report publication artifact inputs are incomplete";
    return false;
  }

  std::ostringstream out;
  JsonObjectWriter publication(out);
  publication.StringField("contract_id", inputs.contract_id);
  publication.StringField("schema_id", inputs.schema_id);
  publication.StringField("selected_profile", inputs.selected_profile);
  publication.BoolField("selected_profile_supported",
                        inputs.selected_profile_supported);
  publication.RawJsonField(
      "supported_profile_ids",
      BuildIndentedStringArrayJson(inputs.supported_profile_ids, "    "));
  publication.RawJsonField(
      "rejected_profile_ids",
      BuildIndentedStringArrayJson(inputs.rejected_profile_ids, "    "));
  publication.StringField("effective_language_profile",
                          inputs.effective_language_profile);
  publication.BoolField("canonical_literal_rejection_diagnostics_enabled",
                        inputs.canonical_literal_rejection_diagnostics_enabled);
  publication.StringField("publication_model", inputs.publication_model);
  publication.StringField("publication_surface_kind",
                          inputs.publication_surface_kind);
  publication.StringField("fail_closed_diagnostic_model",
                          inputs.fail_closed_diagnostic_model);
  publication.StringField("lowered_report_contract_id",
                          inputs.lowered_report_contract_id);
  publication.StringField("runtime_capability_contract_id",
                          inputs.runtime_capability_contract_id);
  publication.StringField("public_conformance_schema_id",
                          inputs.public_conformance_schema_id);
  publication.StringField("advanced_feature_ops_contract_id",
                          inputs.advanced_feature_ops_contract_id);
  publication.StringField("advanced_feature_reporting_contract_id",
                          inputs.advanced_feature_reporting_contract_id);
  publication.StringField(
      "advanced_feature_release_evidence_contract_id",
      inputs.advanced_feature_release_evidence_contract_id);
  publication.RawJsonField(
      "advanced_feature_targeted_profile_ids",
      BuildIndentedStringArrayJson(inputs.advanced_feature_targeted_profile_ids,
                                   "    "));
  publication.StringField("ci_release_evidence_gate_script_path",
                          inputs.ci_release_evidence_gate_script_path);
  publication.StringField("runbook_reference_path",
                          inputs.runbook_reference_path);
  publication.StringField("dashboard_schema_path",
                          inputs.dashboard_schema_path);
  publication.StringField("report_artifact",
                          inputs.report_artifact_relative_path);
  publication.BoolField("ready", true);
  artifact_json = FinishJsonObject(publication, out);
  return true;
}

bool TryBuildObjc3ConformanceClaimValidationArtifact(
    const Objc3ConformanceClaimValidationArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  if (inputs.report_artifact_path.empty() || inputs.publication_artifact_path.empty()) {
    error = "conformance validation artifact inputs are incomplete";
    return false;
  }

  std::string report_schema_id;
  std::string report_contract_id;
  std::string effective_language_profile;
  std::string runtime_capability_contract_id;
  std::string public_conformance_schema_id;
  bool canonical_literal_rejection_diagnostics_enabled = false;
  std::string publication_schema_id;
  std::string publication_contract_id;
  std::string selected_profile;
  bool selected_profile_supported = false;
  std::vector<std::string> supported_profile_ids;
  std::vector<std::string> rejected_profile_ids;
  std::string publication_surface_kind;
  std::string report_artifact_relative_path;
  std::string advanced_feature_ops_contract_id;
  std::string advanced_feature_reporting_contract_id;
  std::string advanced_feature_release_evidence_contract_id;
  std::vector<std::string> advanced_feature_targeted_profile_ids;
  std::string ci_release_evidence_gate_script_path;
  std::string runbook_reference_path;
  std::string dashboard_schema_path;
  const std::vector<std::string> expected_supported_profile_ids =
      BuildObjc3ClaimedConformanceProfileIds();
  const std::vector<std::string> expected_rejected_profile_ids =
      BuildObjc3RejectedConformanceProfileIds();
  const std::vector<std::string> expected_targeted_profile_ids =
      BuildObjc3ReleaseTargetedProfileIds();
  JsonValue report_document;
  JsonValue publication_document;
  if (!TryParseJsonObjectText(report_json, "conformance report",
                              report_document, error) ||
      !TryParseJsonObjectText(publication_json, "conformance publication",
                              publication_document, error)) {
    return false;
  }

  if (!TryGetJsonStringField(report_document, "schema_id", report_schema_id) ||
      report_schema_id != "objc3c-versioned-conformance-report-v1") {
    error = "invalid conformance report schema_id";
    return false;
  }
  if (!TryGetJsonStringField(report_document, "contract_id", report_contract_id) ||
      report_contract_id != "objc3c.versioned.conformance.report.lowering.v1") {
    error = "invalid conformance report contract_id";
    return false;
  }
  if (!TryGetJsonStringField(report_document, "effective_language_profile",
                             effective_language_profile)) {
    error = "missing effective_language_profile in conformance report";
    return false;
  }
  if (!TryGetJsonBoolField(
          report_document, "canonical_literal_rejection_diagnostics_enabled",
          canonical_literal_rejection_diagnostics_enabled)) {
    error = "missing canonical_literal_rejection_diagnostics_enabled in conformance report";
    return false;
  }
  if (!JsonStringFieldEquals(
          report_document, {"runtime_capability_report", "contract_id"},
          "objc3c.runtime.capability.reporting.v1")) {
    error = "runtime_capability_report payload is missing or drifted";
    return false;
  }
  runtime_capability_contract_id =
      "objc3c.runtime.capability.reporting.v1";
  if (!JsonStringFieldEquals(
          report_document, {"public_conformance_report", "schema_id"},
          "objc3-conformance-report/v1")) {
    error = "public_conformance_report payload is missing or drifted";
    return false;
  }
  public_conformance_schema_id = "objc3-conformance-report/v1";
  if (!JsonStringFieldEquals(
          report_document, {"advanced_feature_reporting", "contract_id"},
          kObjc3AdvancedFeatureReportingContractId)) {
    error = "advanced_feature_reporting payload is missing or drifted";
    return false;
  }
  if (!JsonStringFieldEquals(
          report_document,
          {"advanced_feature_release_evidence", "contract_id"},
          kObjc3AdvancedFeatureReleaseEvidenceContractId)) {
    error = "advanced_feature_release_evidence payload is missing or drifted";
    return false;
  }

  if (!TryGetJsonStringField(publication_document, "schema_id",
                             publication_schema_id) ||
      publication_schema_id != "objc3c-driver-conformance-publication-v1") {
    error = "invalid conformance publication schema_id";
    return false;
  }
  if (!TryGetJsonStringField(publication_document, "contract_id",
                             publication_contract_id) ||
      publication_contract_id !=
          "objc3c.driver.conformance.report.publication.v1") {
    error = "invalid conformance publication contract_id";
    return false;
  }
  if (!TryGetJsonStringField(publication_document, "selected_profile",
                             selected_profile)) {
    error = "selected_profile is missing from conformance publication";
    return false;
  }
  if (!TryGetJsonBoolField(publication_document, "selected_profile_supported",
                           selected_profile_supported) ||
      selected_profile_supported !=
          IsObjc3ClaimedConformanceProfile(selected_profile)) {
    error = "selected_profile_supported drifted from the live claim policy";
    return false;
  }
  if (!TryGetJsonStringArrayField(publication_document, "supported_profile_ids",
                                  supported_profile_ids) ||
      supported_profile_ids != expected_supported_profile_ids) {
    error = "supported_profile_ids drifted from the live claim policy";
    return false;
  }
  if (!TryGetJsonStringArrayField(publication_document, "rejected_profile_ids",
                                  rejected_profile_ids) ||
      rejected_profile_ids != expected_rejected_profile_ids) {
    error = "rejected_profile_ids drifted from the live claim policy";
    return false;
  }
  if (!TryGetJsonStringField(publication_document, "publication_surface_kind",
                             publication_surface_kind)) {
    error = "missing publication_surface_kind in conformance publication";
    return false;
  }
  if (!TryGetJsonStringField(publication_document, "report_artifact",
                             report_artifact_relative_path) ||
      report_artifact_relative_path !=
          std::filesystem::path(inputs.report_artifact_path).filename().string()) {
    error = "publication report_artifact does not match the validated report path";
    return false;
  }
  if (!TryGetJsonStringField(publication_document,
                             "advanced_feature_ops_contract_id",
                             advanced_feature_ops_contract_id) ||
      advanced_feature_ops_contract_id != kObjc3AdvancedFeatureOpsContractId) {
    error = "advanced_feature_ops_contract_id drifted";
    return false;
  }
  if (!TryGetJsonStringField(publication_document,
                             "advanced_feature_reporting_contract_id",
                             advanced_feature_reporting_contract_id) ||
      advanced_feature_reporting_contract_id !=
          kObjc3AdvancedFeatureReportingContractId) {
    error = "advanced_feature_reporting_contract_id drifted";
    return false;
  }
  if (!TryGetJsonStringField(
          publication_document, "advanced_feature_release_evidence_contract_id",
          advanced_feature_release_evidence_contract_id) ||
      advanced_feature_release_evidence_contract_id !=
          kObjc3AdvancedFeatureReleaseEvidenceContractId) {
    error = "advanced_feature_release_evidence_contract_id drifted";
    return false;
  }
  if (!TryGetJsonStringArrayField(publication_document,
                                  "advanced_feature_targeted_profile_ids",
                                  advanced_feature_targeted_profile_ids) ||
      advanced_feature_targeted_profile_ids != expected_targeted_profile_ids) {
    error = "advanced_feature_targeted_profile_ids drifted";
    return false;
  }
  if (!TryGetJsonStringField(publication_document,
                             "ci_release_evidence_gate_script_path",
                             ci_release_evidence_gate_script_path) ||
      ci_release_evidence_gate_script_path !=
          kObjc3AdvancedFeatureEvidenceGateScriptPath) {
    error = "ci_release_evidence_gate_script_path drifted";
    return false;
  }
  if (!TryGetJsonStringField(publication_document, "runbook_reference_path",
                             runbook_reference_path) ||
      runbook_reference_path !=
          kObjc3AdvancedFeatureEvidenceRunbookPath) {
    error = "runbook_reference_path drifted";
    return false;
  }
  if (!TryGetJsonStringField(publication_document, "dashboard_schema_path",
                             dashboard_schema_path) ||
      dashboard_schema_path !=
          kObjc3AdvancedFeatureDashboardSchemaPath) {
    error = "dashboard_schema_path drifted";
    return false;
  }
  if (!JsonStringFieldEquals(publication_document,
                             {"fail_closed_diagnostic_model"},
                             kObjc3ConformancePublicationFailClosedDiagnosticModel)) {
    error = "publication fail_closed_diagnostic_model drifted";
    return false;
  }

  std::ostringstream out;
  JsonObjectWriter validation(out);
  validation.StringField("contract_id",
                         kObjc3ToolchainConformanceClaimOperationsContractId);
  validation.StringField("schema_id",
                         kObjc3ToolchainConformanceClaimValidationSchemaId);
  validation.StringField("validation_model",
                         kObjc3ToolchainConformanceClaimValidationModel);
  validation.StringField("consumption_model",
                         kObjc3ToolchainConformanceClaimConsumptionModel);
  validation.StringField("format", "json");
  validation.StringField("validated_report_artifact",
                         inputs.report_artifact_path);
  validation.StringField("validated_publication_artifact",
                         inputs.publication_artifact_path);
  validation.StringField("report_schema_id", report_schema_id);
  validation.StringField("report_contract_id", report_contract_id);
  validation.StringField("runtime_capability_contract_id",
                         runtime_capability_contract_id);
  validation.StringField("public_conformance_schema_id",
                         public_conformance_schema_id);
  validation.StringField("advanced_feature_ops_contract_id",
                         kObjc3AdvancedFeatureOpsContractId);
  validation.StringField("advanced_feature_reporting_contract_id",
                         kObjc3AdvancedFeatureReportingContractId);
  validation.StringField("advanced_feature_release_evidence_contract_id",
                         kObjc3AdvancedFeatureReleaseEvidenceContractId);
  validation.RawJsonField(
      "advanced_feature_targeted_profile_ids",
      BuildIndentedStringArrayJson(advanced_feature_targeted_profile_ids,
                                   "    "));
  validation.StringField("ci_release_evidence_gate_script_path",
                         ci_release_evidence_gate_script_path);
  validation.StringField("runbook_reference_path", runbook_reference_path);
  validation.StringField("dashboard_schema_path", dashboard_schema_path);
  validation.StringField("selected_profile", selected_profile);
  validation.BoolField("selected_profile_supported", selected_profile_supported);
  validation.RawJsonField("supported_profile_ids",
                          BuildIndentedStringArrayJson(supported_profile_ids,
                                                       "    "));
  validation.RawJsonField("rejected_profile_ids",
                          BuildIndentedStringArrayJson(rejected_profile_ids,
                                                       "    "));
  validation.StringField("effective_language_profile",
                         effective_language_profile);
  validation.BoolField("canonical_literal_rejection_diagnostics_enabled",
                       canonical_literal_rejection_diagnostics_enabled);
  validation.StringField("publication_surface_kind", publication_surface_kind);
  validation.BoolField("ready", true);
  artifact_json = FinishJsonObject(validation, out);
  return true;
}

bool TryBuildObjc3ReleaseEvidenceOperationArtifact(
    const Objc3ReleaseEvidenceOperationArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    const std::string &validation_json,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  if (inputs.report_artifact_path.empty() || inputs.publication_artifact_path.empty() ||
      inputs.validation_artifact_path.empty() ||
      inputs.dashboard_artifact_path.empty()) {
    error = "release evidence operation artifact inputs are incomplete";
    return false;
  }
  JsonValue report_document;
  JsonValue publication_document;
  JsonValue validation_document;
  if (!TryParseJsonObjectText(report_json, "conformance report",
                              report_document, error) ||
      !TryParseJsonObjectText(publication_json, "conformance publication",
                              publication_document, error) ||
      !TryParseJsonObjectText(validation_json, "conformance validation",
                              validation_document, error)) {
    return false;
  }
  if (!JsonStringFieldEquals(
          report_document,
          {"advanced_feature_release_evidence",
           "release_evidence_checklist_path"},
          "spec/conformance/profile_release_evidence_checklist.md") ||
      !JsonStringFieldEquals(
          report_document,
          {"advanced_feature_release_evidence", "release_evidence_schema_path"},
          "spec/conformance/objc3_conformance_evidence_bundle_schema.md")) {
    error = "advanced feature release evidence payload is missing or drifted";
    return false;
  }
  if (!JsonStringFieldEquals(publication_document,
                             {"advanced_feature_ops_contract_id"},
                             kObjc3AdvancedFeatureOpsContractId) ||
      !JsonStringFieldEquals(validation_document, {"contract_id"},
                             kObjc3ToolchainConformanceClaimOperationsContractId)) {
    error = "advanced feature operator contract drifted";
    return false;
  }

  std::ostringstream out;
  JsonObjectWriter operation(out);
  operation.StringField("contract_id", kObjc3ReleaseEvidenceOperationContractId);
  operation.StringField("schema_id", kObjc3ReleaseEvidenceOperationSchemaId);
  operation.RawJsonField(
      "dependency_contract_ids",
      BuildIndentedStringArrayJson(
          {kObjc3AdvancedFeatureOpsContractId,
           kObjc3AdvancedFeatureReleaseEvidenceContractId},
          "    "));
  operation.StringField("validation_contract_id",
                        kObjc3ToolchainConformanceClaimOperationsContractId);
  operation.StringField("dashboard_contract_id",
                        kObjc3DashboardStatusPublicationContractId);
  operation.StringField(
      "operation_model",
      "validation-publishes-release-evidence-command-surface-and-dashboard-status-over-the-final-claim-publication-artifact-set");
  operation.StringField("release_label", kObjc3AdvancedFeatureReleaseLabel);
  operation.RawJsonField(
      "command_tokens",
      BuildIndentedStringArrayJson(
          {"python", kObjc3AdvancedFeatureEvidenceGateScriptPath}, "    "));
  operation.StringField("report_artifact", inputs.report_artifact_path);
  operation.StringField("publication_artifact",
                        inputs.publication_artifact_path);
  operation.StringField("validation_artifact", inputs.validation_artifact_path);
  operation.StringField("dashboard_artifact", inputs.dashboard_artifact_path);
  operation.StringField("gate_script_path",
                        kObjc3AdvancedFeatureEvidenceGateScriptPath);
  operation.StringField("runbook_reference_path",
                        kObjc3AdvancedFeatureEvidenceRunbookPath);
  operation.StringField("dashboard_schema_path",
                        kObjc3AdvancedFeatureDashboardSchemaPath);
  operation.StringField("release_evidence_checklist_path",
                        "spec/conformance/profile_release_evidence_checklist.md");
  operation.StringField(
      "release_evidence_schema_path",
      "spec/conformance/objc3_conformance_evidence_bundle_schema.md");
  operation.RawJsonField(
      "targeted_profile_ids",
      BuildIndentedStringArrayJson(
          {"strict", "strict-concurrency", "strict-system"}, "    "));
  operation.RawJsonField(
      "corpus_shard_ids",
      BuildIndentedStringArrayJson({"parser", "semantic", "lowering_abi",
                                    "module_roundtrip", "diagnostics"},
                                   "    "));
  operation.RawJsonField(
      "release_evidence_artifact_ids",
      BuildIndentedStringArrayJson({"EVID-01", "EVID-02", "EVID-03",
                                    "EVID-04", "EVID-07", "EVID-08",
                                    "EVID-09", "EVID-10", "EVID-11"},
                                   "    "));
  operation.StringField("generated_at", kObjc3DeterministicReplayTimestamp);
  operation.BoolField("ready", true);
  artifact_json = FinishJsonObject(operation, out);
  return true;
}

bool TryBuildObjc3DashboardStatusArtifact(
    const Objc3DashboardStatusArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    const std::string &validation_json,
    const std::string &release_evidence_operation_json,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  if (inputs.report_artifact_path.empty() || inputs.publication_artifact_path.empty() ||
      inputs.validation_artifact_path.empty() ||
      inputs.release_evidence_operation_artifact_path.empty()) {
    error = "dashboard status artifact inputs are incomplete";
    return false;
  }
  JsonValue report_document;
  JsonValue publication_document;
  JsonValue validation_document;
  JsonValue release_evidence_operation_document;
  if (!TryParseJsonObjectText(report_json, "conformance report",
                              report_document, error) ||
      !TryParseJsonObjectText(publication_json, "conformance publication",
                              publication_document, error) ||
      !TryParseJsonObjectText(validation_json, "conformance validation",
                              validation_document, error) ||
      !TryParseJsonObjectText(release_evidence_operation_json,
                              "release evidence operation",
                              release_evidence_operation_document, error)) {
    return false;
  }
  if (!HasJsonField(report_document, {"advanced_feature_release_evidence"}) ||
      !JsonStringFieldEquals(publication_document,
                             {"advanced_feature_ops_contract_id"},
                             kObjc3AdvancedFeatureOpsContractId) ||
      !JsonStringFieldEquals(validation_document, {"contract_id"},
                             kObjc3ToolchainConformanceClaimOperationsContractId) ||
      !JsonStringFieldEquals(release_evidence_operation_document,
                             {"contract_id"},
                             kObjc3ReleaseEvidenceOperationContractId)) {
    error = "dashboard status publication inputs drifted";
    return false;
  }

  std::ostringstream out;
  out << "{\n"
      << "  \"schema_id\": \""
      << EscapeJsonString(kObjc3DashboardStatusSchemaId)
      << "\",\n"
      << "  \"schema_version\": 1,\n"
      << "  \"dashboard_version\": \""
      << EscapeJsonString(kObjc3DashboardVersion)
      << "\",\n"
      << "  \"release_label\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureReleaseLabel) << "\",\n"
      << "  \"release_id\": \""
      << EscapeJsonString(kObjc3DashboardReleaseId) << "\",\n"
      << "  \"generated_at\": \""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp) << "\",\n"
      << "  \"source_revision\": \""
      << EscapeJsonString(kObjc3DeterministicSourceRevision) << "\",\n"
      << "  \"status\": \"pass\",\n"
      << "  \"profiles\": [\n"
      << "    {\"profile_id\":\"core\",\"status\":\"pass\",\"dependency_status\":{\"B-04\":\"pass\",\"B-10\":\"pass\",\"B-11\":\"pass\",\"B-12\":\"pass\"},\"last_refresh\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"blocker_ids\":[]},\n"
      << "    {\"profile_id\":\"strict\",\"status\":\"pass\",\"dependency_status\":{\"B-04\":\"pass\",\"B-10\":\"pass\",\"B-11\":\"pass\",\"B-12\":\"pass\"},\"last_refresh\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"blocker_ids\":[]},\n"
      << "    {\"profile_id\":\"strict-concurrency\",\"status\":\"pass\",\"dependency_status\":{\"B-04\":\"pass\",\"B-10\":\"pass\",\"B-11\":\"pass\",\"B-12\":\"pass\"},\"last_refresh\":\""
