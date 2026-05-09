      summary.category_record_count + summary.property_record_count +
      summary.method_record_count + summary.ivar_record_count;
  for (const auto &class_record : runtime_metadata_source_records.classes_lexicographic) {
    if (class_record.has_super && !class_record.super_name.empty()) {
      ++summary.superclass_reference_count;
    }
  }
  summary.protocol_reference_count =
      CountRuntimeAwareImportProtocolReferences(runtime_metadata_source_records);
  summary.property_accessor_reference_count =
      CountRuntimeAwareImportPropertyAccessorReferences(
          runtime_metadata_source_records);
  summary.property_ivar_binding_reference_count =
      CountRuntimeAwareImportPropertyIvarBindingReferences(
          runtime_metadata_source_records);
  summary.method_selector_reference_count =
      runtime_metadata_source_records.methods_lexicographic.size();
  summary.metadata_reference_count =
      summary.superclass_reference_count + summary.protocol_reference_count +
      summary.property_accessor_reference_count +
      summary.property_ivar_binding_reference_count +
      summary.method_selector_reference_count;
  summary.ready_for_import_artifact_emission =
      summary.source_surface_contract_ready &&
      summary.runtime_metadata_source_records_ready;
  summary.ready_for_frontend_module_consumption =
      summary.ready_for_import_artifact_emission;
  summary.source_surface_replay_key =
      BuildRuntimeAwareImportModuleSurfaceReplayKey(
          program,
          parser_contract_snapshot,
          module_import_graph_lowering_contract);
  if (summary.ready_for_frontend_module_consumption) {
    summary.replay_key =
        BuildRuntimeAwareImportModuleFrontendClosureReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(summary)) {
    summary.failure_reason =
        "runtime-aware import/module frontend closure summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeAwareImportModuleFrontendClosureSummaryJson(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.StringField("contract_id", summary.contract_id);
  object.StringField("source_surface_contract_id",
                     summary.source_surface_contract_id);
  object.StringField("frontend_surface_path", summary.frontend_surface_path);
  object.StringField("payload_model", summary.payload_model);
  object.StringField("artifact_relative_path", summary.artifact_relative_path);
  object.StringField("authority_model", summary.authority_model);
  object.StringField("payload_ownership_model",
                     summary.payload_ownership_model);
  object.StringField("module_name", summary.module_name);
  object.SizeField("protocol_decl_count", summary.protocol_decl_count);
  object.SizeField("interface_decl_count", summary.interface_decl_count);
  object.SizeField("implementation_decl_count",
                   summary.implementation_decl_count);
  object.SizeField("interface_category_decl_count",
                   summary.interface_category_decl_count);
  object.SizeField("implementation_category_decl_count",
                   summary.implementation_category_decl_count);
  object.SizeField("function_decl_count", summary.function_decl_count);
  object.SizeField("module_import_graph_sites",
                   summary.module_import_graph_sites);
  object.SizeField("import_edge_candidate_sites",
                   summary.import_edge_candidate_sites);
  object.SizeField("namespace_segment_sites", summary.namespace_segment_sites);
  object.SizeField("object_pointer_type_sites",
                   summary.object_pointer_type_sites);
  object.SizeField("pointer_declarator_sites",
                   summary.pointer_declarator_sites);
  object.SizeField("normalized_sites", summary.normalized_sites);
  object.SizeField("contract_violation_sites",
                   summary.contract_violation_sites);
  object.SizeField("class_record_count", summary.class_record_count);
  object.SizeField("protocol_record_count", summary.protocol_record_count);
  object.SizeField("category_record_count", summary.category_record_count);
  object.SizeField("property_record_count", summary.property_record_count);
  object.SizeField("method_record_count", summary.method_record_count);
  object.SizeField("ivar_record_count", summary.ivar_record_count);
  object.SizeField("runtime_owned_declaration_count",
                   summary.runtime_owned_declaration_count);
  object.SizeField("superclass_reference_count",
                   summary.superclass_reference_count);
  object.SizeField("protocol_reference_count",
                   summary.protocol_reference_count);
  object.SizeField("property_accessor_reference_count",
                   summary.property_accessor_reference_count);
  object.SizeField("property_ivar_binding_reference_count",
                   summary.property_ivar_binding_reference_count);
  object.SizeField("method_selector_reference_count",
                   summary.method_selector_reference_count);
  object.SizeField("metadata_reference_count", summary.metadata_reference_count);
  object.BoolField("ready",
                   IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
                       summary));
  object.BoolField("fail_closed", summary.fail_closed);
  object.BoolField("source_surface_contract_ready",
                   summary.source_surface_contract_ready);
  object.BoolField("runtime_metadata_source_records_ready",
                   summary.runtime_metadata_source_records_ready);
  object.BoolField("frontend_surface_published",
                   summary.frontend_surface_published);
  object.BoolField("import_artifact_template_published",
                   summary.import_artifact_template_published);
  object.BoolField("runtime_aware_import_declarations_landed",
                   summary.runtime_aware_import_declarations_landed);
  object.BoolField("module_metadata_import_surface_landed",
                   summary.module_metadata_import_surface_landed);
  object.BoolField("runtime_owned_declaration_import_landed",
                   summary.runtime_owned_declaration_import_landed);
  object.BoolField("runtime_metadata_reference_import_landed",
                   summary.runtime_metadata_reference_import_landed);
  object.BoolField("public_frontend_api_module_surface_landed",
                   summary.public_frontend_api_module_surface_landed);
  object.BoolField("ready_for_import_artifact_emission",
                   summary.ready_for_import_artifact_emission);
  object.BoolField("ready_for_frontend_module_consumption",
                   summary.ready_for_frontend_module_consumption);
  object.StringField("source_surface_replay_key",
                     summary.source_surface_replay_key);
  object.StringField("replay_key", summary.replay_key);
  object.StringField("failure_reason", summary.failure_reason);
  object.End();
  return out.str();
}

