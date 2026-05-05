               frontend_metadata_
                   .runtime_metadata_section_publication_class_aggregate_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_publication_protocol_aggregate_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_publication_category_aggregate_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_publication_property_aggregate_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_publication_ivar_aggregate_symbol)
        << "\"}\n";
    Objc3RuntimeMetadataLayoutPolicy runtime_metadata_layout_policy;
    std::string runtime_metadata_layout_policy_error;
    if (!TryBuildRuntimeMetadataLayoutPolicy(runtime_metadata_layout_policy,
                                             runtime_metadata_layout_policy_error) &&
        runtime_metadata_layout_policy.failure_reason.empty()) {
      runtime_metadata_layout_policy.failure_reason =
          runtime_metadata_layout_policy_error;
    }
    out << "!50 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_object_inspection_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_object_inspection_publication_contract_id)
        << "\", i1 "
        << (frontend_metadata_.runtime_metadata_object_inspection_matrix_published
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_object_inspection_fail_closed
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_object_inspection_uses_llvm_readobj
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_object_inspection_uses_llvm_objdump
                ? 1
                : 0)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .runtime_metadata_object_inspection_matrix_row_count)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_object_inspection_fixture_path)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_object_inspection_emit_prefix)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_object_inspection_object_relative_path)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_object_inspection_section_inventory_row_key)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_object_inspection_section_inventory_command)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_object_inspection_symbol_inventory_row_key)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_object_inspection_symbol_inventory_command)
        << "\"}\n";
    out << "!51 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_metadata_publication_contract_id)
        << "\", i1 "
        << (frontend_metadata_.runtime_support_library_boundary_frozen ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_support_library_fail_closed ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_support_library_target_name_frozen ? 1
                                                                          : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_exported_entrypoints_frozen
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_ownership_boundaries_frozen
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_support_library_build_constraints_frozen
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_test_dispatch_surface_remains_non_authoritative
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_support_library_native_library_present
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_driver_link_wiring_pending
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_support_library_ready_for_skeleton ? 1
                                                                          : 0)
        << ", !\""
        << EscapeCStringLiteral(frontend_metadata_.runtime_support_library_target_name)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_public_header_path)
        << "\", !\""
        << EscapeCStringLiteral(frontend_metadata_.runtime_support_library_source_root)
        << "\", !\""
        << EscapeCStringLiteral(frontend_metadata_.runtime_support_library_library_kind)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_archive_basename)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_register_image_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_lookup_selector_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_dispatch_i32_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_reset_for_testing_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_driver_link_mode)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_compiler_ownership_boundary)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_runtime_ownership_boundary)
        << "\"}\n";
    out << "!52 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_core_feature_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_support_library_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_metadata_publication_contract_id)
        << "\", i1 "
        << (frontend_metadata_.runtime_support_library_core_feature_fail_closed
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_support_library_core_feature_sources_present
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_support_library_core_feature_header_present
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_core_feature_archive_build_enabled
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_core_feature_entrypoints_implemented
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_core_feature_selector_lookup_stateful
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_core_feature_dispatch_formula_matches_runtime_test_helper
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_core_feature_reset_for_testing_supported
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_core_feature_test_dispatch_surface_remains_non_authoritative
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_core_feature_driver_link_wiring_pending
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_core_feature_ready_for_driver_link_wiring
                ? 1
                : 0)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_core_feature_target_name)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_public_header_path)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_core_feature_source_root)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_implementation_source_path)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_core_feature_library_kind)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_archive_basename)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_archive_relative_path)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_probe_source_path)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_register_image_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_lookup_selector_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_dispatch_i32_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_reset_for_testing_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_driver_link_mode)
        << "\"}\n";
    out << "!53 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_link_wiring_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_link_wiring_core_feature_contract_id)
        << "\", i1 "
        << (frontend_metadata_.runtime_support_library_link_wiring_fail_closed
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_link_wiring_archive_available
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_link_wiring_compatibility_dispatch_alias_exported
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_link_wiring_driver_emits_runtime_link_contract
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_link_wiring_execution_smoke_consumes_runtime_library
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_link_wiring_test_dispatch_surface_remains_non_authoritative
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_link_wiring_ready_for_runtime_library_consumption
                ? 1
                : 0)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_link_wiring_archive_relative_path)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_link_wiring_compatibility_dispatch_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_link_wiring_runtime_dispatch_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_link_wiring_execution_smoke_script_path)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_link_wiring_driver_link_mode)
        << "\"}\n";
    out << "!54 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_typed_handoff_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_source_graph_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_named_metadata_name)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_manifest_surface_path)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_typed_handoff_surface_path)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_source_graph_surface_path)
        << "\", i1 "
        << (frontend_metadata_
                    .executable_metadata_debug_projection_matrix_published
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.executable_metadata_debug_projection_fail_closed
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .executable_metadata_debug_projection_manifest_debug_surface_published
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .executable_metadata_debug_projection_ir_named_metadata_published
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .executable_metadata_debug_projection_replay_anchor_deterministic
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .executable_metadata_debug_projection_active_typed_handoff_ready
                ? 1
                : 0)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .executable_metadata_debug_projection_matrix_row_count)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.executable_metadata_debug_projection_replay_key)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_active_typed_handoff_replay_key)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_row0_descriptor)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_row1_descriptor)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_row2_descriptor)
        << "\"}\n";
    out << "!55 = !{!\""
        << EscapeCStringLiteral(runtime_metadata_layout_policy.contract_id)
        << "\", !\""
        << EscapeCStringLiteral(runtime_metadata_layout_policy.abi_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.scaffold_contract_id)
        << "\", i1 "
        << (runtime_metadata_layout_policy.ready ? 1 : 0) << ", i1 "
        << (runtime_metadata_layout_policy.fail_closed ? 1 : 0) << ", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.family_ordering_model)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.descriptor_ordering_model)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.aggregate_relocation_policy)
        << "\", !\""
        << EscapeCStringLiteral(runtime_metadata_layout_policy.comdat_policy)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.visibility_spelling_policy)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.retention_ordering_model)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.object_format_policy_model)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.object_format_surface_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(runtime_metadata_layout_policy.object_format)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.section_spelling_model)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.retention_anchor_model)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.descriptor_linkage)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.aggregate_linkage)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.metadata_visibility)
        << "\", !\""
        << EscapeCStringLiteral(runtime_metadata_layout_policy.retention_root)
        << "\", i64 "
        << static_cast<unsigned long long>(
               runtime_metadata_layout_policy.total_retained_global_count)
        << ", !\""
        << EscapeCStringLiteral(Objc3RuntimeMetadataLayoutPolicyReplayKey(
               runtime_metadata_layout_policy))
        << "\", !\""
        << EscapeCStringLiteral(runtime_metadata_layout_policy.failure_reason)
        << "\"}\n";
    std::size_t runtime_metadata_class_bundle_count = 0;
    std::size_t runtime_metadata_instance_method_reference_total = 0;
    std::size_t runtime_metadata_class_method_reference_total = 0;
    for (const auto &bundle :
         frontend_metadata_
             .runtime_metadata_class_metaclass_bundles_lexicographic) {
      ++runtime_metadata_class_bundle_count;
      runtime_metadata_instance_method_reference_total +=
          bundle.instance_method_count;
      runtime_metadata_class_method_reference_total += bundle.class_method_count;
    }
    out << "!56 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_class_metaclass_emission_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_class_metaclass_payload_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_class_metaclass_name_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_class_metaclass_super_link_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_class_metaclass_method_list_reference_model)
        << "\", i1 "
        << (frontend_metadata_.runtime_metadata_class_metaclass_emission_ready
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_metadata_class_metaclass_emission_fail_closed
                ? 1
                : 0)
        << ", i64 "
        << static_cast<unsigned long long>(runtime_metadata_class_bundle_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               runtime_metadata_instance_method_reference_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               runtime_metadata_class_method_reference_total)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_class_metaclass_typed_handoff_replay_key)
        << "\"}\n";
    std::size_t runtime_metadata_protocol_bundle_count = 0;
    std::size_t runtime_metadata_protocol_inherited_reference_total = 0;
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_protocol_bundles_lexicographic) {
      ++runtime_metadata_protocol_bundle_count;
      runtime_metadata_protocol_inherited_reference_total +=
          bundle.inherited_protocol_owner_identities_lexicographic.size();
    }
    std::size_t runtime_metadata_category_bundle_count = 0;
    std::size_t runtime_metadata_category_adopted_reference_total = 0;
    std::size_t runtime_metadata_category_attachment_reference_total = 0;
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_category_bundles_lexicographic) {
      ++runtime_metadata_category_bundle_count;
      runtime_metadata_category_adopted_reference_total +=
          bundle.adopted_protocol_owner_identities_lexicographic.size();
      runtime_metadata_category_attachment_reference_total += 3u;
    }
    out << "!57 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_protocol_category_emission_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_protocol_emission_payload_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_category_emission_payload_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_protocol_reference_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_category_attachment_model)
        << "\", i1 "
        << (frontend_metadata_.runtime_metadata_protocol_category_emission_ready
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_metadata_protocol_category_emission_fail_closed
                ? 1
                : 0)
        << ", i64 "
        << static_cast<unsigned long long>(runtime_metadata_protocol_bundle_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               runtime_metadata_protocol_inherited_reference_total)
        << ", i64 "
        << static_cast<unsigned long long>(runtime_metadata_category_bundle_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               runtime_metadata_category_adopted_reference_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               runtime_metadata_category_attachment_reference_total)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_protocol_category_typed_handoff_replay_key)
        << "\"}\n";
    std::size_t runtime_metadata_method_list_bundle_count = 0;
    std::size_t runtime_metadata_method_entry_total = 0;
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_method_list_bundles_lexicographic) {
      ++runtime_metadata_method_list_bundle_count;
      runtime_metadata_method_entry_total += bundle.entries_lexicographic.size();
    }
    out << "!58 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_member_table_emission_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_method_list_emission_payload_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_method_list_grouping_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_property_descriptor_emission_payload_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_ivar_descriptor_emission_payload_model)
        << "\", i1 "
        << (frontend_metadata_.runtime_metadata_member_table_emission_ready ? 1
                                                                            : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_member_table_emission_fail_closed
                ? 1
                : 0)
        << ", i64 "
        << static_cast<unsigned long long>(runtime_metadata_method_list_bundle_count)
        << ", i64 "
        << static_cast<unsigned long long>(runtime_metadata_method_entry_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.runtime_metadata_property_bundles_lexicographic
                   .size())
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.runtime_metadata_ivar_bundles_lexicographic
                   .size())
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_member_table_typed_handoff_replay_key)
        << "\"}\n";
    out << "!59 = !{!\""
        << EscapeCStringLiteral(
               kObjc3RuntimeSelectorStringPoolEmissionContractId)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeSelectorPoolEmissionPayloadModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeStringPoolEmissionPayloadModel)
        << "\", i64 "
        << static_cast<unsigned long long>(selector_pool_globals_.size())
        << ", i64 "
        << static_cast<unsigned long long>(runtime_string_pool_globals_.size())
        << ", !\""
        << EscapeCStringLiteral(Objc3RuntimeMetadataHostSectionForLogicalName(
               kObjc3RuntimeSelectorPoolLogicalSection))
        << "\", !\""
        << EscapeCStringLiteral(Objc3RuntimeMetadataHostSectionForLogicalName(
               kObjc3RuntimeStringPoolLogicalSection))
        << "\"}\n";
    out << "!60 = !{!\""
        << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionHarnessContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionPositiveCorpusModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionNegativeCorpusModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionSectionCommand)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionSymbolCommand)
        << "\", i64 4, i64 1}\n";
    out << "!61 = !{!\""
        << EscapeCStringLiteral(
               kObjc3RuntimeObjectPackagingRetentionContractId)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeObjectPackagingRetentionBoundaryModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeObjectPackagingRetentionAnchorModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeObjectPackagingRetentionArtifact)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeObjectPackagingRetentionSymbolPrefix)
        << "\"}\n";
    out << "!62 = !{!\""
        << EscapeCStringLiteral(kObjc3RuntimeLinkerRetentionContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLinkerRetentionAnchorModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLinkerDiscoveryModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLinkerAnchorLogicalSection)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLinkerDiscoveryRootLogicalSection)
        << "\", !\""
        << EscapeCStringLiteral(RuntimeMetadataLinkerAnchorSymbol())
        << "\", !\""
        << EscapeCStringLiteral(RuntimeMetadataDiscoveryRootSymbol())
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLinkerResponseArtifactSuffix)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLinkerDiscoveryArtifactSuffix)
        << "\"}\n";
    out << "!63 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_archive_static_link_discovery_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_archive_static_link_anchor_seed_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_archive_static_link_translation_unit_identity_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_archive_static_link_merge_model)
        << "\", i1 "
        << (frontend_metadata_
                    .runtime_metadata_archive_static_link_discovery_ready
                ? 1
                : 0)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_archive_static_link_response_artifact_suffix)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_archive_static_link_discovery_artifact_suffix)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_archive_static_link_translation_unit_identity_key)
        << "\"}\n";
    out << "!64 = !{!\""
        << EscapeCStringLiteral(kObjc3RuntimeMetadataEmissionGateContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeMetadataEmissionGateEvidenceModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeMetadataEmissionGateFailureModel)
        << "\"}\n";
    out << "!65 = !{!\""
        << EscapeCStringLiteral(
               kObjc3RuntimeMetadataObjectEmissionCloseoutContractId)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeMetadataObjectEmissionCloseoutEvidenceModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeMetadataObjectEmissionCloseoutFailureModel)
        << "\"}\n";
    out << "!66 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.dispatch_surface_classification_instance_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.dispatch_surface_classification_class_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.dispatch_surface_classification_super_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.dispatch_surface_classification_direct_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.dispatch_surface_classification_dynamic_sites)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .dispatch_surface_classification_instance_entrypoint_family)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .dispatch_surface_classification_class_entrypoint_family)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .dispatch_surface_classification_super_entrypoint_family)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .dispatch_surface_classification_direct_entrypoint_family)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .dispatch_surface_classification_dynamic_entrypoint_family)
        << "\", i1 "
        << (frontend_metadata_.deterministic_dispatch_surface_classification_handoff
                ? 1
                : 0)
        << "}\n";
    out << "!67 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_.executable_ivar_layout_emission_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.executable_ivar_layout_descriptor_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.executable_ivar_offset_global_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.executable_ivar_layout_table_model)
        << "\", i1 "
        << (frontend_metadata_.executable_ivar_layout_emission_ready ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.executable_ivar_layout_emission_fail_closed ? 1
                                                                           : 0)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.executable_ivar_offset_global_entries)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.executable_ivar_layout_table_entries)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.executable_ivar_layout_owner_entries)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.executable_ivar_layout_emission_replay_key)
        << "\"}\n";
    out << "!68 = !{!\""
        << EscapeCStringLiteral(
               kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3ExecutableSynthesizedAccessorPropertyLoweringSourceModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3ExecutableSynthesizedAccessorPropertyLoweringStorageModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3ExecutableSynthesizedAccessorPropertyLoweringPropertyDescriptorModel)
        << "\", i64 "
        << static_cast<unsigned long long>(synthesized_property_accessor_count_)
        << "}\n";
    out << "!69 = !{!\""
        << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionAccessorModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3OwnershipRuntimeHookEmissionPropertyContextModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3OwnershipRuntimeHookEmissionAutoreleaseModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3OwnershipRuntimeHookEmissionFailClosedModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
        << "\", i64 "
        << static_cast<unsigned long long>(synthesized_property_accessor_count_)
        << "}\n";
    out << "!70 = !{!\""
        << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiReferenceModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiWeakModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeMemoryManagementApiAutoreleasepoolModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiFailClosedModel)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
        << "\", i64 "
        << static_cast<unsigned long long>(synthesized_property_accessor_count_)
        << "}\n";
    out << "!71 = !{!\""
        << EscapeCStringLiteral(
               kObjc3RuntimeMemoryManagementImplementationContractId)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeMemoryManagementImplementationRefcountModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeMemoryManagementImplementationWeakModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeMemoryManagementImplementationAutoreleasepoolModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeMemoryManagementImplementationFailClosedModel)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
        << "\", i64 "
        << static_cast<unsigned long long>(synthesized_property_accessor_count_)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.autoreleasepool_scope_lowering_scope_sites)
        << "}\n";
    out << "!72 = !{!\""
        << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateSupportedModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateEvidenceModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateNonGoalModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateFailClosedModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiContractId)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeMemoryManagementImplementationContractId)
        << "\"}\n";
    out << "!73 = !{!\""
        << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateEvidenceModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateActiveModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateNonGoalModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateFailClosedModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ExecutableBlockSourceStorageAnnotationContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ExecutableBlockOwnershipSemanticsImplementationContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeBlockByrefForwardingHeapPromotionInteropContractId)
        << "\"}\n";
    out << "!74 = !{!\""
        << EscapeCStringLiteral(
               Expr::kObjc3RunnableBlockExecutionMatrixContractId)
        << "\", !\""
        << EscapeCStringLiteral(
               Expr::kObjc3RunnableBlockExecutionMatrixEvidenceModel)
        << "\", !\""
        << EscapeCStringLiteral(
               Expr::kObjc3RunnableBlockExecutionMatrixActiveModel)
        << "\", !\""
        << EscapeCStringLiteral(
               Expr::kObjc3RunnableBlockExecutionMatrixNonGoalModel)
        << "\", !\""
        << EscapeCStringLiteral(
               Expr::kObjc3RunnableBlockExecutionMatrixFailClosedModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateContractId)
        << "\"}\n";
    out << "!75 = !{!\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundarySourceModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryModeModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3OwnershipQualifierLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3AutoreleasePoolScopeLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcDiagnosticsFixitLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryNonGoalModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryFailClosedModel)
        << "\"}\n";
    out << "!76 = !{!\""
        << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingSourceModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingModeModel)
        << "\", !\""
        << EscapeCStringLiteral(frontend_metadata_.arc_mode)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3OwnershipQualifierLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3AutoreleasePoolScopeLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcDiagnosticsFixitLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingFailClosedModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingNonGoalModel)
        << "\"}\n";
    out << "!77 = !{!\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesSourceModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesSemanticModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcDiagnosticsFixitLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesFailClosedModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesNonGoalModel)
        << "\"}\n";
    out << "!78 = !{!\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeSourceModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeSemanticModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3BlockStorageEscapeLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeFailClosedModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeNonGoalModel)
        << "\"}\n";
    out << "!79 = !{!\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsSourceModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsSemanticModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3AutoreleasePoolScopeLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3BlockStorageEscapeLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsFailClosedModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsNonGoalModel)
        << "\"}\n";
    out << "!80 = !{!\""
        << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionSourceModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionLoweringModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcLoweringAbiCleanupModelContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionFailureModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionNonGoalModel)
        << "\"}\n";
    out << "!81 = !{!\""
        << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksSourceModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksLoweringModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcLoweringAbiCleanupModelContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksFailureModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksNonGoalModel)
        << "\"}\n";
    out << "!82 = !{!\""
        << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringSourceModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePromoteBlockI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeInvokeBlockI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringFailureModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringNonGoalModel)
        << "\"}\n";
    out << "!83 = !{!\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceReferenceModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceWeakModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceAutoreleasepoolModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceFailClosedModel)
        << "\"}\n";
    out << "!84 = !{!\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportDependencyModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportWeakModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportAutoreleaseReturnModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportExecutionModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportFailClosedModel)
        << "\"}\n";
    out << "!85 = !{!\""
        << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationDependencyModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationCoverageModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationValidationModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationFailClosedModel)
        << "\"}\n";
    out << "!86 = !{!\""
        << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateEvidenceModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateActiveModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateNonGoalModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcModeHandlingContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcInteractionSemanticsContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateFailClosedModel)
        << "\"}\n";
    out << "!87 = !{!\""
        << EscapeCStringLiteral(kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ErrorHandlingThrowsAbiPropagationLoweringSourceModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ErrorHandlingThrowsAbiPropagationLoweringAbiModel)
        << "\", !\""
        << EscapeCStringLiteral(frontend_metadata_.lowering_throws_propagation_replay_key)
        << "\", !\""
        << EscapeCStringLiteral(frontend_metadata_.lowering_result_like_replay_key)
        << "\", !\""
        << EscapeCStringLiteral(frontend_metadata_.lowering_ns_error_bridging_replay_key)
        << "\", !\""
        << EscapeCStringLiteral(frontend_metadata_.lowering_unwind_cleanup_replay_key)
        << "\", i1 "
        << (frontend_metadata_.deterministic_throws_propagation_lowering_handoff
                    ? 1
                    : 0)
        << ", i1 "
        << (frontend_metadata_.deterministic_result_like_lowering_handoff ? 1
                                                                          : 0)
        << ", i1 "
        << (frontend_metadata_.deterministic_ns_error_bridging_lowering_handoff
                    ? 1
                    : 0)
        << ", i1 "
        << (frontend_metadata_.deterministic_unwind_cleanup_lowering_handoff
                    ? 1
                    : 0)
        << ", !\""
        << EscapeCStringLiteral(
               kObjc3ErrorHandlingThrowsAbiPropagationLoweringFailClosedModel)
        << "\"}\n";
    out << "!88 = !{!\""
        << EscapeCStringLiteral(
               kObjc3ErrorHandlingResultAndBridgingArtifactReplayContractId)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3ErrorHandlingResultAndBridgingArtifactReplaySourceModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3ErrorHandlingResultAndBridgingArtifactReplayModel)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .lowering_error_handling_result_and_bridging_artifact_replay_key)
        << "\", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .imported_error_handling_result_and_bridging_artifact_modules)
        << ", i1 "
        << (frontend_metadata_
                    .error_handling_result_and_bridging_binary_artifact_replay_ready
                    ? 1
                    : 0)
        << ", i1 "
        << (frontend_metadata_
                    .error_handling_result_and_bridging_runtime_import_artifact_ready
                    ? 1
                    : 0)
        << ", i1 "
        << (frontend_metadata_
                    .error_handling_result_and_bridging_separate_compilation_replay_ready
                    ? 1
                    : 0)
        << ", i1 "
        << (frontend_metadata_
                    .deterministic_error_handling_result_and_bridging_artifact_replay_handoff
                    ? 1
                    : 0)
        << ", !\""
        << EscapeCStringLiteral(
               kObjc3ErrorHandlingResultAndBridgingArtifactReplayFailClosedModel)
        << "\"}\n";
    out << "!89 = !{!\""
        << EscapeCStringLiteral(kObjc3ErrorHandlingErrorRuntimeBridgeHelperContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ErrorHandlingErrorRuntimeBridgeHelperSourceModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ErrorHandlingErrorRuntimeBridgeHelperAbiModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeStoreThrownErrorI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLoadThrownErrorI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeBridgeStatusErrorI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeBridgeNSErrorErrorI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeCatchMatchesErrorI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3ErrorHandlingErrorRuntimeBridgeHelperFailClosedModel)
        << "\"}\n";
    out << "!90 = !{!\""
        << EscapeCStringLiteral(kObjc3ErrorHandlingLiveErrorRuntimeIntegrationContractId)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3ErrorHandlingLiveErrorRuntimeIntegrationSourceModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3ErrorHandlingLiveErrorRuntimeIntegrationExecutionModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3ErrorHandlingLiveErrorRuntimeIntegrationPackagingModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeStoreThrownErrorI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLoadThrownErrorI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeBridgeStatusErrorI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeCatchMatchesErrorI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3ErrorHandlingLiveErrorRuntimeIntegrationFailClosedModel)
        << "\"}\n";
    out << "!91 = !{!\""
        << EscapeCStringLiteral(kObjc3ConcurrencyContinuationRuntimeHelperContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ConcurrencyContinuationRuntimeHelperSourceModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ConcurrencyContinuationRuntimeHelperAbiModel)
        << "\", !\""
        << EscapeCStringLiteral(
