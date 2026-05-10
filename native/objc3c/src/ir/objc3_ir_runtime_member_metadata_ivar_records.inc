struct Objc3IRRuntimeIvarDescriptorSectionState {
  std::vector<std::string> descriptor_symbols;
  std::map<std::string, std::vector<std::pair<std::size_t, std::string>>>
      descriptor_symbols_by_declaration_owner_identity;
  std::map<std::string, std::size_t> instance_size_by_owner_identity;
};

struct Objc3IRRuntimeIvarDescriptorSymbols {
  std::string descriptor_symbol;
  std::string owner_identity_symbol;
  std::string declaration_owner_identity_symbol;
  std::string export_owner_identity_symbol;
  std::string property_owner_identity_symbol;
  std::string property_name_symbol;
  std::string ivar_binding_symbol;
  std::string layout_symbol;
  std::string offset_global_symbol;
  std::string layout_record_symbol;
  std::string layout_replay_key_symbol;
};

static Objc3IRRuntimeIvarDescriptorSymbols
BuildObjc3IRRuntimeIvarDescriptorSymbols(
    const Objc3RuntimeMetadataLayoutPolicy &layout_policy,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimeMetadataIvarBundle &bundle, std::size_t index) {
  Objc3IRRuntimeIvarDescriptorSymbols symbols;
  symbols.descriptor_symbol =
      BuildObjc3IRRuntimeMemberMetadataDescriptorSymbol(layout_policy, family,
                                                        index);
  symbols.owner_identity_symbol =
      BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
          layout_policy, family, "owner_identity", index);
  symbols.declaration_owner_identity_symbol =
      BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
          layout_policy, family, "declaration_owner_identity", index);
  symbols.export_owner_identity_symbol =
      BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
          layout_policy, family, "export_owner_identity", index);
  symbols.property_owner_identity_symbol =
      BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
          layout_policy, family, "property_owner_identity", index);
  symbols.property_name_symbol =
      BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
          layout_policy, family, "property_name", index);
  symbols.ivar_binding_symbol =
      BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
          layout_policy, family, "binding", index);
  symbols.layout_symbol =
      bundle.executable_ivar_layout_symbol.empty()
          ? std::string{"null"}
          : BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
                layout_policy, family, "layout_symbol", index);
  symbols.offset_global_symbol =
      BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(layout_policy, family,
                                                       "offset", index);
  symbols.layout_record_symbol =
      BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
          layout_policy, family, "layout_record", index);
  symbols.layout_replay_key_symbol =
      bundle.executable_ivar_layout_replay_key.empty()
          ? std::string{"null"}
          : BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
                layout_policy, family, "layout_replay", index);
  return symbols;
}

static void RecordObjc3IRRuntimeIvarDescriptorSymbol(
    const Objc3IRRuntimeMetadataIvarBundle &bundle,
    const Objc3IRRuntimeIvarDescriptorSymbols &symbols,
    Objc3IRRuntimeIvarDescriptorSectionState &state) {
  state.descriptor_symbols.push_back(symbols.descriptor_symbol);
  state.descriptor_symbols_by_declaration_owner_identity
      [bundle.declaration_owner_identity]
          .push_back(std::make_pair(bundle.executable_ivar_layout_slot_index,
                                    symbols.descriptor_symbol));
  state.instance_size_by_owner_identity[bundle.declaration_owner_identity] =
      std::max(
          state.instance_size_by_owner_identity[bundle.declaration_owner_identity],
          bundle.executable_ivar_layout_owner_size_bytes);
}

