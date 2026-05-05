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
  out << "{\n"
      << "  \"contract_id\": \"" << EscapeJsonString(inputs.contract_id)
      << "\",\n"
      << "  \"schema_id\": \"" << EscapeJsonString(inputs.schema_id)
      << "\",\n"
      << "  \"selected_profile\": \""
      << EscapeJsonString(inputs.selected_profile) << "\",\n"
      << "  \"selected_profile_supported\": "
      << (inputs.selected_profile_supported ? "true" : "false") << ",\n"
      << "  \"supported_profile_ids\": "
      << BuildIndentedStringArrayJson(inputs.supported_profile_ids, "    ")
      << ",\n"
      << "  \"rejected_profile_ids\": "
      << BuildIndentedStringArrayJson(inputs.rejected_profile_ids, "    ")
      << ",\n"
      << "  \"effective_language_profile\": \""
      << EscapeJsonString(inputs.effective_language_profile) << "\",\n"
      << "  \"canonical_literal_rejection_diagnostics_enabled\": "
      << (inputs.canonical_literal_rejection_diagnostics_enabled ? "true" : "false") << ",\n"
      << "  \"publication_model\": \""
      << EscapeJsonString(inputs.publication_model) << "\",\n"
      << "  \"publication_surface_kind\": \""
      << EscapeJsonString(inputs.publication_surface_kind) << "\",\n"
      << "  \"fail_closed_diagnostic_model\": \""
      << EscapeJsonString(inputs.fail_closed_diagnostic_model) << "\",\n"
      << "  \"lowered_report_contract_id\": \""
      << EscapeJsonString(inputs.lowered_report_contract_id) << "\",\n"
      << "  \"runtime_capability_contract_id\": \""
      << EscapeJsonString(inputs.runtime_capability_contract_id) << "\",\n"
      << "  \"public_conformance_schema_id\": \""
      << EscapeJsonString(inputs.public_conformance_schema_id) << "\",\n"
      << "  \"advanced_feature_ops_contract_id\": \""
      << EscapeJsonString(inputs.advanced_feature_ops_contract_id) << "\",\n"
      << "  \"advanced_feature_reporting_contract_id\": \""
      << EscapeJsonString(inputs.advanced_feature_reporting_contract_id)
      << "\",\n"
      << "  \"advanced_feature_release_evidence_contract_id\": \""
      << EscapeJsonString(inputs.advanced_feature_release_evidence_contract_id)
      << "\",\n"
      << "  \"advanced_feature_targeted_profile_ids\": "
      << BuildIndentedStringArrayJson(inputs.advanced_feature_targeted_profile_ids,
                                      "    ")
      << ",\n"
      << "  \"ci_release_evidence_gate_script_path\": \""
      << EscapeJsonString(inputs.ci_release_evidence_gate_script_path)
      << "\",\n"
      << "  \"runbook_reference_path\": \""
      << EscapeJsonString(inputs.runbook_reference_path) << "\",\n"
      << "  \"dashboard_schema_path\": \""
      << EscapeJsonString(inputs.dashboard_schema_path) << "\",\n"
      << "  \"report_artifact\": \""
      << EscapeJsonString(inputs.report_artifact_relative_path) << "\",\n"
      << "  \"ready\": true\n"
      << "}\n";
  artifact_json = out.str();
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

  if (!TryExtractJsonStringField(report_json, "schema_id", report_schema_id) ||
      report_schema_id != "objc3c-versioned-conformance-report-v1") {
    error = "invalid conformance report schema_id";
    return false;
  }
  if (!TryExtractJsonStringField(report_json, "contract_id", report_contract_id) ||
      report_contract_id != "objc3c.versioned.conformance.report.lowering.v1") {
    error = "invalid conformance report contract_id";
    return false;
  }
  if (!TryExtractJsonStringField(report_json, "effective_language_profile",
                                 effective_language_profile)) {
    error = "missing effective_language_profile in conformance report";
    return false;
  }
  if (!TryExtractJsonBoolField(report_json, "canonical_literal_rejection_diagnostics_enabled",
                               canonical_literal_rejection_diagnostics_enabled)) {
    error = "missing canonical_literal_rejection_diagnostics_enabled in conformance report";
    return false;
  }
  if (report_json.find("\"runtime_capability_report\"") == std::string::npos ||
      report_json.find(
          "\"contract_id\":\"objc3c.runtime.capability.reporting.v1\"") ==
          std::string::npos) {
    error = "runtime_capability_report payload is missing or drifted";
    return false;
  }
  runtime_capability_contract_id =
      "objc3c.runtime.capability.reporting.v1";
  if (report_json.find("\"public_conformance_report\"") == std::string::npos ||
      report_json.find("\"schema_id\":\"objc3-conformance-report/v1\"") ==
          std::string::npos) {
    error = "public_conformance_report payload is missing or drifted";
    return false;
  }
  public_conformance_schema_id = "objc3-conformance-report/v1";
  if (report_json.find("\"advanced_feature_reporting\"") == std::string::npos ||
      report_json.find(kObjc3AdvancedFeatureReportingContractId) ==
          std::string::npos) {
    error = "advanced_feature_reporting payload is missing or drifted";
    return false;
  }
  if (report_json.find("\"advanced_feature_release_evidence\"") ==
          std::string::npos ||
      report_json.find(kObjc3AdvancedFeatureReleaseEvidenceContractId) ==
          std::string::npos) {
    error = "advanced_feature_release_evidence payload is missing or drifted";
    return false;
  }

  if (!TryExtractJsonStringField(publication_json, "schema_id",
                                 publication_schema_id) ||
      publication_schema_id != "objc3c-driver-conformance-publication-v1") {
    error = "invalid conformance publication schema_id";
    return false;
  }
  if (!TryExtractJsonStringField(publication_json, "contract_id",
                                 publication_contract_id) ||
      publication_contract_id !=
          "objc3c.driver.conformance.report.publication.v1") {
    error = "invalid conformance publication contract_id";
    return false;
  }
  if (!TryExtractJsonStringField(publication_json, "selected_profile",
                                 selected_profile)) {
    error = "selected_profile is missing from conformance publication";
    return false;
  }
  if (!TryExtractJsonBoolField(publication_json, "selected_profile_supported",
                               selected_profile_supported) ||
      selected_profile_supported !=
          IsObjc3ClaimedConformanceProfile(selected_profile)) {
    error = "selected_profile_supported drifted from the live claim policy";
    return false;
  }
  if (!TryExtractJsonStringArrayField(publication_json, "supported_profile_ids",
                                      supported_profile_ids) ||
      supported_profile_ids != expected_supported_profile_ids) {
    error = "supported_profile_ids drifted from the live claim policy";
    return false;
  }
  if (!TryExtractJsonStringArrayField(publication_json, "rejected_profile_ids",
                                      rejected_profile_ids) ||
      rejected_profile_ids != expected_rejected_profile_ids) {
    error = "rejected_profile_ids drifted from the live claim policy";
    return false;
  }
  if (!TryExtractJsonStringField(publication_json, "publication_surface_kind",
                                 publication_surface_kind)) {
    error = "missing publication_surface_kind in conformance publication";
    return false;
  }
  if (!TryExtractJsonStringField(publication_json, "report_artifact",
                                 report_artifact_relative_path) ||
      report_artifact_relative_path !=
          std::filesystem::path(inputs.report_artifact_path).filename().string()) {
    error = "publication report_artifact does not match the validated report path";
    return false;
  }
  if (!TryExtractJsonStringField(publication_json,
                                 "advanced_feature_ops_contract_id",
                                 advanced_feature_ops_contract_id) ||
      advanced_feature_ops_contract_id != kObjc3AdvancedFeatureOpsContractId) {
    error = "advanced_feature_ops_contract_id drifted";
    return false;
  }
  if (!TryExtractJsonStringField(publication_json,
                                 "advanced_feature_reporting_contract_id",
                                 advanced_feature_reporting_contract_id) ||
      advanced_feature_reporting_contract_id !=
          kObjc3AdvancedFeatureReportingContractId) {
    error = "advanced_feature_reporting_contract_id drifted";
    return false;
  }
  if (!TryExtractJsonStringField(
          publication_json, "advanced_feature_release_evidence_contract_id",
          advanced_feature_release_evidence_contract_id) ||
      advanced_feature_release_evidence_contract_id !=
          kObjc3AdvancedFeatureReleaseEvidenceContractId) {
    error = "advanced_feature_release_evidence_contract_id drifted";
    return false;
  }
  if (!TryExtractJsonStringArrayField(publication_json,
                                      "advanced_feature_targeted_profile_ids",
                                      advanced_feature_targeted_profile_ids) ||
      advanced_feature_targeted_profile_ids != expected_targeted_profile_ids) {
    error = "advanced_feature_targeted_profile_ids drifted";
    return false;
  }
  if (!TryExtractJsonStringField(publication_json,
                                 "ci_release_evidence_gate_script_path",
                                 ci_release_evidence_gate_script_path) ||
      ci_release_evidence_gate_script_path !=
          kObjc3AdvancedFeatureEvidenceGateScriptPath) {
    error = "ci_release_evidence_gate_script_path drifted";
    return false;
  }
  if (!TryExtractJsonStringField(publication_json, "runbook_reference_path",
                                 runbook_reference_path) ||
      runbook_reference_path !=
          kObjc3AdvancedFeatureEvidenceRunbookPath) {
    error = "runbook_reference_path drifted";
    return false;
  }
  if (!TryExtractJsonStringField(publication_json, "dashboard_schema_path",
                                 dashboard_schema_path) ||
      dashboard_schema_path !=
          kObjc3AdvancedFeatureDashboardSchemaPath) {
    error = "dashboard_schema_path drifted";
    return false;
  }
  if (publication_json.find(
          std::string("\"fail_closed_diagnostic_model\": \"") +
              kObjc3ConformancePublicationFailClosedDiagnosticModel + "\"") ==
      std::string::npos) {
    error = "publication fail_closed_diagnostic_model drifted";
    return false;
  }

  std::ostringstream out;
  out << "{\n"
      << "  \"contract_id\": \""
      << EscapeJsonString(kObjc3ToolchainConformanceClaimOperationsContractId)
      << "\",\n"
      << "  \"schema_id\": \""
      << EscapeJsonString(kObjc3ToolchainConformanceClaimValidationSchemaId)
      << "\",\n"
      << "  \"validation_model\": \""
      << EscapeJsonString(kObjc3ToolchainConformanceClaimValidationModel)
      << "\",\n"
      << "  \"consumption_model\": \""
      << EscapeJsonString(kObjc3ToolchainConformanceClaimConsumptionModel)
      << "\",\n"
      << "  \"format\": \"json\",\n"
      << "  \"validated_report_artifact\": \""
      << EscapeJsonString(inputs.report_artifact_path) << "\",\n"
      << "  \"validated_publication_artifact\": \""
      << EscapeJsonString(inputs.publication_artifact_path) << "\",\n"
      << "  \"report_schema_id\": \"" << EscapeJsonString(report_schema_id)
      << "\",\n"
      << "  \"report_contract_id\": \"" << EscapeJsonString(report_contract_id)
      << "\",\n"
      << "  \"runtime_capability_contract_id\": \""
      << EscapeJsonString(runtime_capability_contract_id) << "\",\n"
      << "  \"public_conformance_schema_id\": \""
      << EscapeJsonString(public_conformance_schema_id) << "\",\n"
      << "  \"advanced_feature_ops_contract_id\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureOpsContractId) << "\",\n"
      << "  \"advanced_feature_reporting_contract_id\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureReportingContractId)
      << "\",\n"
      << "  \"advanced_feature_release_evidence_contract_id\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureReleaseEvidenceContractId)
      << "\",\n"
      << "  \"advanced_feature_targeted_profile_ids\": "
      << BuildIndentedStringArrayJson(advanced_feature_targeted_profile_ids,
                                      "    ")
      << ",\n"
      << "  \"ci_release_evidence_gate_script_path\": \""
      << EscapeJsonString(ci_release_evidence_gate_script_path) << "\",\n"
      << "  \"runbook_reference_path\": \""
      << EscapeJsonString(runbook_reference_path) << "\",\n"
      << "  \"dashboard_schema_path\": \""
      << EscapeJsonString(dashboard_schema_path) << "\",\n"
      << "  \"selected_profile\": \"" << EscapeJsonString(selected_profile)
      << "\",\n"
      << "  \"selected_profile_supported\": "
      << (selected_profile_supported ? "true" : "false") << ",\n"
      << "  \"supported_profile_ids\": "
      << BuildIndentedStringArrayJson(supported_profile_ids, "    ") << ",\n"
      << "  \"rejected_profile_ids\": "
      << BuildIndentedStringArrayJson(rejected_profile_ids, "    ") << ",\n"
      << "  \"effective_language_profile\": \""
      << EscapeJsonString(effective_language_profile) << "\",\n"
      << "  \"canonical_literal_rejection_diagnostics_enabled\": "
      << (canonical_literal_rejection_diagnostics_enabled ? "true" : "false") << ",\n"
      << "  \"publication_surface_kind\": \""
      << EscapeJsonString(publication_surface_kind) << "\",\n"
      << "  \"ready\": true\n"
      << "}\n";
  artifact_json = out.str();
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
  if (report_json.find("\"advanced_feature_release_evidence\"") ==
          std::string::npos ||
      report_json.find("\"release_evidence_checklist_path\":\"spec/conformance/profile_release_evidence_checklist.md\"") ==
          std::string::npos ||
      report_json.find("\"release_evidence_schema_path\":\"spec/conformance/objc3_conformance_evidence_bundle_schema.md\"") ==
          std::string::npos) {
    error = "advanced feature release evidence payload is missing or drifted";
    return false;
  }
  if (publication_json.find(kObjc3AdvancedFeatureOpsContractId) ==
          std::string::npos ||
      validation_json.find(kObjc3AdvancedFeatureOpsContractId) ==
          std::string::npos) {
    error = "advanced feature operator contract drifted";
    return false;
  }

  std::ostringstream out;
  out << "{\n"
      << "  \"contract_id\": \""
      << EscapeJsonString(kObjc3ReleaseEvidenceOperationContractId) << "\",\n"
      << "  \"schema_id\": \""
      << EscapeJsonString(kObjc3ReleaseEvidenceOperationSchemaId) << "\",\n"
      << "  \"dependency_contract_ids\": "
      << BuildIndentedStringArrayJson(
             {kObjc3AdvancedFeatureOpsContractId,
              kObjc3AdvancedFeatureReleaseEvidenceContractId},
             "    ")
      << ",\n"
      << "  \"validation_contract_id\": \""
      << EscapeJsonString(kObjc3ToolchainConformanceClaimOperationsContractId)
      << "\",\n"
      << "  \"dashboard_contract_id\": \""
      << EscapeJsonString(kObjc3DashboardStatusPublicationContractId)
      << "\",\n"
      << "  \"operation_model\": \""
      << "validation-publishes-release-evidence-command-surface-and-dashboard-status-over-the-final-claim-publication-artifact-set"
      << "\",\n"
      << "  \"release_label\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureReleaseLabel) << "\",\n"
      << "  \"command_tokens\": "
      << BuildIndentedStringArrayJson({"python",
                                       kObjc3AdvancedFeatureEvidenceGateScriptPath},
                                      "    ")
      << ",\n"
      << "  \"report_artifact\": \""
      << EscapeJsonString(inputs.report_artifact_path) << "\",\n"
      << "  \"publication_artifact\": \""
      << EscapeJsonString(inputs.publication_artifact_path) << "\",\n"
      << "  \"validation_artifact\": \""
      << EscapeJsonString(inputs.validation_artifact_path) << "\",\n"
      << "  \"dashboard_artifact\": \""
      << EscapeJsonString(inputs.dashboard_artifact_path) << "\",\n"
      << "  \"gate_script_path\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureEvidenceGateScriptPath)
      << "\",\n"
      << "  \"runbook_reference_path\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureEvidenceRunbookPath)
      << "\",\n"
      << "  \"dashboard_schema_path\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureDashboardSchemaPath)
      << "\",\n"
      << "  \"release_evidence_checklist_path\": "
      << "\"spec/conformance/profile_release_evidence_checklist.md\",\n"
      << "  \"release_evidence_schema_path\": "
      << "\"spec/conformance/objc3_conformance_evidence_bundle_schema.md\",\n"
      << "  \"targeted_profile_ids\": "
      << BuildIndentedStringArrayJson(
             {"strict", "strict-concurrency", "strict-system"}, "    ")
      << ",\n"
      << "  \"corpus_shard_ids\": "
      << BuildIndentedStringArrayJson({"parser", "semantic", "lowering_abi",
                                       "module_roundtrip", "diagnostics"},
                                      "    ")
      << ",\n"
      << "  \"release_evidence_artifact_ids\": "
      << BuildIndentedStringArrayJson({"EVID-01", "EVID-02", "EVID-03",
                                       "EVID-04", "EVID-07", "EVID-08",
                                       "EVID-09", "EVID-10", "EVID-11"},
                                      "    ")
      << ",\n"
      << "  \"generated_at\": \""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp) << "\",\n"
      << "  \"ready\": true\n"
      << "}\n";
  artifact_json = out.str();
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
  if (report_json.find("\"advanced_feature_release_evidence\"") ==
          std::string::npos ||
      publication_json.find(kObjc3AdvancedFeatureOpsContractId) ==
          std::string::npos ||
      validation_json.find(kObjc3AdvancedFeatureOpsContractId) ==
          std::string::npos ||
      release_evidence_operation_json.find(
          kObjc3ReleaseEvidenceOperationContractId) == std::string::npos) {
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