std::string BuildCrossModuleRuntimeMetadataSemanticPreservationReplayKey(
    const Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_frontend_closure_contract_id="
      << summary.source_frontend_closure_contract_id
      << ";frontend_surface_path=" << summary.frontend_surface_path
      << ";source_artifact_relative_path="
      << summary.source_artifact_relative_path
      << ";authority_model=" << summary.authority_model
      << ";conformance_shape_model=" << summary.conformance_shape_model
      << ";dispatch_trait_model=" << summary.dispatch_trait_model
      << ";effect_trait_model=" << summary.effect_trait_model
      << ";module_name=" << summary.module_name
      << ";class_record_count=" << summary.class_record_count
      << ";protocol_record_count=" << summary.protocol_record_count
      << ";category_record_count=" << summary.category_record_count
      << ";property_record_count=" << summary.property_record_count
      << ";method_record_count=" << summary.method_record_count
      << ";ivar_record_count=" << summary.ivar_record_count
      << ";runtime_owned_declaration_count="
      << summary.runtime_owned_declaration_count
      << ";superclass_edge_count=" << summary.superclass_edge_count
      << ";protocol_conformance_edge_count="
      << summary.protocol_conformance_edge_count
      << ";category_attachment_count=" << summary.category_attachment_count
      << ";property_accessor_trait_count="
      << summary.property_accessor_trait_count
      << ";property_ivar_binding_trait_count="
      << summary.property_ivar_binding_trait_count
      << ";method_selector_trait_count="
      << summary.method_selector_trait_count
      << ";class_method_trait_count=" << summary.class_method_trait_count
      << ";instance_method_trait_count="
      << summary.instance_method_trait_count
      << ";implemented_method_count=" << summary.implemented_method_count
      << ";declaration_only_method_count="
      << summary.declaration_only_method_count
      << ";property_attribute_profile_count="
      << summary.property_attribute_profile_count
      << ";ownership_effect_profile_count="
      << summary.ownership_effect_profile_count
      << ";executable_binding_trait_count="
      << summary.executable_binding_trait_count
      << ";source_frontend_closure_replay_key="
      << summary.source_frontend_closure_replay_key;
  return out.str();
}

Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary
BuildCrossModuleRuntimeMetadataSemanticPreservationSummary(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary
        &source_frontend_closure,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary summary;
  summary.fail_closed = true;
  summary.source_frontend_closure_ready =
      IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
          source_frontend_closure);
  summary.runtime_metadata_source_records_ready =
      IsReadyObjc3RuntimeMetadataSourceRecordSet(runtime_metadata_source_records);
  summary.semantic_surface_published = true;
  summary.module_name = source_frontend_closure.module_name;
  summary.class_record_count =
      runtime_metadata_source_records.classes_lexicographic.size();
  summary.protocol_record_count =
      runtime_metadata_source_records.protocols_lexicographic.size();
  summary.category_record_count =
      runtime_metadata_source_records.categories_lexicographic.size();
  summary.property_record_count =
      runtime_metadata_source_records.properties_lexicographic.size();
  summary.method_record_count =
      runtime_metadata_source_records.methods_lexicographic.size();
  summary.ivar_record_count =
      runtime_metadata_source_records.ivars_lexicographic.size();
  summary.runtime_owned_declaration_count =
      summary.class_record_count + summary.protocol_record_count +
      summary.category_record_count + summary.property_record_count +
      summary.method_record_count + summary.ivar_record_count;
  for (const auto &class_record :
       runtime_metadata_source_records.classes_lexicographic) {
    if (class_record.has_super && !class_record.super_name.empty()) {
      ++summary.superclass_edge_count;
    }
    summary.protocol_conformance_edge_count +=
        class_record.adopted_protocols_lexicographic.size();
  }
  for (const auto &protocol_record :
       runtime_metadata_source_records.protocols_lexicographic) {
    summary.protocol_conformance_edge_count +=
        protocol_record.inherited_protocols_lexicographic.size();
  }
  summary.category_attachment_count =
      runtime_metadata_source_records.categories_lexicographic.size();
  for (const auto &category_record :
       runtime_metadata_source_records.categories_lexicographic) {
    summary.protocol_conformance_edge_count +=
        category_record.adopted_protocols_lexicographic.size();
  }
  for (const auto &property_record :
       runtime_metadata_source_records.properties_lexicographic) {
    if (!property_record.effective_getter_selector.empty()) {
      ++summary.property_accessor_trait_count;
    }
    if (property_record.effective_setter_available &&
        !property_record.effective_setter_selector.empty()) {
      ++summary.property_accessor_trait_count;
    }
    if (!property_record.ivar_binding_symbol.empty()) {
      ++summary.property_ivar_binding_trait_count;
    }
    if (!property_record.property_attribute_profile.empty()) {
      ++summary.property_attribute_profile_count;
    }
    if (!property_record.ownership_lifetime_profile.empty() ||
        !property_record.ownership_runtime_hook_profile.empty() ||
        !property_record.accessor_ownership_profile.empty()) {
      ++summary.ownership_effect_profile_count;
    }
    if (property_record.executable_synthesized_binding_kind != "none" &&
        !property_record.executable_synthesized_binding_kind.empty()) {
      ++summary.executable_binding_trait_count;
    }
  }
  for (const auto &method_record :
       runtime_metadata_source_records.methods_lexicographic) {
    if (!method_record.selector.empty()) {
      ++summary.method_selector_trait_count;
    }
    if (method_record.is_class_method) {
      ++summary.class_method_trait_count;
    } else {
      ++summary.instance_method_trait_count;
    }
    if (method_record.has_body) {
      ++summary.implemented_method_count;
    } else {
      ++summary.declaration_only_method_count;
    }
  }
  for (const auto &ivar_record : runtime_metadata_source_records.ivars_lexicographic) {
    if ((ivar_record.executable_synthesized_binding_kind != "none" &&
         !ivar_record.executable_synthesized_binding_kind.empty()) ||
        !ivar_record.executable_ivar_layout_symbol.empty()) {
      ++summary.executable_binding_trait_count;
    }
  }
  summary.source_frontend_closure_replay_key =
      source_frontend_closure.replay_key;
  if (summary.source_frontend_closure_ready &&
      summary.runtime_metadata_source_records_ready) {
    summary.replay_key =
        BuildCrossModuleRuntimeMetadataSemanticPreservationReplayKey(summary);
  }
  if (!IsReadyObjc3CrossModuleRuntimeMetadataSemanticPreservationSummary(
          summary)) {
    summary.failure_reason =
        "cross-module runtime metadata semantic preservation summary is incomplete";
  }
  return summary;
}

std::string BuildCrossModuleRuntimeMetadataSemanticPreservationSummaryJson(
    const Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary &summary) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.StringField("contract_id", summary.contract_id);
  object.StringField("source_frontend_closure_contract_id",
                     summary.source_frontend_closure_contract_id);
  object.StringField("frontend_surface_path", summary.frontend_surface_path);
  object.StringField("source_artifact_relative_path",
                     summary.source_artifact_relative_path);
  object.StringField("authority_model", summary.authority_model);
  object.StringField("conformance_shape_model",
                     summary.conformance_shape_model);
  object.StringField("dispatch_trait_model", summary.dispatch_trait_model);
  object.StringField("effect_trait_model", summary.effect_trait_model);
  object.StringField("module_name", summary.module_name);
  object.SizeField("class_record_count", summary.class_record_count);
  object.SizeField("protocol_record_count", summary.protocol_record_count);
  object.SizeField("category_record_count", summary.category_record_count);
  object.SizeField("property_record_count", summary.property_record_count);
  object.SizeField("method_record_count", summary.method_record_count);
  object.SizeField("ivar_record_count", summary.ivar_record_count);
  object.SizeField("runtime_owned_declaration_count",
                   summary.runtime_owned_declaration_count);
  object.SizeField("superclass_edge_count", summary.superclass_edge_count);
  object.SizeField("protocol_conformance_edge_count",
                   summary.protocol_conformance_edge_count);
  object.SizeField("category_attachment_count",
                   summary.category_attachment_count);
  object.SizeField("property_accessor_trait_count",
                   summary.property_accessor_trait_count);
  object.SizeField("property_ivar_binding_trait_count",
                   summary.property_ivar_binding_trait_count);
  object.SizeField("method_selector_trait_count",
                   summary.method_selector_trait_count);
  object.SizeField("class_method_trait_count", summary.class_method_trait_count);
  object.SizeField("instance_method_trait_count",
                   summary.instance_method_trait_count);
  object.SizeField("implemented_method_count",
                   summary.implemented_method_count);
  object.SizeField("declaration_only_method_count",
                   summary.declaration_only_method_count);
  object.SizeField("property_attribute_profile_count",
                   summary.property_attribute_profile_count);
  object.SizeField("ownership_effect_profile_count",
                   summary.ownership_effect_profile_count);
  object.SizeField("executable_binding_trait_count",
                   summary.executable_binding_trait_count);
  object.BoolField(
      "ready",
      IsReadyObjc3CrossModuleRuntimeMetadataSemanticPreservationSummary(
          summary));
  object.BoolField("fail_closed", summary.fail_closed);
  object.BoolField("source_frontend_closure_ready",
                   summary.source_frontend_closure_ready);
  object.BoolField("runtime_metadata_source_records_ready",
                   summary.runtime_metadata_source_records_ready);
  object.BoolField("semantic_surface_published",
                   summary.semantic_surface_published);
  object.BoolField("imported_conformance_shape_landed",
                   summary.imported_conformance_shape_landed);
  object.BoolField("imported_dispatch_traits_landed",
                   summary.imported_dispatch_traits_landed);
  object.BoolField("imported_effect_traits_landed",
                   summary.imported_effect_traits_landed);
  object.BoolField("imported_runtime_metadata_semantics_landed",
                   summary.imported_runtime_metadata_semantics_landed);
  object.BoolField("ready_for_imported_metadata_semantic_rules",
                   summary.ready_for_imported_metadata_semantic_rules);
  object.BoolField("ready_for_cross_module_dispatch_equivalence",
                   summary.ready_for_cross_module_dispatch_equivalence);
  object.StringField("source_frontend_closure_replay_key",
                     summary.source_frontend_closure_replay_key);
  object.StringField("replay_key", summary.replay_key);
  object.StringField("failure_reason", summary.failure_reason);
  object.End();
  return out.str();
}