static void EmitObjc3IRRuntimeIvarIdentityConstants(
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimeMetadataIvarBundle &bundle,
    const Objc3IRRuntimeIvarDescriptorSymbols &symbols,
    std::ostringstream &out) {
  out << symbols.owner_identity_symbol << " = private constant ["
      << (bundle.owner_identity.size() + 1u) << " x i8] c\""
      << EscapeCStringLiteral(bundle.owner_identity) << "\\00\", section \""
      << family.emitted_section_name << "\", align 1\n";
  out << symbols.declaration_owner_identity_symbol
      << " = private constant ["
      << (bundle.declaration_owner_identity.size() + 1u) << " x i8] c\""
      << EscapeCStringLiteral(bundle.declaration_owner_identity)
      << "\\00\", section \"" << family.emitted_section_name
      << "\", align 1\n";
  out << symbols.export_owner_identity_symbol << " = private constant ["
      << (bundle.export_owner_identity.size() + 1u) << " x i8] c\""
      << EscapeCStringLiteral(bundle.export_owner_identity)
      << "\\00\", section \"" << family.emitted_section_name
      << "\", align 1\n";
  out << symbols.property_owner_identity_symbol << " = private constant ["
      << (bundle.property_owner_identity.size() + 1u) << " x i8] c\""
      << EscapeCStringLiteral(bundle.property_owner_identity)
      << "\\00\", section \"" << family.emitted_section_name
      << "\", align 1\n";
  out << symbols.property_name_symbol << " = private constant ["
      << (bundle.property_name.size() + 1u) << " x i8] c\""
      << EscapeCStringLiteral(bundle.property_name) << "\\00\", section \""
      << family.emitted_section_name << "\", align 1\n";
  out << symbols.ivar_binding_symbol << " = private constant ["
      << (bundle.ivar_binding_symbol.size() + 1u) << " x i8] c\""
      << EscapeCStringLiteral(bundle.ivar_binding_symbol)
      << "\\00\", section \"" << family.emitted_section_name
      << "\", align 1\n";
}

static void EmitObjc3IRRuntimeIvarLayoutConstants(
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimeMetadataIvarBundle &bundle,
    const Objc3IRRuntimeIvarDescriptorSymbols &symbols,
    std::ostringstream &out) {
  if (!bundle.executable_ivar_layout_symbol.empty()) {
    out << symbols.layout_symbol << " = private constant ["
        << (bundle.executable_ivar_layout_symbol.size() + 1u)
        << " x i8] c\""
        << EscapeCStringLiteral(bundle.executable_ivar_layout_symbol)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
  }
  if (!bundle.executable_ivar_layout_replay_key.empty()) {
    out << symbols.layout_replay_key_symbol << " = private constant ["
        << (bundle.executable_ivar_layout_replay_key.size() + 1u)
        << " x i8] c\""
        << EscapeCStringLiteral(bundle.executable_ivar_layout_replay_key)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
  }
}

static void EmitObjc3IRRuntimeIvarLayoutGlobals(
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimeMetadataIvarBundle &bundle,
    const Objc3IRRuntimeIvarDescriptorSymbols &symbols,
    std::ostringstream &out, std::vector<std::string> &retained_globals) {
  out << symbols.offset_global_symbol << " = private global i64 "
      << bundle.executable_ivar_layout_offset_bytes << ", section \""
      << family.emitted_section_name << "\", align 8\n";
  RetainObjc3IRRuntimeMemberMetadataGlobal(retained_globals,
                                           symbols.offset_global_symbol);
  out << symbols.layout_record_symbol
      << " = private global { ptr, ptr, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i1 } { ptr "
      << symbols.layout_symbol << ", ptr " << symbols.layout_replay_key_symbol
      << ", i64 " << bundle.executable_ivar_layout_slot_index << ", i64 "
      << bundle.executable_ivar_layout_offset_bytes << ", i64 "
      << bundle.executable_ivar_layout_size_bytes << ", i64 "
      << bundle.executable_ivar_layout_alignment_bytes << ", i64 "
      << bundle.executable_ivar_layout_padding_bytes << ", i64 "
      << bundle.executable_ivar_layout_inherited_slot_count << ", i64 "
      << bundle.executable_ivar_layout_inherited_size_bytes << ", i64 "
      << bundle.executable_ivar_layout_owner_size_bytes << ", i64 "
      << bundle.executable_ivar_init_order_index << ", i64 "
      << bundle.executable_ivar_destroy_order_index << ", i1 "
      << (bundle.executable_ivar_layout_valid ? 1 : 0) << " }, section \""
      << family.emitted_section_name << "\", align 8\n";
  RetainObjc3IRRuntimeMemberMetadataGlobal(retained_globals,
                                           symbols.layout_record_symbol);
}

