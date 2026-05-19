static void EmitObjc3IRRuntimePropertyDescriptorGlobal(
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimeMetadataPropertyBundle &bundle,
    const Objc3IRRuntimePropertyDescriptorSymbols &symbols,
    const Objc3IRRuntimePropertyImplementationSymbols &implementation_symbols,
    std::ostringstream &out, std::vector<std::string> &retained_globals) {
  out << symbols.descriptor_symbol
      << " = private global { ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i1, i1, i1, i1 } "
         "{ ptr "
      << symbols.property_name_symbol << ", ptr " << symbols.type_name_symbol
      << ", ptr " << symbols.owner_identity_symbol << ", ptr "
      << symbols.declaration_owner_identity_symbol << ", ptr "
      << symbols.export_owner_identity_symbol << ", ptr "
      << symbols.getter_symbol << ", ptr " << symbols.setter_symbol
      << ", ptr " << symbols.effective_getter_symbol << ", ptr "
      << symbols.effective_setter_symbol << ", ptr "
      << symbols.ivar_binding_symbol << ", ptr "
      << symbols.synthesized_binding_symbol << ", ptr "
      << symbols.ivar_layout_symbol << ", ptr "
      << symbols.property_attribute_profile_symbol << ", ptr "
      << symbols.ownership_lifetime_profile_symbol << ", ptr "
      << symbols.ownership_runtime_hook_profile_symbol << ", ptr "
      << symbols.accessor_ownership_profile_symbol << ", ptr "
      << implementation_symbols.getter_implementation_symbol << ", ptr "
      << implementation_symbols.setter_implementation_symbol << ", ptr "
      << symbols.ivar_layout_replay_key_symbol << ", i64 "
      << bundle.executable_ivar_layout_slot_index << ", i64 "
      << bundle.executable_ivar_layout_size_bytes << ", i64 "
      << bundle.executable_ivar_layout_alignment_bytes << ", i64 "
      << bundle.executable_ivar_layout_offset_bytes << ", i64 "
      << bundle.executable_ivar_layout_padding_bytes << ", i64 "
      << bundle.executable_ivar_layout_inherited_slot_count << ", i64 "
      << bundle.executable_ivar_layout_inherited_size_bytes << ", i64 "
      << bundle.executable_ivar_layout_owner_size_bytes << ", i64 "
      << bundle.executable_ivar_init_order_index << ", i64 "
      << bundle.executable_ivar_destroy_order_index << ", i1 "
      << (bundle.executable_ivar_layout_valid ? 1 : 0) << ", i1 "
      << (bundle.has_getter ? 1 : 0) << ", i1 "
      << (bundle.has_setter ? 1 : 0) << ", i1 "
      << (bundle.effective_setter_available ? 1 : 0) << " }, section \""
      << family.emitted_section_name << "\", align 8\n";
  RetainObjc3IRRuntimeMemberMetadataGlobal(retained_globals,
                                           symbols.descriptor_symbol);
}

static bool EmitObjc3IRRuntimePropertyDescriptorRecords(
    const Objc3IRRuntimeMemberMetadataEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::ostringstream &out, std::vector<std::string> &retained_globals,
    Objc3IRRuntimePropertyDescriptorSectionState &state, std::string &error) {
  const Objc3IRFrontendMetadata &frontend_metadata =
      options.frontend_metadata;
  const Objc3RuntimeMetadataLayoutPolicy &layout_policy =
      options.layout_policy;

  for (std::size_t i = 0;
       i < frontend_metadata.runtime_metadata_property_bundles_lexicographic
               .size();
       ++i) {
    const auto &bundle =
        frontend_metadata.runtime_metadata_property_bundles_lexicographic[i];
    const Objc3IRRuntimePropertyDescriptorSymbols symbols =
        BuildObjc3IRRuntimePropertyDescriptorSymbols(layout_policy, family,
                                                     bundle, i);
    Objc3IRRuntimePropertyImplementationSymbols implementation_symbols;

    state.descriptor_symbols.push_back(symbols.descriptor_symbol);
    EmitObjc3IRRuntimePropertyDescriptorConstants(family, bundle, symbols, out);
    if (!ResolveObjc3IRRuntimePropertyImplementationSymbols(
            options.implementation_method_symbols_by_owner_identity, bundle,
            implementation_symbols, error)) {
      return false;
    }
    EmitObjc3IRRuntimePropertyDescriptorGlobal(
        family, bundle, symbols, implementation_symbols, out, retained_globals);
  }
  return true;
}
