struct Objc3IRRuntimePropertyDescriptorSectionState {
  std::vector<std::string> descriptor_symbols;
};

struct Objc3IRRuntimePropertyDescriptorSymbols {
  std::string descriptor_symbol;
  std::string property_name_symbol;
  std::string type_name_symbol;
  std::string owner_identity_symbol;
  std::string declaration_owner_identity_symbol;
  std::string export_owner_identity_symbol;
  std::string getter_symbol = "null";
  std::string setter_symbol = "null";
  std::string effective_getter_symbol = "null";
  std::string effective_setter_symbol = "null";
  std::string ivar_binding_symbol = "null";
  std::string synthesized_binding_symbol = "null";
  std::string ivar_layout_symbol = "null";
  std::string ivar_layout_replay_key_symbol = "null";
  std::string property_attribute_profile_symbol = "null";
  std::string ownership_lifetime_profile_symbol = "null";
  std::string ownership_runtime_hook_profile_symbol = "null";
  std::string accessor_ownership_profile_symbol = "null";
};

struct Objc3IRRuntimePropertyImplementationSymbols {
  std::string getter_implementation_symbol = "null";
  std::string setter_implementation_symbol = "null";
};

static Objc3IRRuntimePropertyDescriptorSymbols
BuildObjc3IRRuntimePropertyDescriptorSymbols(
    const Objc3RuntimeMetadataLayoutPolicy &layout_policy,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimeMetadataPropertyBundle &bundle, std::size_t index) {
  Objc3IRRuntimePropertyDescriptorSymbols symbols;
  symbols.descriptor_symbol =
      BuildObjc3IRRuntimeMemberMetadataDescriptorSymbol(layout_policy, family,
                                                        index);
  symbols.property_name_symbol =
      BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(layout_policy, family,
                                                       "name", index);
  symbols.type_name_symbol = BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
      layout_policy, family, "type", index);
  symbols.owner_identity_symbol =
      BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
          layout_policy, family, "owner_identity", index);
  symbols.declaration_owner_identity_symbol =
      BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
          layout_policy, family, "declaration_owner_identity", index);
  symbols.export_owner_identity_symbol =
      BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
          layout_policy, family, "export_owner_identity", index);
  if (bundle.has_getter) {
    symbols.getter_symbol = BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
        layout_policy, family, "getter_selector", index);
  }
  if (bundle.has_setter) {
    symbols.setter_symbol = BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
        layout_policy, family, "setter_selector", index);
  }
  if (!bundle.effective_getter_selector.empty()) {
    symbols.effective_getter_symbol =
        BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
            layout_policy, family, "effective_getter_selector", index);
  }
  if (bundle.effective_setter_available) {
    symbols.effective_setter_symbol =
        BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
            layout_policy, family, "effective_setter_selector", index);
  }
  if (!bundle.ivar_binding_symbol.empty()) {
    symbols.ivar_binding_symbol =
        BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
            layout_policy, family, "ivar_binding", index);
  }
  if (!bundle.executable_synthesized_binding_symbol.empty()) {
    symbols.synthesized_binding_symbol =
        BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
            layout_policy, family, "synthesized_binding", index);
  }
  if (!bundle.executable_ivar_layout_symbol.empty()) {
    symbols.ivar_layout_symbol =
        BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
            layout_policy, family, "ivar_layout", index);
  }
  if (!bundle.executable_ivar_layout_replay_key.empty()) {
    symbols.ivar_layout_replay_key_symbol =
        BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
            layout_policy, family, "ivar_layout_replay", index);
  }
  if (!bundle.property_attribute_profile.empty()) {
    symbols.property_attribute_profile_symbol =
        BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
            layout_policy, family, "property_attribute_profile", index);
  }
  if (!bundle.ownership_lifetime_profile.empty()) {
    symbols.ownership_lifetime_profile_symbol =
        BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
            layout_policy, family, "ownership_lifetime_profile", index);
  }
  if (!bundle.ownership_runtime_hook_profile.empty()) {
    symbols.ownership_runtime_hook_profile_symbol =
        BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
            layout_policy, family, "ownership_runtime_hook_profile", index);
  }
  if (!bundle.accessor_ownership_profile.empty()) {
    symbols.accessor_ownership_profile_symbol =
        BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
            layout_policy, family, "accessor_ownership_profile", index);
  }
  return symbols;
}