static void EmitObjc3IRRuntimeIvarDescriptorGlobal(
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimeMetadataIvarBundle &bundle,
    const Objc3IRRuntimeIvarDescriptorSymbols &symbols,
    std::ostringstream &out, std::vector<std::string> &retained_globals) {
  out << symbols.descriptor_symbol
      << " = private global { ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i1 } { ptr "
      << symbols.owner_identity_symbol << ", ptr "
      << symbols.declaration_owner_identity_symbol << ", ptr "
      << symbols.export_owner_identity_symbol << ", ptr "
      << symbols.property_owner_identity_symbol << ", ptr "
      << symbols.property_name_symbol << ", ptr " << symbols.ivar_binding_symbol
      << ", ptr " << symbols.layout_record_symbol << ", ptr "
      << symbols.offset_global_symbol << ", ptr "
      << symbols.layout_replay_key_symbol << ", i64 "
      << bundle.executable_ivar_layout_slot_index << ", i64 "
      << bundle.executable_ivar_layout_offset_bytes << ", i64 "
      << bundle.executable_ivar_layout_size_bytes << ", i64 "
      << bundle.executable_ivar_layout_alignment_bytes << ", i64 "
      << bundle.executable_ivar_layout_padding_bytes << ", i64 "
      << bundle.executable_ivar_layout_inherited_slot_count << ", i64 "
      << bundle.executable_ivar_layout_inherited_size_bytes << ", i64 "
      << bundle.executable_ivar_layout_owner_size_bytes << ", i64 "
      << bundle.executable_ivar_init_order_index << ", i64 "
      << bundle.executable_ivar_destroy_order_index << ", i1 "
      << (bundle.executable_ivar_layout_valid ? 1 : 0) << " }, section \""
      << family.emitted_section_name << "\", align 8\n";
  RetainObjc3IRRuntimeMemberMetadataGlobal(retained_globals,
                                           symbols.descriptor_symbol);
}

static void EmitObjc3IRRuntimeIvarDescriptorRecords(
    const Objc3IRRuntimeMemberMetadataEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::ostringstream &out, std::vector<std::string> &retained_globals,
    Objc3IRRuntimeIvarDescriptorSectionState &state) {
  const Objc3IRFrontendMetadata &frontend_metadata =
      options.frontend_metadata;
  const Objc3RuntimeMetadataLayoutPolicy &layout_policy =
      options.layout_policy;
  for (std::size_t i = 0;
       i < frontend_metadata.runtime_metadata_ivar_bundles_lexicographic.size();
       ++i) {
    const auto &bundle =
        frontend_metadata.runtime_metadata_ivar_bundles_lexicographic[i];
    const Objc3IRRuntimeIvarDescriptorSymbols symbols =
        BuildObjc3IRRuntimeIvarDescriptorSymbols(layout_policy, family, bundle,
                                                 i);
    RecordObjc3IRRuntimeIvarDescriptorSymbol(bundle, symbols, state);
    EmitObjc3IRRuntimeIvarIdentityConstants(family, bundle, symbols, out);
    EmitObjc3IRRuntimeIvarLayoutConstants(family, bundle, symbols, out);
    EmitObjc3IRRuntimeIvarLayoutGlobals(family, bundle, symbols, out,
                                        retained_globals);
    EmitObjc3IRRuntimeIvarDescriptorGlobal(family, bundle, symbols, out,
                                           retained_globals);
  }
}