std::string BuildImportedRuntimeMetadataSemanticRulesReplayKey(
    const Objc3ImportedRuntimeMetadataSemanticRulesSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_contract="
      << summary.source_semantic_preservation_contract_id
      << ";input_model=" << summary.input_model
      << ";imported_module_count=" << summary.imported_module_count
      << ";runtime_owned_declaration_count="
      << summary.runtime_owned_declaration_count
      << ";superclass_edge_count=" << summary.superclass_edge_count
      << ";protocol_conformance_edge_count="
      << summary.protocol_conformance_edge_count
      << ";category_attachment_count=" << summary.category_attachment_count
      << ";property_accessor_trait_count="
      << summary.property_accessor_trait_count
      << ";property_ivar_binding_trait_count="
      << summary.property_ivar_binding_trait_count
      << ";method_selector_trait_count="
      << summary.method_selector_trait_count
      << ";class_method_trait_count="
      << summary.class_method_trait_count
      << ";instance_method_trait_count="
      << summary.instance_method_trait_count
      << ";implemented_method_count="
      << summary.implemented_method_count
      << ";declaration_only_method_count="
      << summary.declaration_only_method_count
      << ";property_attribute_profile_count="
      << summary.property_attribute_profile_count
      << ";ownership_effect_profile_count="
      << summary.ownership_effect_profile_count
      << ";executable_binding_trait_count="
      << summary.executable_binding_trait_count
      << ";optional_send_site_count=" << summary.optional_send_site_count
      << ";typed_keypath_literal_site_count="
      << summary.typed_keypath_literal_site_count
      << ";live_optional_lowering_site_count="
      << summary.live_optional_lowering_site_count
      << ";live_typed_keypath_artifact_site_count="
      << summary.live_typed_keypath_artifact_site_count
      << ";imported_type_system_optional_keypath_module_count="
      << summary.imported_type_system_optional_keypath_module_count
      << ";imported_type_system_generic_contract_module_count="
      << summary.imported_type_system_generic_contract_module_count
      << ";imported_generic_interface_count="
      << summary.imported_generic_interface_count
      << ";imported_generic_parameter_count="
      << summary.imported_generic_parameter_count
      << ";imported_generic_variance_annotation_count="
      << summary.imported_generic_variance_annotation_count
      << ";imported_generic_argument_reference_count="
      << summary.imported_generic_argument_reference_count
      << ";imported_protocol_qualified_generic_argument_count="
      << summary.imported_protocol_qualified_generic_argument_count
      << ";imported_type_system_nullability_contract_module_count="
      << summary.imported_type_system_nullability_contract_module_count
      << ";imported_nullability_canonical_type_count="
      << summary.imported_nullability_canonical_type_count
      << ";imported_nullability_object_type_count="
      << summary.imported_nullability_object_type_count
      << ";imported_nullable_entry_count="
      << summary.imported_nullable_entry_count
      << ";imported_nonnull_entry_count="
      << summary.imported_nonnull_entry_count
      << ";imported_implicitly_unwrapped_entry_count="
      << summary.imported_implicitly_unwrapped_entry_count
      << ";imported_null_resettable_entry_count="
      << summary.imported_null_resettable_entry_count
      << ";imported_unspecified_nullability_entry_count="
      << summary.imported_unspecified_nullability_entry_count
      << ";imported_invalid_nullability_entry_count="
      << summary.imported_invalid_nullability_entry_count
      << ";imported_type_system_protocol_contract_module_count="
      << summary.imported_type_system_protocol_contract_module_count
      << ";imported_protocol_decl_count="
      << summary.imported_protocol_decl_count
      << ";imported_protocol_inheritance_edge_count="
      << summary.imported_protocol_inheritance_edge_count
      << ";imported_protocol_required_method_count="
      << summary.imported_protocol_required_method_count
      << ";imported_protocol_optional_method_count="
      << summary.imported_protocol_optional_method_count
      << ";imported_protocol_required_property_count="
      << summary.imported_protocol_required_property_count
      << ";imported_protocol_optional_property_count="
      << summary.imported_protocol_optional_property_count
      << ";imported_class_protocol_adoption_count="
      << summary.imported_class_protocol_adoption_count
      << ";imported_category_protocol_adoption_count="
      << summary.imported_category_protocol_adoption_count
      << ";modules=";
  for (std::size_t i = 0; i < summary.imported_module_names_lexicographic.size();
       ++i) {
    if (i != 0) {
      out << ",";
    }
    out << summary.imported_module_names_lexicographic[i];
  }
  return out.str();
}

