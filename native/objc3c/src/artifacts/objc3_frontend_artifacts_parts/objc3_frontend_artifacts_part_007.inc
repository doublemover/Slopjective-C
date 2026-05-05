        &local_runtime_registration_manifest,
    std::size_t direct_import_input_count) {
  Objc3CrossModuleBuildRuntimeOrchestrationSummary summary;
  summary.module_names_lexicographic =
      serialized_runtime_metadata_artifact_reuse
          .reused_module_names_lexicographic;
  summary.module_image_count = summary.module_names_lexicographic.size();
  summary.direct_import_input_count = direct_import_input_count;
  summary.local_class_descriptor_count =
      local_runtime_registration_manifest.class_descriptor_count;
  summary.local_protocol_descriptor_count =
      local_runtime_registration_manifest.protocol_descriptor_count;
  summary.local_category_descriptor_count =
      local_runtime_registration_manifest.category_descriptor_count;
  summary.local_property_descriptor_count =
      local_runtime_registration_manifest.property_descriptor_count;
  summary.local_ivar_descriptor_count =
      local_runtime_registration_manifest.ivar_descriptor_count;
  summary.local_total_descriptor_count =
      local_runtime_registration_manifest.total_descriptor_count;
  summary.transitive_runtime_owned_declaration_count =
      serialized_runtime_metadata_artifact_reuse
          .runtime_owned_declaration_count;
  summary.transitive_metadata_reference_count =
      serialized_runtime_metadata_artifact_reuse.metadata_reference_count;
  summary.imported_optional_send_site_count =
      imported_runtime_metadata_semantic_rules.optional_send_site_count;
  summary.imported_typed_keypath_literal_site_count =
      imported_runtime_metadata_semantic_rules.typed_keypath_literal_site_count;
  summary.imported_live_optional_lowering_site_count =
      imported_runtime_metadata_semantic_rules.live_optional_lowering_site_count;
  summary.imported_live_typed_keypath_artifact_site_count =
      imported_runtime_metadata_semantic_rules
          .live_typed_keypath_artifact_site_count;
  summary.fail_closed = true;
  summary.source_serialized_runtime_metadata_artifact_reuse_ready =
      IsReadyObjc3SerializedRuntimeMetadataArtifactReuseSummary(
          serialized_runtime_metadata_artifact_reuse);
  summary.source_imported_runtime_metadata_semantic_rules_ready =
      IsReadyObjc3ImportedRuntimeMetadataSemanticRulesSummary(
          imported_runtime_metadata_semantic_rules);
  summary.source_local_registration_manifest_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
          local_runtime_registration_manifest);
  summary.semantic_surface_published = true;
  summary.local_registration_manifest_emitted =
      local_runtime_registration_manifest
          .runtime_registration_artifact_emitted_by_driver;
  summary.imported_type_system_type_surface_landed =
      imported_runtime_metadata_semantic_rules.imported_type_system_type_surface_landed;
  summary.imported_optional_runtime_semantics_landed =
      imported_runtime_metadata_semantic_rules
          .imported_optional_runtime_semantics_landed;
  summary.imported_typed_keypath_runtime_semantics_landed =
      imported_runtime_metadata_semantic_rules
          .imported_typed_keypath_runtime_semantics_landed;
  summary.source_serialized_runtime_metadata_replay_key =
      serialized_runtime_metadata_artifact_reuse.replay_key;
  summary.source_local_registration_manifest_replay_key =
      local_runtime_registration_manifest.replay_key;
  summary.source_imported_runtime_metadata_semantic_rules_replay_key =
      imported_runtime_metadata_semantic_rules.replay_key;
  if (summary.source_serialized_runtime_metadata_artifact_reuse_ready &&
      summary.source_imported_runtime_metadata_semantic_rules_ready &&
      summary.source_local_registration_manifest_ready) {
    summary.replay_key =
        BuildCrossModuleBuildRuntimeOrchestrationReplayKey(summary);
  } else {
    summary.failure_reason =
        "cross-module build/runtime orchestration contract is incomplete";
  }
  if (!IsReadyObjc3CrossModuleBuildRuntimeOrchestrationSummary(summary) &&
      summary.failure_reason.empty()) {
    summary.failure_reason =
        "cross-module build/runtime orchestration contract is incomplete";
  }
  return summary;
}

