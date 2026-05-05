              kObjc3RuntimeSupportLibraryLinkWiringContractId;
          link_plan_inputs.expected_storage_reflection_contract_id =
              kObjc3RuntimeStorageReflectionArtifactPreservationContractId;
          link_plan_inputs.expected_storage_reflection_source_contract_id =
              kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId;
          link_plan_inputs
              .expected_storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id =
              kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId;
          link_plan_inputs
              .expected_storage_reflection_executable_property_accessor_layout_lowering_contract_id =
              kObjc3ExecutablePropertyAccessorLayoutLoweringContractId;
          link_plan_inputs
              .expected_storage_reflection_executable_ivar_layout_emission_contract_id =
              kObjc3ExecutableIvarLayoutEmissionContractId;
          link_plan_inputs
              .expected_storage_reflection_executable_synthesized_accessor_property_lowering_contract_id =
              kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId;
          link_plan_inputs.expected_bootstrap_live_registration_contract_id =
              "objc3c.runtime.live.registration.discovery.replay.v1";
          link_plan_inputs.expected_bootstrap_live_restart_hardening_contract_id =
              "objc3c.runtime.live.restart.hardening.v1";
          link_plan_inputs.expected_bootstrap_replay_registered_images_symbol =
              kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol;
          link_plan_inputs.expected_bootstrap_reset_replay_state_snapshot_symbol =
              kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol;
          link_plan_inputs.expected_bootstrap_reset_for_testing_symbol =
              kObjc3RuntimeSupportLibraryResetForTestingSymbol;
          for (std::size_t index = 0; index < imported_surfaces.size(); ++index) {
            link_plan_inputs.direct_import_surface_artifact_paths.push_back(
                imported_surfaces[index].source_path.generic_string());
            const auto &peer_artifacts = imported_peer_artifacts[index];
            const auto &imported_surface = imported_surfaces[index];
            Objc3CrossModuleRuntimeLinkPlanImportedInput imported_input;
            imported_input.module_name =
                imported_surface.frontend_closure_summary.module_name;
            imported_input.import_surface_artifact_path =
                imported_surface.source_path.generic_string();
            imported_input.registration_manifest_artifact_path =
                peer_artifacts.registration_manifest_path.generic_string();
            imported_input.object_artifact_path =
                peer_artifacts.object_artifact_path.generic_string();
            imported_input.discovery_artifact_path =
                peer_artifacts.discovery_artifact_path.generic_string();
            imported_input.linker_response_artifact_path =
                peer_artifacts.linker_response_artifact_path.generic_string();
            imported_input.translation_unit_identity_model =
                peer_artifacts.translation_unit_identity_model;
            imported_input.translation_unit_identity_key =
                peer_artifacts.translation_unit_identity_key;
            imported_input.object_format = peer_artifacts.object_format;
            imported_input.runtime_support_library_archive_relative_path =
                peer_artifacts.runtime_support_library_archive_relative_path;
            imported_input.translation_unit_registration_order_ordinal =
                peer_artifacts.translation_unit_registration_order_ordinal;
            imported_input.class_descriptor_count =
                peer_artifacts.class_descriptor_count;
            imported_input.protocol_descriptor_count =
                peer_artifacts.protocol_descriptor_count;
            imported_input.category_descriptor_count =
                peer_artifacts.category_descriptor_count;
            imported_input.property_descriptor_count =
                peer_artifacts.property_descriptor_count;
            imported_input.ivar_descriptor_count =
                peer_artifacts.ivar_descriptor_count;
            imported_input.total_descriptor_count =
                peer_artifacts.total_descriptor_count;
            imported_input.driver_linker_flags =
                peer_artifacts.driver_linker_flags;
            imported_input.ready_for_live_registration_discovery_replay =
                peer_artifacts.ready_for_live_registration_discovery_replay;
            imported_input.ready_for_live_restart_hardening =
                peer_artifacts.ready_for_live_restart_hardening;
            imported_input.bootstrap_live_registration_contract_id =
                peer_artifacts.bootstrap_live_registration_contract_id;
            imported_input.bootstrap_live_restart_hardening_contract_id =
                peer_artifacts.bootstrap_live_restart_hardening_contract_id;
            imported_input.bootstrap_live_replay_registered_images_symbol =
                peer_artifacts.bootstrap_live_replay_registered_images_symbol;
            imported_input.bootstrap_live_reset_replay_state_snapshot_symbol =
                peer_artifacts
                    .bootstrap_live_reset_replay_state_snapshot_symbol;
            imported_input.bootstrap_live_restart_reset_for_testing_symbol =
                peer_artifacts
                    .bootstrap_live_restart_reset_for_testing_symbol;
            imported_input.bootstrap_live_restart_replay_registered_images_symbol =
                peer_artifacts
                    .bootstrap_live_restart_replay_registered_images_symbol;
            imported_input
                .bootstrap_live_restart_reset_replay_state_snapshot_symbol =
                peer_artifacts
                    .bootstrap_live_restart_reset_replay_state_snapshot_symbol;
            imported_input.error_handling_result_and_bridging_artifact_replay_present =
                imported_surface.error_handling_result_and_bridging_artifact_replay_present;
            imported_input.error_handling_binary_artifact_replay_ready =
                imported_surface.error_handling_binary_artifact_replay_ready;
            imported_input.error_handling_runtime_import_artifact_ready =
                imported_surface.error_handling_runtime_import_artifact_ready;
            imported_input.error_handling_separate_compilation_replay_ready =
                imported_surface.error_handling_separate_compilation_replay_ready;
            imported_input.error_handling_deterministic =
                imported_surface.error_handling_deterministic;
            imported_input.error_handling_contract_id = imported_surface.error_handling_contract_id;
            imported_input.error_handling_source_contract_id =
                imported_surface.error_handling_source_contract_id;
            imported_input.error_handling_result_and_bridging_artifact_replay_key =
                imported_surface.error_handling_result_and_bridging_artifact_replay_key;
            imported_input.error_handling_error_handling_replay_key =
                imported_surface.error_handling_error_handling_replay_key;
            imported_input.error_handling_throws_replay_key =
                imported_surface.error_handling_throws_replay_key;
            imported_input.error_handling_result_like_replay_key =
                imported_surface.error_handling_result_like_replay_key;
            imported_input.error_handling_ns_error_replay_key =
                imported_surface.error_handling_ns_error_replay_key;
            imported_input.error_handling_unwind_replay_key =
                imported_surface.error_handling_unwind_replay_key;
            imported_input.concurrency_actor_mailbox_runtime_import_present =
                imported_surface.concurrency_actor_mailbox_runtime_import_present;
            imported_input.concurrency_actor_mailbox_runtime_ready =
                imported_surface.concurrency_actor_mailbox_runtime_ready;
            imported_input.concurrency_actor_mailbox_runtime_deterministic =
                imported_surface.concurrency_actor_mailbox_runtime_deterministic;
            imported_input.concurrency_actor_contract_id =
                imported_surface.concurrency_actor_mailbox_runtime_contract_id;
            imported_input.concurrency_actor_source_contract_id =
                imported_surface.concurrency_actor_mailbox_runtime_source_contract_id;
            imported_input.concurrency_actor_mailbox_runtime_replay_key =
                imported_surface.concurrency_actor_mailbox_runtime_replay_key;
            imported_input.concurrency_actor_lowering_replay_key =
                imported_surface.concurrency_actor_lowering_replay_key;
            imported_input.concurrency_actor_isolation_lowering_replay_key =
                imported_surface.concurrency_actor_isolation_lowering_replay_key;
            imported_input.interop_ffi_metadata_interface_preservation_present =
                imported_surface.interop_ffi_metadata_interface_preservation_present;
            imported_input.interop_ffi_runtime_import_artifact_ready =
                imported_surface.interop_ffi_runtime_import_artifact_ready;
            imported_input.interop_ffi_separate_compilation_preservation_ready =
                imported_surface
                    .interop_ffi_separate_compilation_preservation_ready;
            imported_input.interop_ffi_deterministic =
                imported_surface.interop_ffi_deterministic;
            imported_input.interop_ffi_contract_id =
                imported_surface.interop_ffi_contract_id;
            imported_input.interop_ffi_source_contract_id =
                imported_surface.interop_ffi_source_contract_id;
            imported_input.interop_ffi_preservation_contract_id =
                imported_surface.interop_ffi_preservation_contract_id;
            imported_input.interop_ffi_replay_key =
                imported_surface.interop_ffi_replay_key;
            imported_input.interop_ffi_lowering_replay_key =
                imported_surface.interop_ffi_lowering_replay_key;
            imported_input.interop_ffi_preservation_replay_key =
                imported_surface.interop_ffi_preservation_replay_key;
            imported_input.interop_ffi_local_foreign_callable_count =
                imported_surface.interop_ffi_local_foreign_callable_count;
            imported_input.interop_ffi_local_metadata_preservation_sites =
                imported_surface.interop_ffi_local_metadata_preservation_sites;
            imported_input.interop_ffi_local_interface_annotation_sites =
                imported_surface.interop_ffi_local_interface_annotation_sites;
            imported_input.interop_header_module_bridge_generation_present =
                imported_surface.interop_header_module_bridge_generation_present;
            imported_input.interop_header_module_bridge_runtime_generation_ready =
                imported_surface
                    .interop_header_module_bridge_runtime_generation_ready;
            imported_input
                .interop_header_module_bridge_cross_module_packaging_ready =
                imported_surface
                    .interop_header_module_bridge_cross_module_packaging_ready;
            imported_input.interop_header_module_bridge_deterministic =
                imported_surface.interop_header_module_bridge_deterministic;
            imported_input.interop_header_module_bridge_contract_id =
                imported_surface.interop_header_module_bridge_contract_id;
            imported_input.interop_header_module_bridge_source_contract_id =
                imported_surface.interop_header_module_bridge_source_contract_id;
            imported_input.interop_header_module_bridge_preservation_contract_id =
                imported_surface
                    .interop_header_module_bridge_preservation_contract_id;
            imported_input.interop_header_module_bridge_replay_key =
                imported_surface.interop_header_module_bridge_replay_key;
            imported_input.interop_header_module_bridge_preservation_replay_key =
                imported_surface
                    .interop_header_module_bridge_preservation_replay_key;
            imported_input.interop_bridge_header_artifact_relative_path =
                imported_surface.interop_bridge_header_artifact_relative_path;
            imported_input.interop_bridge_module_artifact_relative_path =
                imported_surface.interop_bridge_module_artifact_relative_path;
            imported_input.interop_bridge_artifact_relative_path =
                imported_surface.interop_bridge_artifact_relative_path;
            imported_input.interop_header_module_bridge_local_foreign_callable_count =
                imported_surface
                    .interop_header_module_bridge_local_foreign_callable_count;
            imported_input
                .metaprogramming_macro_host_process_cache_runtime_integration_present =
                imported_surface
                    .metaprogramming_macro_host_process_cache_runtime_integration_present;
            imported_input.metaprogramming_macro_host_process_cache_runtime_ready =
                imported_surface.metaprogramming_macro_host_process_cache_runtime_ready;
            imported_input
                .metaprogramming_macro_host_process_cache_separate_compilation_ready =
                imported_surface
                    .metaprogramming_macro_host_process_cache_separate_compilation_ready;
            imported_input.metaprogramming_macro_host_process_cache_deterministic =
                imported_surface.metaprogramming_macro_host_process_cache_deterministic;
            imported_input.metaprogramming_macro_host_process_cache_contract_id =
                imported_surface.metaprogramming_macro_host_process_cache_contract_id;
            imported_input.metaprogramming_macro_host_process_cache_source_contract_id =
                imported_surface
                    .metaprogramming_macro_host_process_cache_source_contract_id;
            imported_input.metaprogramming_macro_host_process_cache_replay_key =
                imported_surface.metaprogramming_macro_host_process_cache_replay_key;
            imported_input
                .metaprogramming_macro_host_process_cache_host_executable_relative_path =
                imported_surface
                    .metaprogramming_macro_host_process_cache_host_executable_relative_path;
            imported_input.metaprogramming_macro_host_process_cache_root_relative_path =
                imported_surface
                    .metaprogramming_macro_host_process_cache_root_relative_path;
            imported_input.block_ownership_artifact_preservation_present =
                imported_surface.block_ownership_artifact_preservation_present;
            imported_input.block_ownership_runtime_import_artifact_ready =
                imported_surface.block_ownership_runtime_import_artifact_ready;
            imported_input
                .block_ownership_separate_compilation_preservation_ready =
                imported_surface
                    .block_ownership_separate_compilation_preservation_ready;
            imported_input
                .block_ownership_runtime_support_library_link_wiring_ready =
                imported_surface
                    .block_ownership_runtime_support_library_link_wiring_ready;
            imported_input.block_ownership_deterministic =
                imported_surface.block_ownership_deterministic;
            imported_input.block_ownership_contract_id =
                imported_surface.block_ownership_contract_id;
            imported_input.block_ownership_source_contract_id =
                imported_surface.block_ownership_source_contract_id;
            imported_input.block_ownership_object_invoke_thunk_lowering_contract_id =
                imported_surface
                    .block_ownership_object_invoke_thunk_lowering_contract_id;
            imported_input.block_ownership_byref_helper_lowering_contract_id =
                imported_surface
                    .block_ownership_byref_helper_lowering_contract_id;
            imported_input.block_ownership_escape_runtime_hook_lowering_contract_id =
                imported_surface
                    .block_ownership_escape_runtime_hook_lowering_contract_id;
            imported_input
                .block_ownership_runtime_support_library_link_wiring_contract_id =
                imported_surface
                    .block_ownership_runtime_support_library_link_wiring_contract_id;
            imported_input.block_ownership_replay_key =
                imported_surface.block_ownership_replay_key;
            imported_input.block_ownership_local_block_literal_sites =
                imported_surface.block_ownership_local_block_literal_sites;
            imported_input
                .block_ownership_local_invoke_trampoline_symbolized_sites =
                imported_surface
                    .block_ownership_local_invoke_trampoline_symbolized_sites;
            imported_input.block_ownership_local_copy_helper_required_sites =
                imported_surface
                    .block_ownership_local_copy_helper_required_sites;
            imported_input.block_ownership_local_dispose_helper_required_sites =
                imported_surface
                    .block_ownership_local_dispose_helper_required_sites;
            imported_input.block_ownership_local_copy_helper_symbolized_sites =
                imported_surface
                    .block_ownership_local_copy_helper_symbolized_sites;
            imported_input.block_ownership_local_dispose_helper_symbolized_sites =
                imported_surface
                    .block_ownership_local_dispose_helper_symbolized_sites;
            imported_input.block_ownership_local_escape_to_heap_sites =
                imported_surface.block_ownership_local_escape_to_heap_sites;
            imported_input.block_ownership_local_byref_layout_symbolized_sites =
                imported_surface
                    .block_ownership_local_byref_layout_symbolized_sites;
            imported_input.storage_reflection_artifact_preservation_present =
                imported_surface
                    .storage_reflection_artifact_preservation_present;
            imported_input.storage_reflection_runtime_import_artifact_ready =
                imported_surface
                    .storage_reflection_runtime_import_artifact_ready;
            imported_input
                .storage_reflection_separate_compilation_preservation_ready =
                imported_surface
                    .storage_reflection_separate_compilation_preservation_ready;
            imported_input.storage_reflection_deterministic =
                imported_surface.storage_reflection_deterministic;
            imported_input.storage_reflection_contract_id =
                imported_surface.storage_reflection_contract_id;
            imported_input.storage_reflection_source_contract_id =
                imported_surface.storage_reflection_source_contract_id;
            imported_input
                .storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id =
                imported_surface
                    .storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id;
            imported_input
                .storage_reflection_executable_property_accessor_layout_lowering_contract_id =
                imported_surface
                    .storage_reflection_executable_property_accessor_layout_lowering_contract_id;
            imported_input
                .storage_reflection_executable_ivar_layout_emission_contract_id =
                imported_surface
                    .storage_reflection_executable_ivar_layout_emission_contract_id;
            imported_input
                .storage_reflection_executable_synthesized_accessor_property_lowering_contract_id =
                imported_surface
                    .storage_reflection_executable_synthesized_accessor_property_lowering_contract_id;
            imported_input.storage_reflection_replay_key =
                imported_surface.storage_reflection_replay_key;
            imported_input.storage_reflection_local_property_descriptor_count =
                imported_surface
                    .storage_reflection_local_property_descriptor_count;
            imported_input.storage_reflection_local_ivar_descriptor_count =
                imported_surface.storage_reflection_local_ivar_descriptor_count;
            imported_input
                .storage_reflection_implementation_owned_property_entries =
                imported_surface
                    .storage_reflection_implementation_owned_property_entries;
            imported_input.storage_reflection_synthesized_accessor_owner_entries =
                imported_surface
                    .storage_reflection_synthesized_accessor_owner_entries;
            imported_input.storage_reflection_synthesized_getter_entries =
                imported_surface
                    .storage_reflection_synthesized_getter_entries;
            imported_input.storage_reflection_synthesized_setter_entries =
                imported_surface
                    .storage_reflection_synthesized_setter_entries;
            imported_input.storage_reflection_synthesized_accessor_entries =
                imported_surface
                    .storage_reflection_synthesized_accessor_entries;
            imported_input.storage_reflection_current_property_read_entries =
                imported_surface
                    .storage_reflection_current_property_read_entries;
            imported_input.storage_reflection_current_property_write_entries =
                imported_surface
                    .storage_reflection_current_property_write_entries;
            imported_input.storage_reflection_current_property_exchange_entries =
                imported_surface
                    .storage_reflection_current_property_exchange_entries;
            imported_input
                .storage_reflection_weak_current_property_load_entries =
                imported_surface
                    .storage_reflection_weak_current_property_load_entries;
            imported_input
                .storage_reflection_weak_current_property_store_entries =
                imported_surface
                    .storage_reflection_weak_current_property_store_entries;
            imported_input.storage_reflection_ivar_layout_entries =
                imported_surface.storage_reflection_ivar_layout_entries;
            imported_input.storage_reflection_ivar_layout_owner_entries =
                imported_surface
                    .storage_reflection_ivar_layout_owner_entries;
            link_plan_inputs.imported_inputs.push_back(std::move(imported_input));
          }

          std::string link_plan_json;
          std::string cross_module_linker_response_payload;
          std::string link_plan_error;
          if (!TryBuildObjc3CrossModuleRuntimeLinkPlanArtifact(
                  link_plan_inputs, link_plan_json,
                  cross_module_linker_response_payload, link_plan_error)) {
            compile_status = 125;
            std::cerr << link_plan_error << "\n";
          } else {
            WriteCrossModuleRuntimeLinkPlanArtifact(cli_options.out_dir,
                                                   cli_options.emit_prefix,
                                                   link_plan_json);
            WriteCrossModuleRuntimeLinkerResponseArtifact(
                cli_options.out_dir, cli_options.emit_prefix,
                cross_module_linker_response_payload);
          }
        }
      }
    }

    const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface toolchain_runtime_core_feature_surface =
        BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface(
            toolchain_runtime_ga_operations_scaffold,
            compile_status,
            backend_output_recorded,
            backend_out,
            backend_output_payload);
    std::string toolchain_runtime_core_feature_reason;
    if (!IsObjc3ToolchainRuntimeGaOperationsCoreFeatureSurfaceReady(
            toolchain_runtime_core_feature_surface,
            toolchain_runtime_core_feature_reason)) {
      std::cerr << "toolchain/runtime core feature fail-closed: "
                << toolchain_runtime_core_feature_reason << "\n";
      return 3;
    }

    return 0;
  } catch (const std::exception &io_error) {
    std::cerr << "artifact io failure: " << io_error.what() << "\n";
    return 3;
  }
}