Objc3ImportedRuntimeMetadataSemanticRulesSummary
BuildImportedRuntimeMetadataSemanticRulesSummary(
    const Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary
        &source_semantic_preservation,
    const std::vector<Objc3ImportedRuntimeModuleSurface> &imported_surfaces,
    const std::size_t imported_input_path_count) {
  Objc3ImportedRuntimeMetadataSemanticRulesSummary summary;
  summary.fail_closed = true;
  summary.semantic_surface_published = true;
  summary.imported_input_path_count = imported_input_path_count;
  summary.imported_runtime_surface_inputs_present = imported_input_path_count > 0u;
  summary.imported_runtime_surface_inputs_loaded = true;
  summary.source_semantic_preservation_contract_ready =
      IsReadyObjc3CrossModuleRuntimeMetadataSemanticPreservationSummary(
          source_semantic_preservation);
  summary.imported_module_count = imported_surfaces.size();
  summary.imported_module_names_lexicographic.reserve(imported_surfaces.size());

  for (const auto &surface : imported_surfaces) {
    summary.imported_module_names_lexicographic.push_back(
        surface.frontend_closure_summary.module_name);
    const auto &records = surface.runtime_metadata_source_records;
    summary.class_record_count += records.classes_lexicographic.size();
    summary.protocol_record_count += records.protocols_lexicographic.size();
    summary.category_record_count += records.categories_lexicographic.size();
    summary.property_record_count += records.properties_lexicographic.size();
    summary.method_record_count += records.methods_lexicographic.size();
    summary.ivar_record_count += records.ivars_lexicographic.size();
    for (const auto &class_record : records.classes_lexicographic) {
      if (class_record.has_super && !class_record.super_name.empty()) {
        ++summary.superclass_edge_count;
      }
      summary.protocol_conformance_edge_count +=
          class_record.adopted_protocols_lexicographic.size();
    }
    for (const auto &protocol_record : records.protocols_lexicographic) {
      summary.protocol_conformance_edge_count +=
          protocol_record.inherited_protocols_lexicographic.size();
    }
    summary.category_attachment_count += records.categories_lexicographic.size();
    for (const auto &category_record : records.categories_lexicographic) {
      summary.protocol_conformance_edge_count +=
          category_record.adopted_protocols_lexicographic.size();
    }
    for (const auto &property_record : records.properties_lexicographic) {
      if (!property_record.effective_getter_selector.empty()) {
        ++summary.property_accessor_trait_count;
      }
      if (property_record.effective_setter_available &&
          !property_record.effective_setter_selector.empty()) {
        ++summary.property_accessor_trait_count;
      }
      if (!property_record.ivar_binding_symbol.empty()) {
        ++summary.property_ivar_binding_trait_count;
      }
      if (!property_record.property_attribute_profile.empty()) {
        ++summary.property_attribute_profile_count;
      }
      if (!property_record.ownership_lifetime_profile.empty() ||
          !property_record.ownership_runtime_hook_profile.empty() ||
          !property_record.accessor_ownership_profile.empty()) {
        ++summary.ownership_effect_profile_count;
      }
      if ((property_record.executable_synthesized_binding_kind != "none" &&
           !property_record.executable_synthesized_binding_kind.empty()) ||
          !property_record.executable_synthesized_binding_symbol.empty() ||
          !property_record.executable_ivar_layout_symbol.empty()) {
        ++summary.executable_binding_trait_count;
      }
    }
    for (const auto &method_record : records.methods_lexicographic) {
      if (!method_record.selector.empty()) {
        ++summary.method_selector_trait_count;
      }
      if (method_record.is_class_method) {
        ++summary.class_method_trait_count;
      } else {
        ++summary.instance_method_trait_count;
      }
      if (method_record.has_body) {
        ++summary.implemented_method_count;
      } else {
        ++summary.declaration_only_method_count;
      }
    }
    for (const auto &ivar_record : records.ivars_lexicographic) {
      if ((ivar_record.executable_synthesized_binding_kind != "none" &&
           !ivar_record.executable_synthesized_binding_kind.empty()) ||
          !ivar_record.executable_synthesized_binding_symbol.empty() ||
          !ivar_record.executable_ivar_layout_symbol.empty()) {
        ++summary.executable_binding_trait_count;
      }
    }
    if (surface.type_system_optional_keypath_lowering_contract_present) {
      ++summary.imported_type_system_optional_keypath_module_count;
      summary.optional_send_site_count += surface.type_system_optional_send_sites;
      summary.typed_keypath_literal_site_count +=
          surface.type_system_typed_keypath_literal_sites;
      summary.live_optional_lowering_site_count +=
          surface.type_system_live_optional_lowering_sites;
      summary.live_typed_keypath_artifact_site_count +=
          surface.type_system_live_typed_keypath_artifact_sites;
      if (surface.type_system_optional_send_runtime_ready) {
        ++summary.imported_optional_runtime_ready_module_count;
      }
      if (surface.type_system_typed_keypath_descriptor_handles_ready &&
          surface.type_system_typed_keypath_runtime_execution_helper_landed) {
        ++summary.imported_typed_keypath_runtime_ready_module_count;
      }
    }
    if (surface.type_system_generic_contract_preservation_present) {
      ++summary.imported_type_system_generic_contract_module_count;
      summary.imported_generic_interface_count +=
          surface.type_system_generic_interface_count;
      summary.imported_generic_parameter_count +=
          surface.type_system_generic_parameter_count;
      summary.imported_generic_variance_annotation_count +=
          surface.type_system_generic_variance_annotation_count;
      summary.imported_generic_argument_reference_count +=
          surface.type_system_generic_argument_reference_count;
      summary.imported_protocol_qualified_generic_argument_count +=
          surface.type_system_protocol_qualified_generic_argument_count;
    }
    if (surface.type_system_nullability_contract_preservation_present) {
      ++summary.imported_type_system_nullability_contract_module_count;
      summary.imported_nullability_canonical_type_count +=
          surface.type_system_nullability_canonical_type_count;
      summary.imported_nullability_object_type_count +=
          surface.type_system_nullability_object_type_count;
      summary.imported_nullable_entry_count +=
          surface.type_system_nullable_entry_count;
      summary.imported_nonnull_entry_count +=
          surface.type_system_nonnull_entry_count;
      summary.imported_implicitly_unwrapped_entry_count +=
          surface.type_system_implicitly_unwrapped_entry_count;
      summary.imported_null_resettable_entry_count +=
          surface.type_system_null_resettable_entry_count;
      summary.imported_unspecified_nullability_entry_count +=
          surface.type_system_unspecified_nullability_entry_count;
      summary.imported_invalid_nullability_entry_count +=
          surface.type_system_invalid_nullability_entry_count;
    }
    if (surface.type_system_protocol_contract_preservation_present) {
      ++summary.imported_type_system_protocol_contract_module_count;
      summary.imported_protocol_decl_count +=
          surface.type_system_protocol_decl_count;
      summary.imported_protocol_forward_declaration_count +=
          surface.type_system_protocol_forward_declaration_count;
      summary.imported_protocol_inheritance_edge_count +=
          surface.type_system_protocol_inheritance_edge_count;
      summary.imported_protocol_required_method_count +=
          surface.type_system_protocol_required_method_count;
      summary.imported_protocol_optional_method_count +=
          surface.type_system_protocol_optional_method_count;
      summary.imported_protocol_required_property_count +=
          surface.type_system_protocol_required_property_count;
      summary.imported_protocol_optional_property_count +=
          surface.type_system_protocol_optional_property_count;
      summary.imported_class_protocol_adoption_count +=
          surface.type_system_class_protocol_adoption_count;
      summary.imported_category_protocol_adoption_count +=
          surface.type_system_category_protocol_adoption_count;
    }
  }

  std::sort(summary.imported_module_names_lexicographic.begin(),
            summary.imported_module_names_lexicographic.end());
  summary.runtime_owned_declaration_count =
      summary.class_record_count + summary.protocol_record_count +
      summary.category_record_count + summary.property_record_count +
      summary.method_record_count + summary.ivar_record_count;
  summary.imported_conformance_shape_landed =
      summary.source_semantic_preservation_contract_ready;
  summary.imported_dispatch_traits_landed =
      summary.source_semantic_preservation_contract_ready;
  summary.imported_effect_traits_landed =
      summary.source_semantic_preservation_contract_ready;
  summary.imported_runtime_metadata_semantics_landed =
      summary.source_semantic_preservation_contract_ready;
  const bool imported_optional_keypath_landed =
      summary.imported_type_system_optional_keypath_module_count == 0u ||
      summary.imported_type_system_optional_keypath_module_count ==
          summary.imported_optional_runtime_ready_module_count;
  const bool imported_generic_contract_landed =
      summary.imported_type_system_generic_contract_module_count == 0u ||
      summary.imported_generic_variance_annotation_count >=
          summary.imported_generic_parameter_count;
  const bool imported_nullability_contract_landed =
      summary.imported_type_system_nullability_contract_module_count == 0u ||
      summary.imported_nullability_canonical_type_count ==
          summary.imported_nullable_entry_count +
              summary.imported_nonnull_entry_count +
              summary.imported_implicitly_unwrapped_entry_count +
              summary.imported_null_resettable_entry_count +
              summary.imported_unspecified_nullability_entry_count;
  const bool imported_protocol_contract_landed =
      summary.imported_type_system_protocol_contract_module_count == 0u ||
      summary.imported_protocol_decl_count >=
          summary.imported_protocol_forward_declaration_count;
  summary.imported_type_system_type_surface_landed =
      imported_optional_keypath_landed && imported_generic_contract_landed &&
      imported_nullability_contract_landed && imported_protocol_contract_landed;
  summary.imported_optional_runtime_semantics_landed =
      summary.optional_send_site_count == 0u ||
      summary.imported_optional_runtime_ready_module_count > 0u;
  summary.imported_typed_keypath_runtime_semantics_landed =
      summary.typed_keypath_literal_site_count == 0u ||
      summary.imported_typed_keypath_runtime_ready_module_count > 0u;
  summary.ready_for_imported_metadata_semantic_rules =
      summary.source_semantic_preservation_contract_ready &&
      summary.imported_runtime_surface_inputs_loaded;
  summary.ready_for_cross_module_dispatch_equivalence =
      summary.ready_for_imported_metadata_semantic_rules;

  if (summary.ready_for_imported_metadata_semantic_rules) {
    summary.replay_key =
        BuildImportedRuntimeMetadataSemanticRulesReplayKey(summary);
  } else {
    summary.failure_reason =
        "imported runtime metadata semantic rules summary is incomplete";
  }
  if (!IsReadyObjc3ImportedRuntimeMetadataSemanticRulesSummary(summary) &&
      summary.failure_reason.empty()) {
    summary.failure_reason =
        "imported runtime metadata semantic rules summary is incomplete";
  }
  return summary;
}