std::string BuildCrossModuleBuildRuntimeOrchestrationSummaryJson(
    const Objc3CrossModuleBuildRuntimeOrchestrationSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_serialized_runtime_metadata_artifact_reuse_contract_id\":\""
      << EscapeJsonString(
             summary
                 .source_serialized_runtime_metadata_artifact_reuse_contract_id)
      << "\",\"source_local_registration_manifest_contract_id\":\""
      << EscapeJsonString(
             summary.source_local_registration_manifest_contract_id)
      << "\",\"source_imported_runtime_metadata_semantic_rules_contract_id\":\""
      << EscapeJsonString(
             summary.source_imported_runtime_metadata_semantic_rules_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"import_artifact_relative_path\":\""
      << EscapeJsonString(summary.import_artifact_relative_path)
      << "\",\"local_registration_manifest_artifact_relative_path\":\""
      << EscapeJsonString(
             summary.local_registration_manifest_artifact_relative_path)
      << "\",\"authority_model\":\""
      << EscapeJsonString(summary.authority_model)
      << "\",\"input_model\":\"" << EscapeJsonString(summary.input_model)
      << "\",\"registration_scope_model\":\""
      << EscapeJsonString(summary.registration_scope_model)
      << "\",\"packaging_model\":\""
      << EscapeJsonString(summary.packaging_model)
      << "\",\"module_names_lexicographic\":"
      << BuildStringArrayJson(summary.module_names_lexicographic)
      << ",\"module_image_count\":" << summary.module_image_count
      << ",\"direct_import_input_count\":" << summary.direct_import_input_count
      << ",\"local_class_descriptor_count\":"
      << summary.local_class_descriptor_count
      << ",\"local_protocol_descriptor_count\":"
      << summary.local_protocol_descriptor_count
      << ",\"local_category_descriptor_count\":"
      << summary.local_category_descriptor_count
      << ",\"local_property_descriptor_count\":"
      << summary.local_property_descriptor_count
      << ",\"local_ivar_descriptor_count\":"
      << summary.local_ivar_descriptor_count
      << ",\"local_total_descriptor_count\":"
      << summary.local_total_descriptor_count
      << ",\"transitive_runtime_owned_declaration_count\":"
      << summary.transitive_runtime_owned_declaration_count
      << ",\"transitive_metadata_reference_count\":"
      << summary.transitive_metadata_reference_count
      << ",\"imported_optional_send_site_count\":"
      << summary.imported_optional_send_site_count
      << ",\"imported_typed_keypath_literal_site_count\":"
      << summary.imported_typed_keypath_literal_site_count
      << ",\"imported_live_optional_lowering_site_count\":"
      << summary.imported_live_optional_lowering_site_count
      << ",\"imported_live_typed_keypath_artifact_site_count\":"
      << summary.imported_live_typed_keypath_artifact_site_count
      << ",\"ready\":"
      << (IsReadyObjc3CrossModuleBuildRuntimeOrchestrationSummary(summary)
              ? "true"
              : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"source_serialized_runtime_metadata_artifact_reuse_ready\":"
      << (summary.source_serialized_runtime_metadata_artifact_reuse_ready
              ? "true"
              : "false")
      << ",\"source_local_registration_manifest_ready\":"
      << (summary.source_local_registration_manifest_ready ? "true" : "false")
      << ",\"source_imported_runtime_metadata_semantic_rules_ready\":"
      << (summary.source_imported_runtime_metadata_semantic_rules_ready ? "true"
                                                                        : "false")
      << ",\"semantic_surface_published\":"
      << (summary.semantic_surface_published ? "true" : "false")
      << ",\"local_registration_manifest_emitted\":"
      << (summary.local_registration_manifest_emitted ? "true" : "false")
      << ",\"imported_type_system_type_surface_landed\":"
      << (summary.imported_type_system_type_surface_landed ? "true" : "false")
      << ",\"imported_optional_runtime_semantics_landed\":"
      << (summary.imported_optional_runtime_semantics_landed ? "true"
                                                             : "false")
      << ",\"imported_typed_keypath_runtime_semantics_landed\":"
      << (summary.imported_typed_keypath_runtime_semantics_landed ? "true"
                                                                  : "false")
      << ",\"cross_module_link_plan_artifact_landed\":"
      << (summary.cross_module_link_plan_artifact_landed ? "true" : "false")
      << ",\"imported_registration_manifest_loading_landed\":"
      << (summary.imported_registration_manifest_loading_landed ? "true"
                                                                : "false")
      << ",\"runtime_archive_aggregation_landed\":"
      << (summary.runtime_archive_aggregation_landed ? "true" : "false")
      << ",\"cross_module_runtime_registration_landed\":"
      << (summary.cross_module_runtime_registration_landed ? "true" : "false")
      << ",\"cross_module_launch_orchestration_landed\":"
      << (summary.cross_module_launch_orchestration_landed ? "true"
                                                           : "false")
      << ",\"public_cross_module_orchestration_abi_landed\":"
      << (summary.public_cross_module_orchestration_abi_landed ? "true"
                                                               : "false")
      << ",\"ready_for_packaging_and_runtime_registration_impl\":"
      << (summary.ready_for_packaging_and_runtime_registration_impl ? "true"
                                                                   : "false")
      << ",\"source_serialized_runtime_metadata_replay_key\":\""
      << EscapeJsonString(
             summary.source_serialized_runtime_metadata_replay_key)
      << "\",\"source_local_registration_manifest_replay_key\":\""
      << EscapeJsonString(summary.source_local_registration_manifest_replay_key)
      << "\",\"source_imported_runtime_metadata_semantic_rules_replay_key\":\""
      << EscapeJsonString(
             summary.source_imported_runtime_metadata_semantic_rules_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildRuntimeOwnedDeclarationsJson(
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  std::ostringstream out;
  out << "{\n"
      << "    \"classes\": [\n";
  for (std::size_t i = 0; i < runtime_metadata_source_records.classes_lexicographic.size(); ++i) {
    const auto &class_record = runtime_metadata_source_records.classes_lexicographic[i];
    out << "      {\"record_kind\":\"" << EscapeJsonString(class_record.record_kind)
        << "\",\"name\":\"" << EscapeJsonString(class_record.name)
        << "\",\"super_name\":\"" << EscapeJsonString(class_record.super_name)
        << "\",\"has_super\":" << (class_record.has_super ? "true" : "false")
        << ",\"objc_final_declared\":"
        << (class_record.objc_final_declared ? "true" : "false")
        << ",\"objc_sealed_declared\":"
        << (class_record.objc_sealed_declared ? "true" : "false")
        << ",\"adopted_protocols\":"
        << BuildStringArrayJson(class_record.adopted_protocols_lexicographic)
        << ",\"property_count\":" << class_record.property_count
        << ",\"method_count\":" << class_record.method_count
        << ",\"line\":" << class_record.line
        << ",\"column\":" << class_record.column << "}";
    if (i + 1 != runtime_metadata_source_records.classes_lexicographic.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "    ],\n"
      << "    \"protocols\": [\n";
  for (std::size_t i = 0; i < runtime_metadata_source_records.protocols_lexicographic.size(); ++i) {
    const auto &protocol_record = runtime_metadata_source_records.protocols_lexicographic[i];
    out << "      {\"name\":\"" << EscapeJsonString(protocol_record.name)
        << "\",\"inherited_protocols\":"
        << BuildStringArrayJson(protocol_record.inherited_protocols_lexicographic)
        << ",\"is_forward_declaration\":"
        << (protocol_record.is_forward_declaration ? "true" : "false")
        << ",\"property_count\":" << protocol_record.property_count
        << ",\"method_count\":" << protocol_record.method_count
        << ",\"line\":" << protocol_record.line
        << ",\"column\":" << protocol_record.column << "}";
    if (i + 1 != runtime_metadata_source_records.protocols_lexicographic.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "    ],\n"
      << "    \"categories\": [\n";
  for (std::size_t i = 0; i < runtime_metadata_source_records.categories_lexicographic.size(); ++i) {
    const auto &category_record = runtime_metadata_source_records.categories_lexicographic[i];
    out << "      {\"record_kind\":\"" << EscapeJsonString(category_record.record_kind)
        << "\",\"class_name\":\"" << EscapeJsonString(category_record.class_name)
        << "\",\"category_name\":\"" << EscapeJsonString(category_record.category_name)
        << "\",\"adopted_protocols\":"
        << BuildStringArrayJson(category_record.adopted_protocols_lexicographic)
        << ",\"property_count\":" << category_record.property_count
        << ",\"method_count\":" << category_record.method_count
        << ",\"line\":" << category_record.line
        << ",\"column\":" << category_record.column << "}";
    if (i + 1 != runtime_metadata_source_records.categories_lexicographic.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "    ],\n"
      << "    \"properties\": [\n";
  for (std::size_t i = 0; i < runtime_metadata_source_records.properties_lexicographic.size(); ++i) {
    const auto &property_record = runtime_metadata_source_records.properties_lexicographic[i];
    out << "      {\"owner_kind\":\"" << EscapeJsonString(property_record.owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(property_record.owner_name)
        << "\",\"property_name\":\"" << EscapeJsonString(property_record.property_name)
        << "\",\"type\":\"" << EscapeJsonString(property_record.type_name)
        << "\",\"effective_getter_selector\":\""
        << EscapeJsonString(property_record.effective_getter_selector)
        << "\",\"effective_setter_available\":"
        << (property_record.effective_setter_available ? "true" : "false")
        << ",\"effective_setter_selector\":\""
        << EscapeJsonString(property_record.effective_setter_selector)
        << "\",\"ivar_binding_symbol\":\""
        << EscapeJsonString(property_record.ivar_binding_symbol)
        << "\",\"executable_synthesized_binding_kind\":\""
        << EscapeJsonString(property_record.executable_synthesized_binding_kind)
        << "\",\"executable_synthesized_binding_symbol\":\""
        << EscapeJsonString(property_record.executable_synthesized_binding_symbol)
        << "\",\"property_attribute_profile\":\""
        << EscapeJsonString(property_record.property_attribute_profile)
        << "\",\"ownership_lifetime_profile\":\""
        << EscapeJsonString(property_record.ownership_lifetime_profile)
        << "\",\"ownership_runtime_hook_profile\":\""
        << EscapeJsonString(property_record.ownership_runtime_hook_profile)
        << "\",\"accessor_ownership_profile\":\""
        << EscapeJsonString(property_record.accessor_ownership_profile)
        << "\",\"synthesizes_executable_accessors\":"
        << (property_record.synthesizes_executable_accessors ? "true" : "false")
        << ",\"getter_storage_runtime_helper_symbol\":\""
        << EscapeJsonString(property_record.getter_storage_runtime_helper_symbol)
        << "\",\"setter_storage_runtime_helper_symbol\":\""
        << EscapeJsonString(property_record.setter_storage_runtime_helper_symbol)
        << "\",\"executable_ivar_layout_symbol\":\""
        << EscapeJsonString(property_record.executable_ivar_layout_symbol)
        << "\",\"executable_ivar_layout_slot_index\":"
        << property_record.executable_ivar_layout_slot_index
        << ",\"executable_ivar_layout_size_bytes\":"
        << property_record.executable_ivar_layout_size_bytes
        << ",\"executable_ivar_layout_alignment_bytes\":"
        << property_record.executable_ivar_layout_alignment_bytes
        << ",\"executable_ivar_layout_offset_bytes\":"
        << property_record.executable_ivar_layout_offset_bytes
        << ",\"executable_ivar_layout_padding_bytes\":"
        << property_record.executable_ivar_layout_padding_bytes
        << ",\"executable_ivar_layout_inherited_slot_count\":"
        << property_record.executable_ivar_layout_inherited_slot_count
        << ",\"executable_ivar_layout_inherited_size_bytes\":"
        << property_record.executable_ivar_layout_inherited_size_bytes
        << ",\"executable_ivar_layout_owner_size_bytes\":"
        << property_record.executable_ivar_layout_owner_size_bytes
        << ",\"executable_ivar_init_order_index\":"
        << property_record.executable_ivar_init_order_index
        << ",\"executable_ivar_destroy_order_index\":"
        << property_record.executable_ivar_destroy_order_index
        << ",\"executable_ivar_layout_valid\":"
        << (property_record.executable_ivar_layout_valid ? "true" : "false")
        << ",\"executable_ivar_layout_replay_key\":\""
        << EscapeJsonString(property_record.executable_ivar_layout_replay_key)
        << "\""
        << ",\"line\":" << property_record.line
        << ",\"column\":" << property_record.column << "}";
    if (i + 1 != runtime_metadata_source_records.properties_lexicographic.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "    ],\n"
      << "    \"methods\": [\n";
  for (std::size_t i = 0; i < runtime_metadata_source_records.methods_lexicographic.size(); ++i) {
    const auto &method_record = runtime_metadata_source_records.methods_lexicographic[i];
    out << "      {\"owner_kind\":\"" << EscapeJsonString(method_record.owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(method_record.owner_name)
        << "\",\"selector\":\"" << EscapeJsonString(method_record.selector)
        << "\",\"is_class_method\":"
        << (method_record.is_class_method ? "true" : "false")
        << ",\"has_body\":" << (method_record.has_body ? "true" : "false")
        << ",\"effective_direct_dispatch\":"
        << (method_record.effective_direct_dispatch ? "true" : "false")
        << ",\"objc_final_declared\":"
        << (method_record.objc_final_declared ? "true" : "false")
        << ",\"parameter_count\":" << method_record.parameter_count
        << ",\"return_type\":\""
        << EscapeJsonString(method_record.return_type_name)
        << "\",\"line\":" << method_record.line
        << ",\"column\":" << method_record.column << "}";
    if (i + 1 != runtime_metadata_source_records.methods_lexicographic.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "    ],\n"
      << "    \"ivars\": [\n";
  for (std::size_t i = 0; i < runtime_metadata_source_records.ivars_lexicographic.size(); ++i) {
    const auto &ivar_record = runtime_metadata_source_records.ivars_lexicographic[i];
    out << "      {\"owner_kind\":\"" << EscapeJsonString(ivar_record.owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(ivar_record.owner_name)
        << "\",\"property_name\":\"" << EscapeJsonString(ivar_record.property_name)
        << "\",\"ivar_binding_symbol\":\""
        << EscapeJsonString(ivar_record.ivar_binding_symbol)
        << "\",\"executable_synthesized_binding_kind\":\""
        << EscapeJsonString(ivar_record.executable_synthesized_binding_kind)
        << "\",\"executable_synthesized_binding_symbol\":\""
        << EscapeJsonString(ivar_record.executable_synthesized_binding_symbol)
        << "\",\"executable_ivar_layout_symbol\":\""
        << EscapeJsonString(ivar_record.executable_ivar_layout_symbol)
        << "\",\"executable_ivar_layout_slot_index\":"
        << ivar_record.executable_ivar_layout_slot_index
        << ",\"executable_ivar_layout_size_bytes\":"
        << ivar_record.executable_ivar_layout_size_bytes
        << ",\"executable_ivar_layout_alignment_bytes\":"
        << ivar_record.executable_ivar_layout_alignment_bytes
        << ",\"executable_ivar_layout_offset_bytes\":"
        << ivar_record.executable_ivar_layout_offset_bytes
        << ",\"executable_ivar_layout_padding_bytes\":"
        << ivar_record.executable_ivar_layout_padding_bytes
        << ",\"executable_ivar_layout_inherited_slot_count\":"
        << ivar_record.executable_ivar_layout_inherited_slot_count
        << ",\"executable_ivar_layout_inherited_size_bytes\":"
        << ivar_record.executable_ivar_layout_inherited_size_bytes
        << ",\"executable_ivar_layout_owner_size_bytes\":"
        << ivar_record.executable_ivar_layout_owner_size_bytes
        << ",\"executable_ivar_init_order_index\":"
        << ivar_record.executable_ivar_init_order_index
        << ",\"executable_ivar_destroy_order_index\":"
        << ivar_record.executable_ivar_destroy_order_index
        << ",\"executable_ivar_layout_valid\":"
        << (ivar_record.executable_ivar_layout_valid ? "true" : "false")
        << ",\"executable_ivar_layout_replay_key\":\""
        << EscapeJsonString(ivar_record.executable_ivar_layout_replay_key)
        << "\""
        << ",\"source_model\":\""
        << EscapeJsonString(ivar_record.source_model)
        << "\",\"line\":" << ivar_record.line
        << ",\"column\":" << ivar_record.column << "}";
    if (i + 1 != runtime_metadata_source_records.ivars_lexicographic.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "    ]\n"
      << "  }";
  return out.str();
}

std::string BuildRuntimeMetadataReferencesJson(
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  std::ostringstream out;
  out << "[\n";
  bool first_reference = true;
  auto emit_reference = [&](const std::string &reference_kind,
                            const std::string &owner_kind,
                            const std::string &owner_name,
                            const std::string &target_kind,
                            const std::string &target_name,
                            unsigned line,
                            unsigned column) {
    if (!first_reference) {
      out << ",\n";
    }
    first_reference = false;
    out << "    {\"reference_kind\":\"" << EscapeJsonString(reference_kind)
        << "\",\"owner_kind\":\"" << EscapeJsonString(owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(owner_name)
        << "\",\"target_kind\":\"" << EscapeJsonString(target_kind)
        << "\",\"target_name\":\"" << EscapeJsonString(target_name)
        << "\",\"line\":" << line << ",\"column\":" << column << "}";
  };
  for (const auto &class_record : runtime_metadata_source_records.classes_lexicographic) {
    if (class_record.has_super && !class_record.super_name.empty()) {
      emit_reference("class-superclass", class_record.record_kind,
                     class_record.name, "class", class_record.super_name,
                     class_record.line, class_record.column);
    }
    for (const auto &protocol_name : class_record.adopted_protocols_lexicographic) {
      emit_reference("class-adopted-protocol", class_record.record_kind,
                     class_record.name, "protocol", protocol_name,
                     class_record.line, class_record.column);
    }
  }
  for (const auto &protocol_record : runtime_metadata_source_records.protocols_lexicographic) {
    for (const auto &inherited_name : protocol_record.inherited_protocols_lexicographic) {
      emit_reference("protocol-inherited-protocol", "protocol",
                     protocol_record.name, "protocol", inherited_name,
                     protocol_record.line, protocol_record.column);
    }
  }
  for (const auto &category_record : runtime_metadata_source_records.categories_lexicographic) {
    for (const auto &protocol_name : category_record.adopted_protocols_lexicographic) {
      emit_reference("category-adopted-protocol", category_record.record_kind,
                     category_record.category_name, "protocol", protocol_name,
                     category_record.line, category_record.column);
    }
  }
  for (const auto &property_record : runtime_metadata_source_records.properties_lexicographic) {
    if (!property_record.effective_getter_selector.empty()) {
      emit_reference("property-getter-selector", property_record.owner_kind,
                     property_record.owner_name, "selector",
                     property_record.effective_getter_selector,
                     property_record.line, property_record.column);
    }
    if (property_record.effective_setter_available &&
        !property_record.effective_setter_selector.empty()) {
      emit_reference("property-setter-selector", property_record.owner_kind,
                     property_record.owner_name, "selector",
                     property_record.effective_setter_selector,
                     property_record.line, property_record.column);
    }
    if (!property_record.ivar_binding_symbol.empty()) {
      emit_reference("property-ivar-binding", property_record.owner_kind,
                     property_record.owner_name, "ivar-binding-symbol",
                     property_record.ivar_binding_symbol,
                     property_record.line, property_record.column);
    }
  }
  for (const auto &method_record : runtime_metadata_source_records.methods_lexicographic) {
    emit_reference("method-selector", method_record.owner_kind,
                   method_record.owner_name, "selector", method_record.selector,
                   method_record.line, method_record.column);
  }
  if (!first_reference) {
    out << "\n";
  }
  out << "  ]";
  return out.str();
}

std::string BuildSerializedRuntimeMetadataReusePayloadJson(
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary &summary,
    const std::string &module_name,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  std::ostringstream out;
  out << "{\n"
      << "    \"contract_id\": \"" << EscapeJsonString(summary.contract_id)
      << "\",\n"
      << "    \"module_name\": \"" << EscapeJsonString(module_name) << "\",\n"
      << "    \"reused_module_names_lexicographic\": "
      << BuildStringArrayJson(summary.reused_module_names_lexicographic)
      << ",\n"
      << "    \"runtime_owned_declaration_count\": "
      << summary.runtime_owned_declaration_count << ",\n"
      << "    \"metadata_reference_count\": " << summary.metadata_reference_count
      << ",\n"
      << "    \"runtime_owned_declarations\": "
      << BuildRuntimeOwnedDeclarationsJson(runtime_metadata_source_records)
      << ",\n"
      << "    \"metadata_references\": "
      << BuildRuntimeMetadataReferencesJson(runtime_metadata_source_records)
      << ",\n"
      << "    \"ready\": "
      << (IsReadyObjc3SerializedRuntimeMetadataArtifactReuseSummary(summary)
              ? "true"
              : "false")
      << ",\n"
      << "    \"replay_key\": \"" << EscapeJsonString(summary.replay_key)
      << "\"\n"
      << "  }";
  return out.str();
}

struct Objc3ConcurrencyActorMailboxRuntimeImportSummary {
  std::string contract_id = kObjc3ConcurrencyActorMailboxRuntimeImportContractId;
  std::string source_contract_id = kObjc3ConcurrencyActorLoweringMetadataContractId;
  std::string surface_path = kObjc3ConcurrencyActorMailboxRuntimeImportSurfacePath;
  std::string source_model = kObjc3ConcurrencyActorMailboxRuntimeImportSourceModel;
  std::string fail_closed_model =
      kObjc3ConcurrencyActorMailboxRuntimeImportFailClosedModel;
  bool actor_mailbox_runtime_ready = false;
  bool deterministic = false;
  std::string replay_key;
  std::string actor_lowering_replay_key;
  std::string actor_isolation_lowering_replay_key;
};

Objc3ConcurrencyActorMailboxRuntimeImportSummary
BuildConcurrencyActorMailboxRuntimeImportSummary(
    const Objc3ActorLoweringMetadataContract &actor_contract,
    const std::string &actor_lowering_replay_key,
    const std::string &actor_isolation_lowering_replay_key) {
  Objc3ConcurrencyActorMailboxRuntimeImportSummary summary;
  summary.actor_lowering_replay_key = actor_lowering_replay_key;
  summary.actor_isolation_lowering_replay_key =
      actor_isolation_lowering_replay_key;
  const bool actor_sites_present = actor_contract.actor_interface_sites != 0u ||
                                   actor_contract.actor_method_sites != 0u;
  summary.actor_mailbox_runtime_ready =
      actor_sites_present && IsValidObjc3ActorLoweringMetadataContract(actor_contract) &&
      !actor_lowering_replay_key.empty() &&
      !actor_isolation_lowering_replay_key.empty();
  summary.deterministic = actor_contract.deterministic;
  std::ostringstream replay_key;
  replay_key << summary.contract_id
             << ";source_contract_id=" << summary.source_contract_id
             << ";actor_mailbox_runtime_ready="
             << (summary.actor_mailbox_runtime_ready ? "true" : "false")
             << ";deterministic=" << (summary.deterministic ? "true" : "false")
             << ";actor_lowering_replay_key=" << actor_lowering_replay_key
             << ";actor_isolation_lowering_replay_key="
             << actor_isolation_lowering_replay_key;
  summary.replay_key = replay_key.str();
  return summary;
}

std::string BuildConcurrencyActorMailboxRuntimeImportSummaryJson(
    const Objc3ConcurrencyActorMailboxRuntimeImportSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"actor_mailbox_runtime_ready\":"
      << (summary.actor_mailbox_runtime_ready ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"actor_lowering_replay_key\":\""
      << EscapeJsonString(summary.actor_lowering_replay_key)
      << "\",\"actor_isolation_lowering_replay_key\":\""
      << EscapeJsonString(summary.actor_isolation_lowering_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

struct Objc3DispatchDispatchMetadataInterfacePreservationSurfaceSummary {
  std::string contract_id =
      kObjc3DispatchDispatchMetadataInterfacePreservationContractId;
  std::string source_contract_id = kObjc3DispatchDispatchControlLoweringContractId;
  std::string surface_path =
      kObjc3DispatchDispatchMetadataInterfacePreservationSurfacePath;
  std::string import_artifact_member_name =
      kObjc3DispatchDispatchMetadataInterfacePreservationImportArtifactMemberName;
  std::string source_model =
      kObjc3DispatchDispatchMetadataInterfacePreservationSourceModel;
  std::string preservation_model =
      kObjc3DispatchDispatchMetadataInterfacePreservationModel;
  std::string fail_closed_model =
      kObjc3DispatchDispatchMetadataInterfacePreservationFailClosedModel;
  std::string replay_key;
  std::string lowering_replay_key;
  std::size_t local_direct_callable_record_count = 0;
  std::size_t local_final_callable_record_count = 0;
  std::size_t local_final_container_record_count = 0;
  std::size_t local_sealed_container_record_count = 0;
  std::size_t imported_module_count = 0;
  std::size_t imported_direct_callable_record_count = 0;
  std::size_t imported_final_callable_record_count = 0;
  std::size_t imported_final_container_record_count = 0;
  std::size_t imported_sealed_container_record_count = 0;
  bool runtime_import_artifact_ready = false;
  bool separate_compilation_preservation_ready = false;
  bool deterministic = false;
};

std::size_t CountDirectCallableRuntimeMethodRecords(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  return static_cast<std::size_t>(std::count_if(
      records.methods_lexicographic.begin(), records.methods_lexicographic.end(),
      [](const Objc3RuntimeMetadataMethodSourceRecord &record) {
        return record.effective_direct_dispatch;
      }));
}

std::size_t CountFinalCallableRuntimeMethodRecords(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  return static_cast<std::size_t>(std::count_if(
      records.methods_lexicographic.begin(), records.methods_lexicographic.end(),
      [](const Objc3RuntimeMetadataMethodSourceRecord &record) {
        return record.objc_final_declared;
      }));
}

std::size_t CountFinalRuntimeClassRecords(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  return static_cast<std::size_t>(std::count_if(
      records.classes_lexicographic.begin(), records.classes_lexicographic.end(),
      [](const Objc3RuntimeMetadataClassSourceRecord &record) {
        return record.objc_final_declared;
      }));
}

std::size_t CountSealedRuntimeClassRecords(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  return static_cast<std::size_t>(std::count_if(
      records.classes_lexicographic.begin(), records.classes_lexicographic.end(),
      [](const Objc3RuntimeMetadataClassSourceRecord &record) {
        return record.objc_sealed_declared;
      }));
}

Objc3DispatchDispatchMetadataInterfacePreservationSurfaceSummary
BuildDispatchDispatchMetadataInterfacePreservationSummary(
    const Objc3RuntimeMetadataSourceRecordSet &local_runtime_metadata_source_records,
    const std::string &lowering_replay_key, bool runtime_import_artifact_ready,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces) {
  Objc3DispatchDispatchMetadataInterfacePreservationSurfaceSummary summary;
  summary.lowering_replay_key = lowering_replay_key;
  summary.local_direct_callable_record_count =
      CountDirectCallableRuntimeMethodRecords(local_runtime_metadata_source_records);
  summary.local_final_callable_record_count =
      CountFinalCallableRuntimeMethodRecords(local_runtime_metadata_source_records);
  summary.local_final_container_record_count =
      CountFinalRuntimeClassRecords(local_runtime_metadata_source_records);
  summary.local_sealed_container_record_count =
      CountSealedRuntimeClassRecords(local_runtime_metadata_source_records);
  summary.runtime_import_artifact_ready =
      runtime_import_artifact_ready &&
      IsReadyObjc3RuntimeMetadataSourceRecordSet(
          local_runtime_metadata_source_records);
  summary.deterministic = local_runtime_metadata_source_records.deterministic;
  for (const auto &surface : imported_runtime_module_surfaces) {
    if (!surface.dispatch_dispatch_metadata_interface_preservation_present) {
      continue;
    }
    ++summary.imported_module_count;
    summary.imported_direct_callable_record_count +=
        CountDirectCallableRuntimeMethodRecords(surface.runtime_metadata_source_records);
    summary.imported_final_callable_record_count +=
        CountFinalCallableRuntimeMethodRecords(surface.runtime_metadata_source_records);
    summary.imported_final_container_record_count +=
        CountFinalRuntimeClassRecords(surface.runtime_metadata_source_records);
    summary.imported_sealed_container_record_count +=
        CountSealedRuntimeClassRecords(surface.runtime_metadata_source_records);
    summary.deterministic = summary.deterministic && surface.dispatch_deterministic;
  }
  summary.separate_compilation_preservation_ready =
      summary.runtime_import_artifact_ready;
  std::ostringstream replay_key;
  replay_key << Objc3DispatchDispatchMetadataInterfacePreservationSummary()
             << ";runtime_import_artifact_ready="
             << (summary.runtime_import_artifact_ready ? "true" : "false")
             << ";separate_compilation_preservation_ready="
             << (summary.separate_compilation_preservation_ready ? "true"
                                                                 : "false")
             << ";imported_module_count=" << summary.imported_module_count
             << ";deterministic="
             << (summary.deterministic ? "true" : "false")
             << ";lowering_replay_key=" << lowering_replay_key
             << ";local_direct_callable_record_count="
             << summary.local_direct_callable_record_count
             << ";local_final_callable_record_count="
             << summary.local_final_callable_record_count
             << ";local_final_container_record_count="
             << summary.local_final_container_record_count
             << ";local_sealed_container_record_count="
             << summary.local_sealed_container_record_count
             << ";imported_direct_callable_record_count="
             << summary.imported_direct_callable_record_count
             << ";imported_final_callable_record_count="
             << summary.imported_final_callable_record_count
             << ";imported_final_container_record_count="
             << summary.imported_final_container_record_count
             << ";imported_sealed_container_record_count="
             << summary.imported_sealed_container_record_count;
  summary.replay_key = replay_key.str();
  return summary;
}

std::string BuildDispatchDispatchMetadataInterfacePreservationSummaryJson(
    const Objc3DispatchDispatchMetadataInterfacePreservationSurfaceSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"preservation_model\":\""
      << EscapeJsonString(summary.preservation_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"lowering_replay_key\":\""
      << EscapeJsonString(summary.lowering_replay_key)
      << "\",\"local_direct_callable_record_count\":"
      << summary.local_direct_callable_record_count
      << ",\"local_final_callable_record_count\":"
      << summary.local_final_callable_record_count
      << ",\"local_final_container_record_count\":"
      << summary.local_final_container_record_count
      << ",\"local_sealed_container_record_count\":"
      << summary.local_sealed_container_record_count
      << ",\"imported_module_count\":" << summary.imported_module_count
      << ",\"imported_direct_callable_record_count\":"
      << summary.imported_direct_callable_record_count
      << ",\"imported_final_callable_record_count\":"
      << summary.imported_final_callable_record_count
      << ",\"imported_final_container_record_count\":"
      << summary.imported_final_container_record_count
      << ",\"imported_sealed_container_record_count\":"
      << summary.imported_sealed_container_record_count
      << ",\"runtime_import_artifact_ready\":"
      << (summary.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_preservation_ready\":"
      << (summary.separate_compilation_preservation_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildRuntimeStorageReflectionArtifactPreservationSummaryJson(
    const Objc3RuntimeStorageReflectionArtifactPreservationSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"dispatch_and_synthesized_accessor_lowering_surface_contract_id\":\""
      << EscapeJsonString(
             summary.dispatch_and_synthesized_accessor_lowering_surface_contract_id)
      << "\",\"executable_property_accessor_layout_lowering_contract_id\":\""
      << EscapeJsonString(
             summary.executable_property_accessor_layout_lowering_contract_id)
      << "\",\"executable_ivar_layout_emission_contract_id\":\""
      << EscapeJsonString(
             summary.executable_ivar_layout_emission_contract_id)
      << "\",\"executable_synthesized_accessor_property_lowering_contract_id\":\""
      << EscapeJsonString(summary
                              .executable_synthesized_accessor_property_lowering_contract_id)
      << "\",\"surface_path\":\""
      << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"preservation_model\":\""
      << EscapeJsonString(summary.preservation_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"local_property_descriptor_count\":"
      << summary.local_property_descriptor_count
      << ",\"local_ivar_descriptor_count\":"
      << summary.local_ivar_descriptor_count
      << ",\"implementation_owned_property_entries\":"
      << summary.implementation_owned_property_entries
      << ",\"synthesized_accessor_owner_entries\":"
      << summary.synthesized_accessor_owner_entries
      << ",\"synthesized_getter_entries\":"
      << summary.synthesized_getter_entries
      << ",\"synthesized_setter_entries\":"
      << summary.synthesized_setter_entries
      << ",\"synthesized_accessor_entries\":"
      << summary.synthesized_accessor_entries
      << ",\"current_property_read_entries\":"
      << summary.current_property_read_entries
      << ",\"current_property_write_entries\":"
      << summary.current_property_write_entries
      << ",\"current_property_exchange_entries\":"
      << summary.current_property_exchange_entries
      << ",\"weak_current_property_load_entries\":"
      << summary.weak_current_property_load_entries
      << ",\"weak_current_property_store_entries\":"
      << summary.weak_current_property_store_entries
      << ",\"ivar_layout_entries\":" << summary.ivar_layout_entries
      << ",\"ivar_layout_owner_entries\":"
      << summary.ivar_layout_owner_entries
      << ",\"runtime_import_artifact_ready\":"
      << (summary.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_preservation_ready\":"
      << (summary.separate_compilation_preservation_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildRuntimeBlockOwnershipArtifactPreservationSummaryJson(
    const Objc3RuntimeBlockOwnershipArtifactPreservationSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"block_object_invoke_thunk_lowering_contract_id\":\""
      << EscapeJsonString(
             summary.block_object_invoke_thunk_lowering_contract_id)
      << "\",\"block_byref_helper_lowering_contract_id\":\""
      << EscapeJsonString(summary.block_byref_helper_lowering_contract_id)
      << "\",\"block_escape_runtime_hook_lowering_contract_id\":\""
      << EscapeJsonString(
             summary.block_escape_runtime_hook_lowering_contract_id)
      << "\",\"runtime_support_library_link_wiring_contract_id\":\""
      << EscapeJsonString(
             summary.runtime_support_library_link_wiring_contract_id)
      << "\",\"surface_path\":\""
      << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"preservation_model\":\""
      << EscapeJsonString(summary.preservation_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"local_block_literal_sites\":"
      << summary.local_block_literal_sites
      << ",\"local_invoke_trampoline_symbolized_sites\":"
      << summary.local_invoke_trampoline_symbolized_sites
      << ",\"local_copy_helper_required_sites\":"
      << summary.local_copy_helper_required_sites
      << ",\"local_dispose_helper_required_sites\":"
      << summary.local_dispose_helper_required_sites
      << ",\"local_copy_helper_symbolized_sites\":"
      << summary.local_copy_helper_symbolized_sites
      << ",\"local_dispose_helper_symbolized_sites\":"
      << summary.local_dispose_helper_symbolized_sites
      << ",\"local_escape_to_heap_sites\":"
      << summary.local_escape_to_heap_sites
      << ",\"local_byref_layout_symbolized_sites\":"
      << summary.local_byref_layout_symbolized_sites
      << ",\"runtime_import_artifact_ready\":"
      << (summary.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_preservation_ready\":"
      << (summary.separate_compilation_preservation_ready ? "true" : "false")
      << ",\"runtime_support_library_link_wiring_ready\":"
      << (summary.runtime_support_library_link_wiring_ready ? "true"
                                                            : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildRuntimeAwareImportModuleArtifactJson(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records,
    const std::string &type_system_optional_keypath_lowering_contract_json,
    const std::string &type_system_optional_keypath_runtime_helper_contract_json,
    const std::string &type_system_generic_contract_preservation_json,
    const std::string &type_system_nullability_contract_preservation_json,
    const std::string &type_system_protocol_contract_preservation_json,
    const std::string &error_handling_result_and_bridging_artifact_replay_json,
    const std::string &concurrency_actor_mailbox_runtime_import_json,
    const std::string &interop_foreign_surface_interface_preservation_json,
    const std::string &interop_header_module_bridge_generation_json,
    const std::string &interop_ffi_metadata_interface_preservation_json,
    const std::string &metaprogramming_module_interface_replay_preservation_json,
    const std::string &metaprogramming_macro_host_process_cache_runtime_integration_json,
    const std::string &dispatch_dispatch_metadata_interface_preservation_json,
    const std::string &runtime_block_ownership_artifact_preservation_json,
    const std::string &runtime_storage_reflection_artifact_preservation_json,
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary
        &serialized_runtime_metadata_artifact_reuse,
    const Objc3RuntimeMetadataSourceRecordSet
        &serialized_runtime_metadata_reuse_records) {
  std::ostringstream out;
  out << "{\n"
      << "  \"contract_id\": \"" << EscapeJsonString(summary.contract_id)
      << "\",\n"
      << "  \"source_surface_contract_id\": \""
      << EscapeJsonString(summary.source_surface_contract_id) << "\",\n"
      << "  \"frontend_surface_path\": \""
      << EscapeJsonString(summary.frontend_surface_path) << "\",\n"
      << "  \"artifact\": \""
      << EscapeJsonString(summary.artifact_relative_path) << "\",\n"
      << "  \"payload_model\": \""
      << EscapeJsonString(summary.payload_model) << "\",\n"
      << "  \"authority_model\": \""
      << EscapeJsonString(summary.authority_model) << "\",\n"
      << "  \"payload_ownership_model\": \""
      << EscapeJsonString(summary.payload_ownership_model) << "\",\n"
      << "  \"module_name\": \"" << EscapeJsonString(summary.module_name)
      << "\",\n"
      << "  \"protocol_decl_count\": " << summary.protocol_decl_count << ",\n"
      << "  \"interface_decl_count\": " << summary.interface_decl_count
      << ",\n"
      << "  \"implementation_decl_count\": "
      << summary.implementation_decl_count << ",\n"
      << "  \"interface_category_decl_count\": "
      << summary.interface_category_decl_count << ",\n"
      << "  \"implementation_category_decl_count\": "
      << summary.implementation_category_decl_count << ",\n"
      << "  \"function_decl_count\": " << summary.function_decl_count << ",\n"
      << "  \"module_import_graph_sites\": " << summary.module_import_graph_sites
      << ",\n"
      << "  \"import_edge_candidate_sites\": "
      << summary.import_edge_candidate_sites << ",\n"
      << "  \"namespace_segment_sites\": " << summary.namespace_segment_sites
      << ",\n"
      << "  \"object_pointer_type_sites\": "
      << summary.object_pointer_type_sites << ",\n"
      << "  \"pointer_declarator_sites\": "
      << summary.pointer_declarator_sites << ",\n"
      << "  \"normalized_sites\": " << summary.normalized_sites << ",\n"
      << "  \"contract_violation_sites\": "
      << summary.contract_violation_sites << ",\n"
      << "  \"runtime_owned_declaration_count\": "
      << summary.runtime_owned_declaration_count << ",\n"
      << "  \"metadata_reference_count\": "
      << summary.metadata_reference_count << ",\n"
      << "  \"runtime_aware_import_declarations_landed\": "
      << (summary.runtime_aware_import_declarations_landed ? "true" : "false")
      << ",\n"
      << "  \"module_metadata_import_surface_landed\": "
      << (summary.module_metadata_import_surface_landed ? "true" : "false")
      << ",\n"
      << "  \"runtime_owned_declaration_import_landed\": "
      << (summary.runtime_owned_declaration_import_landed ? "true" : "false")
      << ",\n"
      << "  \"runtime_metadata_reference_import_landed\": "
      << (summary.runtime_metadata_reference_import_landed ? "true" : "false")
      << ",\n"
      << "  \"public_frontend_api_module_surface_landed\": "
      << (summary.public_frontend_api_module_surface_landed ? "true" : "false")
      << ",\n"
      << "  \"ready_for_import_artifact_emission\": "
      << (summary.ready_for_import_artifact_emission ? "true" : "false")
      << ",\n"
      << "  \"ready_for_frontend_module_consumption\": "
      << (summary.ready_for_frontend_module_consumption ? "true" : "false")
      << ",\n"
      << "  \"runtime_owned_declarations\": "
      << BuildRuntimeOwnedDeclarationsJson(runtime_metadata_source_records)
      << ",\n"
      << "  \"metadata_references\": "
      << BuildRuntimeMetadataReferencesJson(runtime_metadata_source_records)
      << ",\n"
      << "  \"objc_type_system_optional_keypath_lowering_contract\": "
      << type_system_optional_keypath_lowering_contract_json << ",\n"
      << "  \"objc_type_system_optional_keypath_runtime_helper_contract\": "
      << type_system_optional_keypath_runtime_helper_contract_json << ",\n"
      << "  \"objc_type_system_generic_contract_preservation\": "
      << type_system_generic_contract_preservation_json << ",\n"
      << "  \"objc_type_system_nullability_contract_preservation\": "
      << type_system_nullability_contract_preservation_json << ",\n"
      << "  \"objc_type_system_protocol_contract_preservation\": "
      << type_system_protocol_contract_preservation_json << ",\n"
      << "  \"objc_error_handling_result_and_bridging_artifact_replay\": "
      << error_handling_result_and_bridging_artifact_replay_json << ",\n"
      << "  \"objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface\": "
      << concurrency_actor_mailbox_runtime_import_json << ",\n"
      << "  \"objc_interop_foreign_surface_interface_and_module_preservation\": "
      << interop_foreign_surface_interface_preservation_json << ",\n"
      << "  \"objc_interop_header_module_and_bridge_generation\": "
      << interop_header_module_bridge_generation_json << ",\n"
      << "  \"objc_interop_ffi_metadata_and_interface_preservation\": "
      << interop_ffi_metadata_interface_preservation_json << ",\n"
      << "  \"objc_metaprogramming_module_interface_and_replay_preservation\": "
      << metaprogramming_module_interface_replay_preservation_json << ",\n"
      << "  \"objc_metaprogramming_macro_host_process_and_cache_runtime_integration\": "
      << metaprogramming_macro_host_process_cache_runtime_integration_json << ",\n"
      << "  \"objc_dispatch_dispatch_metadata_and_interface_preservation\": "
      << dispatch_dispatch_metadata_interface_preservation_json << ",\n"
      << "  \"objc_runtime_block_ownership_artifact_preservation\": "
      << runtime_block_ownership_artifact_preservation_json << ",\n"
      << "  \"objc_runtime_storage_reflection_artifact_preservation\": "
      << runtime_storage_reflection_artifact_preservation_json << ",\n"
      << "  \""
      << kObjc3SerializedRuntimeMetadataArtifactReusePayloadMemberName
      << "\": "
      << BuildSerializedRuntimeMetadataReusePayloadJson(
             serialized_runtime_metadata_artifact_reuse, summary.module_name,
             serialized_runtime_metadata_reuse_records)
      << ",\n"
      << "  \"source_surface_replay_key\": \""
      << EscapeJsonString(summary.source_surface_replay_key) << "\",\n"
      << "  \"replay_key\": \"" << EscapeJsonString(summary.replay_key)
      << "\"\n"
      << "}\n";
  return out.str();
}

std::string BuildFeatureClaimStrictnessTruthSurfaceReplayKey(
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result) {
  std::ostringstream out;
  out << kObjc3FeatureClaimStrictnessTruthSurfaceContractId
      << ";language_mode=" << kObjc3RunnableFeatureClaimModeName
      << ";language_version=" << static_cast<unsigned>(options.language_version)
      << ";language_profile=" << LanguageProfileName(options.language_profile)
      << ";supported_selection_surfaces=3"
      << ";unsupported_selection_surfaces=2"
      << ";suppressed_macro_claims=3"
      << ";parser_declared_protocols=" << pipeline_result.program.ast.protocols.size()
      << ";parser_declared_interfaces=" << pipeline_result.program.ast.interfaces.size()
      << ";parser_declared_implementations=" << pipeline_result.program.ast.implementations.size();
  return out.str();
}

std::string BuildFeatureClaimStrictnessTruthSurfaceJson(
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result) {
  const std::vector<std::string> supported_selection_surface_ids =
      BuildSupportedSelectionSurfaceIds();
  const std::vector<std::string> unsupported_selection_surface_ids =
      BuildUnsupportedSelectionSurfaceIds();
  const std::vector<std::string> suppressed_macro_claim_ids =
      BuildSuppressedMacroClaimIds();
  const std::vector<std::string> supported_language_profiles = {
      "canonical",
  };
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << kObjc3FeatureClaimStrictnessTruthSurfaceContractId
      << "\",\"runnable_feature_claim_inventory_contract_id\":\""
      << kObjc3RunnableFeatureClaimInventoryContractId
      << "\",\"language_mode\":\"" << kObjc3RunnableFeatureClaimModeName
      << "\",\"language_version\":" << static_cast<unsigned>(options.language_version)
      << ",\"effective_language_profile\":\""
      << LanguageProfileName(options.language_profile)
      << "\",\"default_language_profile\":\"canonical\""
      << ",\"canonical_literal_rejection_diagnostics_enabled\":"
      << "true"
      << ",\"driver_surface_model\":\""
      << kObjc3FeatureClaimStrictnessTruthDriverSurfaceModel
      << "\",\"language_version_selection_supported\":true"
      << ",\"compatibility_selection_supported\":true"
      << ",\"canonical_literal_rejection_diagnostics_hard_error\":true"
      << ",\"strictness_selection_supported\":false"
      << ",\"strict_concurrency_selection_supported\":false"
      << ",\"feature_macro_surface_supported\":false"
      << ",\"claim_truth_fail_closed\":true"
      << ",\"supported_language_profiles\":"
      << BuildStringArrayJson(supported_language_profiles)
      << ",\"supported_selection_surface_ids\":"
      << BuildStringArrayJson(supported_selection_surface_ids)
      << ",\"unsupported_selection_surface_ids\":"
      << BuildStringArrayJson(unsupported_selection_surface_ids)
      << ",\"suppressed_macro_claim_ids\":"
      << BuildStringArrayJson(suppressed_macro_claim_ids)
      << ",\"declared_protocol_count\":"
      << pipeline_result.program.ast.protocols.size()
      << ",\"declared_interface_count\":"
      << pipeline_result.program.ast.interfaces.size()
      << ",\"declared_implementation_count\":"
      << pipeline_result.program.ast.implementations.size()
      << ",\"replay_key\":\""
      << EscapeJsonString(BuildFeatureClaimStrictnessTruthSurfaceReplayKey(
             options, pipeline_result))
      << "\"}";
  return out.str();
}

std::string BuildFrontendCompatibilityStrictnessClaimSemanticsReplayKey(
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";inventory_contract_id="
      << summary.runnable_feature_claim_inventory_contract_id
      << ";truth_surface_contract_id="
      << summary.feature_claim_truth_surface_contract_id
      << ";frontend_surface_path=" << summary.frontend_surface_path
      << ";semantic_model=" << summary.semantic_model
      << ";downgrade_model=" << summary.downgrade_model
      << ";rejection_model=" << summary.rejection_model
      << ";canonical_interface_truth_model="
      << summary.canonical_interface_truth_model
      << ";separate_compilation_macro_truth_model="
      << summary.separate_compilation_macro_truth_model
      << ";canonical_interface_payload_mode="
      << summary.canonical_interface_payload_mode
      << ";language_profile=" << summary.effective_language_profile
      << ";canonical_literal_rejection_diagnostics="
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true" : "false")
      << ";live_unsupported_feature_families="
      << summary.live_unsupported_feature_family_count
      << ";live_unsupported_feature_sites="
      << summary.live_unsupported_feature_site_count
      << ";live_unsupported_feature_diagnostics="
      << summary.live_unsupported_feature_diagnostic_count
      << ";throws_source_rejections="
      << summary.throws_source_rejection_site_count
      << ";blocks_source_rejections="
      << summary.blocks_source_rejection_site_count
      << ";arc_source_rejections="
      << summary.arc_source_rejection_site_count
      << ";suppressed_macro_claim_ids=";
  for (std::size_t index = 0; index < summary.suppressed_macro_claim_ids.size();
       ++index) {
    if (index > 0u) {
      out << ",";
    }
    out << summary.suppressed_macro_claim_ids[index];
  }
  out << ";semantic_boundary_replay_key="
      << summary.semantic_boundary_replay_key;
  return out.str();
}

Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
BuildFrontendCompatibilityStrictnessClaimSemanticsSummary(
    const Objc3CompatibilityStrictnessClaimSemanticsSummary
        &semantic_boundary) {
  Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary summary;
  summary.semantic_boundary_ready =
      IsReadyObjc3CompatibilityStrictnessClaimSemanticsSummary(
          semantic_boundary);
  if (summary.semantic_boundary_ready) {
    summary.effective_language_profile =
        semantic_boundary.effective_language_profile;
    summary.canonical_literal_rejection_diagnostics_enabled = true;
    summary.valid_language_profile_count =
        semantic_boundary.valid_language_profile_count;
    summary.live_selection_surface_count =
        semantic_boundary.live_selection_surface_count;
    summary.valid_selection_combination_count =
        semantic_boundary.valid_selection_combination_count;
    summary.runnable_feature_claim_count =
        semantic_boundary.runnable_feature_claim_count;
    summary.downgraded_source_only_claim_count =
        semantic_boundary.downgraded_source_only_claim_count;
    summary.rejected_unsupported_feature_claim_count =
        semantic_boundary.rejected_unsupported_feature_claim_count;
    summary.rejected_selection_surface_count =
        semantic_boundary.rejected_selection_surface_count;
    summary.suppressed_macro_claim_count =
        semantic_boundary.suppressed_macro_claim_count;
    // unsupported-feature enforcement anchor: preserve the
    // semantic boundary counters verbatim so positive runnable proofs and
    // fail-closed advanced-source rejections remain replay-stable.
    summary.live_unsupported_feature_family_count =
        semantic_boundary.live_unsupported_feature_family_count;
    summary.live_unsupported_feature_site_count =
        semantic_boundary.live_unsupported_feature_site_count;
    summary.live_unsupported_feature_diagnostic_count =
        semantic_boundary.live_unsupported_feature_diagnostic_count;
    summary.throws_source_rejection_site_count =
        semantic_boundary.throws_source_rejection_site_count;
    summary.blocks_source_rejection_site_count =
        semantic_boundary.blocks_source_rejection_site_count;
    summary.arc_source_rejection_site_count =
        semantic_boundary.arc_source_rejection_site_count;
    summary.canonical_interface_truth_model =
        semantic_boundary.canonical_interface_truth_model;
    summary.separate_compilation_macro_truth_model =
        semantic_boundary.separate_compilation_macro_truth_model;
    summary.canonical_interface_payload_mode =
        semantic_boundary.canonical_interface_payload_mode;
    summary.suppressed_macro_claim_ids =
        semantic_boundary.suppressed_macro_claim_ids;
    summary.fail_closed = semantic_boundary.fail_closed;
    summary.language_profile_semantics_landed =
        semantic_boundary.language_profile_semantics_landed;
    summary.canonical_literal_rejection_semantics_landed =
        semantic_boundary.canonical_literal_rejection_semantics_landed;
    summary.source_only_claim_downgrade_semantics_landed =
        semantic_boundary.source_only_claim_downgrade_semantics_landed;
    summary.unsupported_feature_claim_rejection_semantics_landed =
        semantic_boundary.unsupported_feature_claim_rejection_semantics_landed;
    summary.live_unsupported_feature_source_rejection_landed =
        semantic_boundary.live_unsupported_feature_source_rejection_landed;
    summary.strictness_selection_rejection_semantics_landed =
        semantic_boundary.strictness_selection_rejection_semantics_landed;
    summary.feature_macro_claim_suppression_semantics_landed =
        semantic_boundary.feature_macro_claim_suppression_semantics_landed;
    summary.canonical_interface_truth_semantics_landed =
        semantic_boundary.canonical_interface_truth_semantics_landed;
    summary.separate_compilation_macro_truth_semantics_landed =
        semantic_boundary.separate_compilation_macro_truth_semantics_landed;
    summary.selected_configuration_valid =
        semantic_boundary.selected_configuration_valid;
    summary.selected_configuration_downgraded =
        semantic_boundary.selected_configuration_downgraded;
    summary.selected_configuration_rejected =
        semantic_boundary.selected_configuration_rejected;
    summary.ready_for_lowering_and_runtime =
        semantic_boundary.ready_for_lowering_and_runtime;
    summary.semantic_boundary_replay_key = semantic_boundary.replay_key;
    summary.replay_key =
        BuildFrontendCompatibilityStrictnessClaimSemanticsReplayKey(summary);
  }
  if (!IsReadyObjc3FrontendCompatibilityStrictnessClaimSemanticsSummary(
          summary)) {
    summary.failure_reason =
        "frontend compatibility/strictness/claim semantics summary is incomplete";
  }
  return summary;
}

std::string BuildFrontendCompatibilityStrictnessClaimSemanticsSummaryJson(
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"runnable_feature_claim_inventory_contract_id\":\""
      << EscapeJsonString(summary.runnable_feature_claim_inventory_contract_id)
      << "\",\"feature_claim_truth_surface_contract_id\":\""
      << EscapeJsonString(summary.feature_claim_truth_surface_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"downgrade_model\":\""
      << EscapeJsonString(summary.downgrade_model)
      << "\",\"rejection_model\":\""
      << EscapeJsonString(summary.rejection_model)
      << "\",\"canonical_interface_truth_model\":\""
      << EscapeJsonString(summary.canonical_interface_truth_model)
      << "\",\"separate_compilation_macro_truth_model\":\""
      << EscapeJsonString(summary.separate_compilation_macro_truth_model)
      << "\",\"canonical_interface_payload_mode\":\""
      << EscapeJsonString(summary.canonical_interface_payload_mode)
      << "\",\"effective_language_profile\":\""
      << EscapeJsonString(summary.effective_language_profile)
      << "\",\"canonical_literal_rejection_diagnostics_enabled\":"
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true" : "false")
      << ",\"valid_language_profile_count\":"
      << summary.valid_language_profile_count
      << ",\"live_selection_surface_count\":"
      << summary.live_selection_surface_count
      << ",\"valid_selection_combination_count\":"
      << summary.valid_selection_combination_count
      << ",\"runnable_feature_claim_count\":"
      << summary.runnable_feature_claim_count
      << ",\"downgraded_source_only_claim_count\":"
      << summary.downgraded_source_only_claim_count
      << ",\"rejected_unsupported_feature_claim_count\":"
      << summary.rejected_unsupported_feature_claim_count
      << ",\"live_unsupported_feature_family_count\":"
      << summary.live_unsupported_feature_family_count
      << ",\"live_unsupported_feature_site_count\":"
      << summary.live_unsupported_feature_site_count
      << ",\"live_unsupported_feature_diagnostic_count\":"
      << summary.live_unsupported_feature_diagnostic_count
      << ",\"throws_source_rejection_site_count\":"
      << summary.throws_source_rejection_site_count
      << ",\"blocks_source_rejection_site_count\":"
      << summary.blocks_source_rejection_site_count
      << ",\"arc_source_rejection_site_count\":"
      << summary.arc_source_rejection_site_count
      << ",\"rejected_selection_surface_count\":"
      << summary.rejected_selection_surface_count
      << ",\"suppressed_macro_claim_count\":"
      << summary.suppressed_macro_claim_count
      << ",\"suppressed_macro_claim_ids\":"
      << BuildStringArrayJson(summary.suppressed_macro_claim_ids)
      << ",\"fail_closed\":"
      << (summary.fail_closed ? "true" : "false")
      << ",\"semantic_boundary_ready\":"
      << (summary.semantic_boundary_ready ? "true" : "false")
      << ",\"language_profile_semantics_landed\":"
      << (summary.language_profile_semantics_landed ? "true" : "false")
      << ",\"canonical_literal_rejection_semantics_landed\":"
      << (summary.canonical_literal_rejection_semantics_landed ? "true" : "false")
      << ",\"source_only_claim_downgrade_semantics_landed\":"
      << (summary.source_only_claim_downgrade_semantics_landed ? "true"
                                                               : "false")
      << ",\"unsupported_feature_claim_rejection_semantics_landed\":"
      << (summary.unsupported_feature_claim_rejection_semantics_landed ? "true"
                                                                       : "false")
      << ",\"live_unsupported_feature_source_rejection_landed\":"
      << (summary.live_unsupported_feature_source_rejection_landed ? "true"
                                                                   : "false")
      << ",\"strictness_selection_rejection_semantics_landed\":"
      << (summary.strictness_selection_rejection_semantics_landed ? "true"
                                                                  : "false")
      << ",\"feature_macro_claim_suppression_semantics_landed\":"
      << (summary.feature_macro_claim_suppression_semantics_landed ? "true"
                                                                   : "false")
      << ",\"canonical_interface_truth_semantics_landed\":"
      << (summary.canonical_interface_truth_semantics_landed ? "true"
                                                             : "false")
      << ",\"separate_compilation_macro_truth_semantics_landed\":"
      << (summary.separate_compilation_macro_truth_semantics_landed ? "true"
                                                                    : "false")
      << ",\"selected_configuration_valid\":"
      << (summary.selected_configuration_valid ? "true" : "false")
      << ",\"selected_configuration_downgraded\":"
      << (summary.selected_configuration_downgraded ? "true" : "false")
      << ",\"selected_configuration_rejected\":"
      << (summary.selected_configuration_rejected ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"semantic_boundary_replay_key\":\""
      << EscapeJsonString(summary.semantic_boundary_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"ready\":"
      << (IsReadyObjc3FrontendCompatibilityStrictnessClaimSemanticsSummary(
              summary)
              ? "true"
              : "false")
      << "}";
  return out.str();
}

std::string BuildObjc3TranslationUnitIdentityKey(
    const std::filesystem::path &input_path,
    const Objc3ParseLoweringReadinessSurface &parse_lowering_readiness_surface) {
  return input_path.generic_string() + "|" +
         parse_lowering_readiness_surface.parse_artifact_replay_key + "|" +
         parse_lowering_readiness_surface.lowering_boundary_replay_key;
}

std::string BuildExecutableMetadataSourceGraphJson(
    const Objc3ExecutableMetadataSourceGraph &graph) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(graph.contract_id)
      << "\",\"owner_identity_model\":\""
      << EscapeJsonString(graph.owner_identity_model)
      << "\",\"metaclass_node_policy\":\""
      << EscapeJsonString(graph.metaclass_node_policy)
      << "\",\"edge_ordering_model\":\""
      << EscapeJsonString(graph.edge_ordering_model)
      << "\",\"class_metaclass_source_closure_contract_id\":\""
      << EscapeJsonString(graph.class_metaclass_source_closure_contract_id)
      << "\",\"class_metaclass_parent_identity_model\":\""
      << EscapeJsonString(graph.class_metaclass_parent_identity_model)
      << "\",\"class_metaclass_method_owner_identity_model\":\""
      << EscapeJsonString(graph.class_metaclass_method_owner_identity_model)
      << "\",\"class_metaclass_object_identity_model\":\""
      << EscapeJsonString(graph.class_metaclass_object_identity_model)
      << "\",\"protocol_category_source_closure_contract_id\":\""
      << EscapeJsonString(graph.protocol_category_source_closure_contract_id)
      << "\",\"protocol_inheritance_identity_model\":\""
      << EscapeJsonString(graph.protocol_inheritance_identity_model)
      << "\",\"category_attachment_identity_model\":\""
      << EscapeJsonString(graph.category_attachment_identity_model)
      << "\",\"protocol_category_conformance_identity_model\":\""
      << EscapeJsonString(graph.protocol_category_conformance_identity_model)
      << "\",\"interface_nodes\":"
      << graph.interface_nodes_lexicographic.size()
      << ",\"implementation_nodes\":"
      << graph.implementation_nodes_lexicographic.size()
      << ",\"class_nodes\":" << graph.class_nodes_lexicographic.size()
      << ",\"metaclass_nodes\":"
      << graph.metaclass_nodes_lexicographic.size()
      << ",\"protocol_nodes\":" << graph.protocol_nodes_lexicographic.size()
      << ",\"category_nodes\":" << graph.category_nodes_lexicographic.size()
      << ",\"property_nodes\":" << graph.property_nodes_lexicographic.size()
      << ",\"ivar_nodes\":" << graph.ivar_nodes_lexicographic.size()
      << ",\"method_nodes\":" << graph.method_nodes_lexicographic.size()
      << ",\"class_metaclass_declaration_closure_complete\":"
      << (graph.class_metaclass_declaration_closure_complete ? "true" : "false")
      << ",\"class_metaclass_parent_identity_closure_complete\":"
      << (graph.class_metaclass_parent_identity_closure_complete ? "true"
                                                                 : "false")
      << ",\"class_metaclass_method_owner_identity_closure_complete\":"
      << (graph.class_metaclass_method_owner_identity_closure_complete ? "true"
                                                                       : "false")
      << ",\"class_metaclass_object_identity_closure_complete\":"
      << (graph.class_metaclass_object_identity_closure_complete ? "true"
                                                                 : "false")
      << ",\"protocol_category_declaration_closure_complete\":"
