static void AccumulateImportedRuntimeMetadataSourceRules(
    const Objc3ImportedRuntimeModuleSurface &surface,
    Objc3ImportedRuntimeMetadataSemanticRulesSummary &summary) {
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
}