std::string BuildImportedRuntimeMetadataSemanticRulesSummaryJson(
    const Objc3ImportedRuntimeMetadataSemanticRulesSummary &summary) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.StringField("contract_id", summary.contract_id);
  object.StringField("source_semantic_preservation_contract_id",
                     summary.source_semantic_preservation_contract_id);
  object.StringField("frontend_surface_path", summary.frontend_surface_path);
  object.StringField("authority_model", summary.authority_model);
  object.StringField("input_model", summary.input_model);
  object.StringArrayField("imported_module_names_lexicographic",
                          summary.imported_module_names_lexicographic);
  object.SizeField("imported_input_path_count",
                   summary.imported_input_path_count);
  object.SizeField("imported_module_count", summary.imported_module_count);
  object.SizeField("class_record_count", summary.class_record_count);
  object.SizeField("protocol_record_count", summary.protocol_record_count);
  object.SizeField("category_record_count", summary.category_record_count);
  object.SizeField("property_record_count", summary.property_record_count);
  object.SizeField("method_record_count", summary.method_record_count);
  object.SizeField("ivar_record_count", summary.ivar_record_count);
  object.SizeField("runtime_owned_declaration_count",
                   summary.runtime_owned_declaration_count);
  object.SizeField("superclass_edge_count", summary.superclass_edge_count);
  object.SizeField("protocol_conformance_edge_count",
                   summary.protocol_conformance_edge_count);
  object.SizeField("category_attachment_count",
                   summary.category_attachment_count);
  object.SizeField("property_accessor_trait_count",
                   summary.property_accessor_trait_count);
  object.SizeField("property_ivar_binding_trait_count",
                   summary.property_ivar_binding_trait_count);
  object.SizeField("method_selector_trait_count",
                   summary.method_selector_trait_count);
  object.SizeField("class_method_trait_count", summary.class_method_trait_count);
  object.SizeField("instance_method_trait_count",
                   summary.instance_method_trait_count);
  object.SizeField("implemented_method_count",
                   summary.implemented_method_count);
  object.SizeField("declaration_only_method_count",
                   summary.declaration_only_method_count);
  object.SizeField("property_attribute_profile_count",
                   summary.property_attribute_profile_count);
  object.SizeField("ownership_effect_profile_count",
                   summary.ownership_effect_profile_count);
  object.SizeField("executable_binding_trait_count",
                   summary.executable_binding_trait_count);
  object.SizeField("optional_send_site_count", summary.optional_send_site_count);
  object.SizeField("typed_keypath_literal_site_count",
                   summary.typed_keypath_literal_site_count);
  object.SizeField("live_optional_lowering_site_count",
                   summary.live_optional_lowering_site_count);
  object.SizeField("live_typed_keypath_artifact_site_count",
                   summary.live_typed_keypath_artifact_site_count);
  object.SizeField("imported_type_system_optional_keypath_module_count",
                   summary.imported_type_system_optional_keypath_module_count);
  object.SizeField("imported_optional_runtime_ready_module_count",
                   summary.imported_optional_runtime_ready_module_count);
  object.SizeField("imported_typed_keypath_runtime_ready_module_count",
                   summary.imported_typed_keypath_runtime_ready_module_count);
  object.SizeField("imported_type_system_generic_contract_module_count",
                   summary.imported_type_system_generic_contract_module_count);
  object.SizeField("imported_generic_interface_count",
                   summary.imported_generic_interface_count);
  object.SizeField("imported_generic_parameter_count",
                   summary.imported_generic_parameter_count);
  object.SizeField("imported_generic_variance_annotation_count",
                   summary.imported_generic_variance_annotation_count);
  object.SizeField("imported_generic_argument_reference_count",
                   summary.imported_generic_argument_reference_count);
  object.SizeField("imported_protocol_qualified_generic_argument_count",
                   summary.imported_protocol_qualified_generic_argument_count);
  object.SizeField("imported_type_system_nullability_contract_module_count",
                   summary.imported_type_system_nullability_contract_module_count);
  object.SizeField("imported_nullability_canonical_type_count",
                   summary.imported_nullability_canonical_type_count);
  object.SizeField("imported_nullability_object_type_count",
                   summary.imported_nullability_object_type_count);
  object.SizeField("imported_nullable_entry_count",
                   summary.imported_nullable_entry_count);
  object.SizeField("imported_nonnull_entry_count",
                   summary.imported_nonnull_entry_count);
  object.SizeField("imported_implicitly_unwrapped_entry_count",
                   summary.imported_implicitly_unwrapped_entry_count);
  object.SizeField("imported_null_resettable_entry_count",
                   summary.imported_null_resettable_entry_count);
  object.SizeField("imported_unspecified_nullability_entry_count",
                   summary.imported_unspecified_nullability_entry_count);
  object.SizeField("imported_invalid_nullability_entry_count",
                   summary.imported_invalid_nullability_entry_count);
  object.SizeField("imported_type_system_protocol_contract_module_count",
                   summary.imported_type_system_protocol_contract_module_count);
  object.SizeField("imported_protocol_decl_count",
                   summary.imported_protocol_decl_count);
  object.SizeField("imported_protocol_forward_declaration_count",
                   summary.imported_protocol_forward_declaration_count);
  object.SizeField("imported_protocol_inheritance_edge_count",
                   summary.imported_protocol_inheritance_edge_count);
  object.SizeField("imported_protocol_required_method_count",
                   summary.imported_protocol_required_method_count);
  object.SizeField("imported_protocol_optional_method_count",
                   summary.imported_protocol_optional_method_count);
  object.SizeField("imported_protocol_required_property_count",
                   summary.imported_protocol_required_property_count);
  object.SizeField("imported_protocol_optional_property_count",
                   summary.imported_protocol_optional_property_count);
  object.SizeField("imported_class_protocol_adoption_count",
                   summary.imported_class_protocol_adoption_count);
  object.SizeField("imported_category_protocol_adoption_count",
                   summary.imported_category_protocol_adoption_count);
  object.BoolField(
      "ready",
      IsReadyObjc3ImportedRuntimeMetadataSemanticRulesSummary(summary));
  object.BoolField("fail_closed", summary.fail_closed);
  object.BoolField("source_semantic_preservation_contract_ready",
                   summary.source_semantic_preservation_contract_ready);
  object.BoolField("semantic_surface_published",
                   summary.semantic_surface_published);
  object.BoolField("imported_runtime_surface_inputs_present",
                   summary.imported_runtime_surface_inputs_present);
  object.BoolField("imported_runtime_surface_inputs_loaded",
                   summary.imported_runtime_surface_inputs_loaded);
  object.BoolField("imported_conformance_shape_landed",
                   summary.imported_conformance_shape_landed);
  object.BoolField("imported_dispatch_traits_landed",
                   summary.imported_dispatch_traits_landed);
  object.BoolField("imported_effect_traits_landed",
                   summary.imported_effect_traits_landed);
  object.BoolField("imported_runtime_metadata_semantics_landed",
                   summary.imported_runtime_metadata_semantics_landed);
  object.BoolField("imported_type_system_type_surface_landed",
                   summary.imported_type_system_type_surface_landed);
  object.BoolField("imported_optional_runtime_semantics_landed",
                   summary.imported_optional_runtime_semantics_landed);
  object.BoolField("imported_typed_keypath_runtime_semantics_landed",
                   summary.imported_typed_keypath_runtime_semantics_landed);
  object.BoolField("ready_for_imported_metadata_semantic_rules",
                   summary.ready_for_imported_metadata_semantic_rules);
  object.BoolField("ready_for_cross_module_dispatch_equivalence",
                   summary.ready_for_cross_module_dispatch_equivalence);
  object.StringField("replay_key", summary.replay_key);
  object.StringField("failure_reason", summary.failure_reason);
  object.End();
  return out.str();
}

std::string BuildSerializedRuntimeMetadataImportLoweringReplayKey(
    const Objc3SerializedRuntimeMetadataImportLoweringSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_contract="
      << summary.source_imported_semantic_rules_contract_id
      << ";input_model=" << summary.input_model
      << ";imported_input_path_count=" << summary.imported_input_path_count
      << ";imported_module_count=" << summary.imported_module_count
      << ";imported_surface_ingest_landed="
      << (summary.imported_surface_ingest_landed ? "true" : "false")
      << ";serialized_metadata_rehydration_landed="
      << (summary.serialized_metadata_rehydration_landed ? "true" : "false")
      << ";incremental_reuse_landed="
      << (summary.incremental_reuse_landed ? "true" : "false")
      << ";imported_metadata_ir_lowering_landed="
      << (summary.imported_metadata_ir_lowering_landed ? "true" : "false")
      << ";public_live_imported_payload_abi_landed="
      << (summary.public_live_imported_payload_abi_landed ? "true" : "false");
  return out.str();
}

Objc3SerializedRuntimeMetadataImportLoweringSummary
BuildSerializedRuntimeMetadataImportLoweringSummary(
    const Objc3ImportedRuntimeMetadataSemanticRulesSummary
        &imported_runtime_metadata_semantic_rules) {
  Objc3SerializedRuntimeMetadataImportLoweringSummary summary;
  summary.imported_input_path_count =
      imported_runtime_metadata_semantic_rules.imported_input_path_count;
  summary.imported_module_count =
      imported_runtime_metadata_semantic_rules.imported_module_count;
  summary.fail_closed = true;
  summary.source_imported_semantic_rules_ready =
      IsReadyObjc3ImportedRuntimeMetadataSemanticRulesSummary(
          imported_runtime_metadata_semantic_rules);
  summary.semantic_surface_published = true;
  summary.imported_surface_ingest_landed =
      imported_runtime_metadata_semantic_rules
          .ready_for_imported_metadata_semantic_rules;
  if (summary.source_imported_semantic_rules_ready) {
    summary.replay_key =
        BuildSerializedRuntimeMetadataImportLoweringReplayKey(summary);
  } else {
    summary.failure_reason =
        "serialized runtime metadata import/lowering summary is incomplete";
  }
  if (!IsReadyObjc3SerializedRuntimeMetadataImportLoweringSummary(summary) &&
      summary.failure_reason.empty()) {
    summary.failure_reason =
        "serialized runtime metadata import/lowering summary is incomplete";
  }
  return summary;
}

std::string BuildSerializedRuntimeMetadataImportLoweringSummaryJson(
    const Objc3SerializedRuntimeMetadataImportLoweringSummary &summary) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.StringField("contract_id", summary.contract_id);
  object.StringField("source_imported_semantic_rules_contract_id",
                     summary.source_imported_semantic_rules_contract_id);
  object.StringField("frontend_surface_path", summary.frontend_surface_path);
  object.StringField("source_artifact_relative_path",
                     summary.source_artifact_relative_path);
  object.StringField("authority_model", summary.authority_model);
  object.StringField("input_model", summary.input_model);
  object.SizeField("imported_input_path_count",
                   summary.imported_input_path_count);
  object.SizeField("imported_module_count", summary.imported_module_count);
  object.BoolField(
      "ready",
      IsReadyObjc3SerializedRuntimeMetadataImportLoweringSummary(summary));
  object.BoolField("fail_closed", summary.fail_closed);
  object.BoolField("source_imported_semantic_rules_ready",
                   summary.source_imported_semantic_rules_ready);
  object.BoolField("semantic_surface_published",
                   summary.semantic_surface_published);
  object.BoolField("imported_surface_ingest_landed",
                   summary.imported_surface_ingest_landed);
  object.BoolField("serialized_metadata_rehydration_landed",
                   summary.serialized_metadata_rehydration_landed);
  object.BoolField("incremental_reuse_landed",
                   summary.incremental_reuse_landed);
  object.BoolField("imported_metadata_ir_lowering_landed",
                   summary.imported_metadata_ir_lowering_landed);
  object.BoolField("public_live_imported_payload_abi_landed",
                   summary.public_live_imported_payload_abi_landed);
  object.BoolField("ready_for_serialized_metadata_lowering_impl",
                   summary.ready_for_serialized_metadata_lowering_impl);
  object.BoolField("ready_for_incremental_reuse_impl",
                   summary.ready_for_incremental_reuse_impl);
  object.StringField("replay_key", summary.replay_key);
  object.StringField("failure_reason", summary.failure_reason);
  object.End();
  return out.str();
}

std::string BuildRuntimeMetadataClassRecordMergeKey(
    const Objc3RuntimeMetadataClassSourceRecord &record) {
  return record.record_kind + "|" + record.name;
}

std::string BuildRuntimeMetadataProtocolRecordMergeKey(
    const Objc3RuntimeMetadataProtocolSourceRecord &record) {
  return record.name;
}

std::string BuildRuntimeMetadataCategoryRecordMergeKey(
    const Objc3RuntimeMetadataCategorySourceRecord &record) {
  return record.record_kind + "|" + record.class_name + "|" +
         record.category_name;
}

std::string BuildRuntimeMetadataPropertyRecordMergeKey(
    const Objc3RuntimeMetadataPropertySourceRecord &record) {
  return record.owner_kind + "|" + record.owner_name + "|" +
         record.property_name;
}

std::string BuildRuntimeMetadataMethodRecordMergeKey(
    const Objc3RuntimeMetadataMethodSourceRecord &record) {
  return record.owner_kind + "|" + record.owner_name + "|" + record.selector +
         "|" + (record.is_class_method ? "class" : "instance");
}

std::string BuildRuntimeMetadataIvarRecordMergeKey(
    const Objc3RuntimeMetadataIvarSourceRecord &record) {
  return record.owner_kind + "|" + record.owner_name + "|" +
         record.property_name + "|" + record.ivar_binding_symbol;
}

struct Objc3AccessorStorageLoweringMetadataSummary {
  std::size_t synthesized_accessor_owner_entries = 0;
  std::size_t synthesized_getter_entries = 0;
  std::size_t synthesized_setter_entries = 0;
  std::size_t current_property_read_entries = 0;
  std::size_t current_property_write_entries = 0;
  std::size_t current_property_exchange_entries = 0;
  std::size_t weak_current_property_load_entries = 0;
  std::size_t weak_current_property_store_entries = 0;
  bool deterministic = false;
};

Objc3AccessorStorageLoweringMetadataSummary
BuildAccessorStorageLoweringMetadataSummary(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  Objc3AccessorStorageLoweringMetadataSummary summary;
  summary.deterministic = records.deterministic;
  for (const auto &property_record : records.properties_lexicographic) {
    if (!property_record.synthesizes_executable_accessors) {
      continue;
    }
    ++summary.synthesized_accessor_owner_entries;
    if (!property_record.getter_storage_runtime_helper_symbol.empty()) {
      ++summary.synthesized_getter_entries;
      if (property_record.getter_storage_runtime_helper_symbol ==
          kObjc3RuntimeReadCurrentPropertyI32Symbol) {
        ++summary.current_property_read_entries;
      } else if (property_record.getter_storage_runtime_helper_symbol ==
                 kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol) {
        ++summary.weak_current_property_load_entries;
      }
    }
    if (!property_record.setter_storage_runtime_helper_symbol.empty()) {
      ++summary.synthesized_setter_entries;
      if (property_record.setter_storage_runtime_helper_symbol ==
          kObjc3RuntimeWriteCurrentPropertyI32Symbol) {
        ++summary.current_property_write_entries;
      } else if (property_record.setter_storage_runtime_helper_symbol ==
                 kObjc3RuntimeExchangeCurrentPropertyI32Symbol) {
        ++summary.current_property_exchange_entries;
      } else if (property_record.setter_storage_runtime_helper_symbol ==
                 kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol) {
        ++summary.weak_current_property_store_entries;
      }
    }
  }
  return summary;
}

struct Objc3ExecutableAccessorLayoutLoweringSummary {
  std::size_t property_metadata_entries = 0;
  std::size_t ivar_metadata_entries = 0;
  std::size_t property_attribute_profile_entries = 0;
  std::size_t accessor_ownership_profile_entries = 0;
  std::size_t synthesized_binding_entries = 0;
  std::size_t implementation_owned_property_entries = 0;
  std::size_t synthesized_getter_entries = 0;
  std::size_t synthesized_setter_entries = 0;
  std::size_t synthesized_accessor_entries = 0;
  std::size_t ivar_layout_entries = 0;
  std::size_t ivar_layout_owner_entries = 0;
  bool deterministic = false;
};

Objc3ExecutableAccessorLayoutLoweringSummary
BuildExecutableAccessorLayoutLoweringSummary(
    const Objc3ExecutableMetadataSourceGraph &source_graph) {
  Objc3ExecutableAccessorLayoutLoweringSummary summary;
  summary.deterministic = source_graph.deterministic;
  std::set<std::string> ivar_layout_owner_identities;
  for (const auto &property_node : source_graph.property_nodes_lexicographic) {
    ++summary.property_metadata_entries;
    if (!property_node.property_attribute_profile.empty()) {
      ++summary.property_attribute_profile_entries;
    }
    if (!property_node.accessor_ownership_profile.empty()) {
      ++summary.accessor_ownership_profile_entries;
    }
    if (!property_node.executable_synthesized_binding_kind.empty()) {
      ++summary.synthesized_binding_entries;
    }
    if (property_node.owner_kind == "class-implementation" &&
        property_node.synthesizes_executable_accessors) {
      ++summary.implementation_owned_property_entries;
      if (!property_node.effective_getter_selector.empty()) {
        ++summary.synthesized_getter_entries;
        ++summary.synthesized_accessor_entries;
      }
      if (property_node.effective_setter_available &&
          !property_node.effective_setter_selector.empty()) {
        ++summary.synthesized_setter_entries;
        ++summary.synthesized_accessor_entries;
      }
    }
  }
  for (const auto &ivar_node : source_graph.ivar_nodes_lexicographic) {
    ++summary.ivar_metadata_entries;
    ++summary.ivar_layout_entries;
    if (!ivar_node.declaration_owner_identity.empty()) {
      ivar_layout_owner_identities.insert(ivar_node.declaration_owner_identity);
    }
  }
  summary.ivar_layout_owner_entries = ivar_layout_owner_identities.size();
  return summary;
}

template <typename RecordT, typename KeyFn>
std::vector<RecordT> BuildMergedRuntimeMetadataRecordVector(
    const std::vector<const std::vector<RecordT> *> &imported_record_vectors,
    const std::vector<RecordT> &local_records, KeyFn key_fn) {
  std::unordered_map<std::string, RecordT> merged_by_key;
  for (const auto *records : imported_record_vectors) {
    for (const auto &record : *records) {
      merged_by_key.emplace(key_fn(record), record);
    }
  }
  for (const auto &record : local_records) {
    merged_by_key[key_fn(record)] = record;
  }
  std::vector<std::pair<std::string, RecordT>> ordered_records;
  ordered_records.reserve(merged_by_key.size());
  for (auto &entry : merged_by_key) {
    ordered_records.push_back(std::move(entry));
  }
  std::sort(ordered_records.begin(), ordered_records.end(),
            [](const auto &lhs, const auto &rhs) { return lhs.first < rhs.first; });
  std::vector<RecordT> merged;
  merged.reserve(ordered_records.size());
  for (auto &entry : ordered_records) {
    merged.push_back(std::move(entry.second));
  }
  return merged;
}

std::vector<const Objc3ImportedRuntimeModuleSurface *>
BuildSortedImportedRuntimeModuleSurfacePointers(
    const std::vector<Objc3ImportedRuntimeModuleSurface> &imported_surfaces) {
  std::vector<const Objc3ImportedRuntimeModuleSurface *> sorted;
  sorted.reserve(imported_surfaces.size());
  for (const auto &surface : imported_surfaces) {
    sorted.push_back(&surface);
  }
  std::sort(sorted.begin(), sorted.end(),
            [](const auto *lhs, const auto *rhs) {
              return lhs->frontend_closure_summary.module_name <
                     rhs->frontend_closure_summary.module_name;
            });
  return sorted;
}

std::vector<std::string> BuildSerializedRuntimeMetadataReusedModuleNames(
    const std::string &local_module_name,
    const std::vector<Objc3ImportedRuntimeModuleSurface> &imported_surfaces) {
  std::set<std::string> names;
  if (!local_module_name.empty()) {
    names.insert(local_module_name);
  }
  for (const auto *surface :
       BuildSortedImportedRuntimeModuleSurfacePointers(imported_surfaces)) {
    if (surface->uses_serialized_runtime_metadata_payload &&
        !surface->reused_module_names_lexicographic.empty()) {
      names.insert(surface->reused_module_names_lexicographic.begin(),
                   surface->reused_module_names_lexicographic.end());
    } else if (!surface->frontend_closure_summary.module_name.empty()) {
      names.insert(surface->frontend_closure_summary.module_name);
    }
  }
  return std::vector<std::string>(names.begin(), names.end());
}

Objc3RuntimeMetadataSourceRecordSet BuildSerializedRuntimeMetadataReuseRecordSet(
    const Objc3RuntimeMetadataSourceRecordSet &local_runtime_metadata_source_records,
    const std::vector<Objc3ImportedRuntimeModuleSurface> &imported_surfaces) {
  std::vector<const std::vector<Objc3RuntimeMetadataClassSourceRecord> *>
      imported_classes;
  std::vector<const std::vector<Objc3RuntimeMetadataProtocolSourceRecord> *>
      imported_protocols;
  std::vector<const std::vector<Objc3RuntimeMetadataCategorySourceRecord> *>
      imported_categories;
  std::vector<const std::vector<Objc3RuntimeMetadataPropertySourceRecord> *>
      imported_properties;
  std::vector<const std::vector<Objc3RuntimeMetadataMethodSourceRecord> *>
      imported_methods;
  std::vector<const std::vector<Objc3RuntimeMetadataIvarSourceRecord> *>
      imported_ivars;
  for (const auto *surface :
       BuildSortedImportedRuntimeModuleSurfacePointers(imported_surfaces)) {
    imported_classes.push_back(
        &surface->runtime_metadata_source_records.classes_lexicographic);
    imported_protocols.push_back(
        &surface->runtime_metadata_source_records.protocols_lexicographic);
    imported_categories.push_back(
        &surface->runtime_metadata_source_records.categories_lexicographic);
    imported_properties.push_back(
        &surface->runtime_metadata_source_records.properties_lexicographic);
    imported_methods.push_back(
        &surface->runtime_metadata_source_records.methods_lexicographic);
    imported_ivars.push_back(
        &surface->runtime_metadata_source_records.ivars_lexicographic);
  }

  Objc3RuntimeMetadataSourceRecordSet merged;
  merged.classes_lexicographic = BuildMergedRuntimeMetadataRecordVector(
      imported_classes, local_runtime_metadata_source_records.classes_lexicographic,
      BuildRuntimeMetadataClassRecordMergeKey);
  merged.protocols_lexicographic = BuildMergedRuntimeMetadataRecordVector(
      imported_protocols,
      local_runtime_metadata_source_records.protocols_lexicographic,
      BuildRuntimeMetadataProtocolRecordMergeKey);
  merged.categories_lexicographic = BuildMergedRuntimeMetadataRecordVector(
      imported_categories,
      local_runtime_metadata_source_records.categories_lexicographic,
      BuildRuntimeMetadataCategoryRecordMergeKey);
  merged.properties_lexicographic = BuildMergedRuntimeMetadataRecordVector(
      imported_properties,
      local_runtime_metadata_source_records.properties_lexicographic,
      BuildRuntimeMetadataPropertyRecordMergeKey);
  merged.methods_lexicographic = BuildMergedRuntimeMetadataRecordVector(
      imported_methods, local_runtime_metadata_source_records.methods_lexicographic,
      BuildRuntimeMetadataMethodRecordMergeKey);
  merged.ivars_lexicographic = BuildMergedRuntimeMetadataRecordVector(
      imported_ivars, local_runtime_metadata_source_records.ivars_lexicographic,
      BuildRuntimeMetadataIvarRecordMergeKey);
  merged.deterministic = true;
  return merged;
}

std::string BuildSerializedRuntimeMetadataArtifactReuseReplayKey(
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_contract="
      << summary.source_serialized_import_lowering_contract_id
      << ";artifact=" << summary.artifact_relative_path
      << ";reused_module_count=" << summary.reused_module_count
      << ";runtime_owned_declaration_count="
      << summary.runtime_owned_declaration_count
      << ";metadata_reference_count=" << summary.metadata_reference_count
      << ";modules=";
  for (std::size_t i = 0; i < summary.reused_module_names_lexicographic.size();
       ++i) {
    if (i != 0u) {
      out << ",";
    }
    out << summary.reused_module_names_lexicographic[i];
  }
  return out.str();
}

Objc3SerializedRuntimeMetadataArtifactReuseSummary
BuildSerializedRuntimeMetadataArtifactReuseSummary(
    const Objc3SerializedRuntimeMetadataImportLoweringSummary
        &serialized_import_lowering,
    const std::string &local_module_name,
    const Objc3RuntimeMetadataSourceRecordSet &reused_runtime_metadata_source_records,
    const std::vector<std::string> &reused_module_names_lexicographic) {
  Objc3SerializedRuntimeMetadataArtifactReuseSummary summary;
  summary.reused_module_names_lexicographic = reused_module_names_lexicographic;
  summary.reused_module_count = reused_module_names_lexicographic.size();
  summary.class_record_count =
      reused_runtime_metadata_source_records.classes_lexicographic.size();
  summary.protocol_record_count =
      reused_runtime_metadata_source_records.protocols_lexicographic.size();
  summary.category_record_count =
      reused_runtime_metadata_source_records.categories_lexicographic.size();
  summary.property_record_count =
      reused_runtime_metadata_source_records.properties_lexicographic.size();
  summary.method_record_count =
      reused_runtime_metadata_source_records.methods_lexicographic.size();
  summary.ivar_record_count =
      reused_runtime_metadata_source_records.ivars_lexicographic.size();
  summary.runtime_owned_declaration_count =
      CountRuntimeMetadataSourceRecordSetDeclarations(
          reused_runtime_metadata_source_records);
  summary.metadata_reference_count =
      CountRuntimeMetadataSourceRecordSetReferences(
          reused_runtime_metadata_source_records);
  summary.fail_closed = true;
  summary.source_serialized_import_lowering_ready =
      IsReadyObjc3SerializedRuntimeMetadataImportLoweringSummary(
          serialized_import_lowering);
  summary.semantic_surface_published = true;
  summary.serialized_metadata_rehydration_landed =
      summary.source_serialized_import_lowering_ready;
  summary.artifact_reuse_landed =
      summary.source_serialized_import_lowering_ready;
  summary.downstream_module_consumption_ready =
      summary.source_serialized_import_lowering_ready;
  if (summary.source_serialized_import_lowering_ready) {
    summary.replay_key =
        BuildSerializedRuntimeMetadataArtifactReuseReplayKey(summary);
  } else {
    summary.failure_reason =
        "serialized runtime metadata artifact reuse summary is incomplete";
  }
  if (!IsReadyObjc3SerializedRuntimeMetadataArtifactReuseSummary(summary) &&
      summary.failure_reason.empty()) {
    summary.failure_reason =
        "serialized runtime metadata artifact reuse summary is incomplete";
  }
  (void)local_module_name;
  return summary;
}

std::string BuildSerializedRuntimeMetadataArtifactReuseSummaryJson(
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary &summary) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.StringField("contract_id", summary.contract_id);
  object.StringField("source_serialized_import_lowering_contract_id",
                     summary.source_serialized_import_lowering_contract_id);
  object.StringField("frontend_surface_path", summary.frontend_surface_path);
  object.StringField("artifact_relative_path", summary.artifact_relative_path);
  object.StringField("payload_member_name", summary.payload_member_name);
  object.StringField("authority_model", summary.authority_model);
  object.StringField("input_model", summary.input_model);
  object.StringArrayField("reused_module_names_lexicographic",
                          summary.reused_module_names_lexicographic);
  object.SizeField("reused_module_count", summary.reused_module_count);
  object.SizeField("class_record_count", summary.class_record_count);
  object.SizeField("protocol_record_count", summary.protocol_record_count);
  object.SizeField("category_record_count", summary.category_record_count);
  object.SizeField("property_record_count", summary.property_record_count);
  object.SizeField("method_record_count", summary.method_record_count);
  object.SizeField("ivar_record_count", summary.ivar_record_count);
  object.SizeField("runtime_owned_declaration_count",
                   summary.runtime_owned_declaration_count);
  object.SizeField("metadata_reference_count", summary.metadata_reference_count);
  object.BoolField(
      "ready",
      IsReadyObjc3SerializedRuntimeMetadataArtifactReuseSummary(summary));
  object.BoolField("fail_closed", summary.fail_closed);
  object.BoolField("source_serialized_import_lowering_ready",
                   summary.source_serialized_import_lowering_ready);
  object.BoolField("semantic_surface_published",
                   summary.semantic_surface_published);
  object.BoolField("serialized_metadata_rehydration_landed",
                   summary.serialized_metadata_rehydration_landed);
  object.BoolField("artifact_reuse_landed", summary.artifact_reuse_landed);
  object.BoolField("downstream_module_consumption_ready",
                   summary.downstream_module_consumption_ready);
  object.StringField("replay_key", summary.replay_key);
  object.StringField("failure_reason", summary.failure_reason);
  object.End();
  return out.str();
}

std::string BuildCrossModuleBuildRuntimeOrchestrationReplayKey(
    const Objc3CrossModuleBuildRuntimeOrchestrationSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_contract="
      << summary
             .source_serialized_runtime_metadata_artifact_reuse_contract_id
      << ";local_manifest_contract="
      << summary.source_local_registration_manifest_contract_id
      << ";imported_runtime_semantic_rules_contract="
      << summary.source_imported_runtime_metadata_semantic_rules_contract_id
      << ";module_image_count=" << summary.module_image_count
      << ";direct_import_input_count=" << summary.direct_import_input_count
      << ";local_total_descriptor_count="
      << summary.local_total_descriptor_count
      << ";transitive_runtime_owned_declaration_count="
      << summary.transitive_runtime_owned_declaration_count
      << ";transitive_metadata_reference_count="
      << summary.transitive_metadata_reference_count
      << ";imported_optional_send_site_count="
      << summary.imported_optional_send_site_count
      << ";imported_typed_keypath_literal_site_count="
      << summary.imported_typed_keypath_literal_site_count << ";modules=";
  for (std::size_t i = 0; i < summary.module_names_lexicographic.size(); ++i) {
    if (i != 0u) {
      out << ",";
    }
    out << summary.module_names_lexicographic[i];
  }
  return out.str();
}

Objc3CrossModuleBuildRuntimeOrchestrationSummary
BuildCrossModuleBuildRuntimeOrchestrationSummary(
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary
        &serialized_runtime_metadata_artifact_reuse,
    const Objc3ImportedRuntimeMetadataSemanticRulesSummary
        &imported_runtime_metadata_semantic_rules,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
