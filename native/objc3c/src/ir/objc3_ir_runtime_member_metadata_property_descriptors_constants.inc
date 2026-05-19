static void EmitObjc3IRRuntimePropertyCStringConstant(
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const std::string &symbol, const std::string &value,
    std::ostringstream &out) {
  out << symbol << " = private constant [" << (value.size() + 1u)
      << " x i8] c\"" << EscapeCStringLiteral(value)
      << "\\00\", section \"" << family.emitted_section_name
      << "\", align 1\n";
}

static void EmitObjc3IRRuntimePropertyIdentityConstants(
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimeMetadataPropertyBundle &bundle,
    const Objc3IRRuntimePropertyDescriptorSymbols &symbols,
    std::ostringstream &out) {
  EmitObjc3IRRuntimePropertyCStringConstant(
      family, symbols.property_name_symbol, bundle.property_name, out);
  EmitObjc3IRRuntimePropertyCStringConstant(family, symbols.type_name_symbol,
                                            bundle.type_name, out);
  EmitObjc3IRRuntimePropertyCStringConstant(
      family, symbols.owner_identity_symbol, bundle.owner_identity, out);
  EmitObjc3IRRuntimePropertyCStringConstant(
      family, symbols.declaration_owner_identity_symbol,
      bundle.declaration_owner_identity, out);
  EmitObjc3IRRuntimePropertyCStringConstant(
      family, symbols.export_owner_identity_symbol, bundle.export_owner_identity,
      out);
}

static void EmitObjc3IRRuntimePropertyAccessorConstants(
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimeMetadataPropertyBundle &bundle,
    const Objc3IRRuntimePropertyDescriptorSymbols &symbols,
    std::ostringstream &out) {
  if (bundle.has_getter) {
    EmitObjc3IRRuntimePropertyCStringConstant(
        family, symbols.getter_symbol, bundle.getter_selector, out);
  }
  if (bundle.has_setter) {
    EmitObjc3IRRuntimePropertyCStringConstant(
        family, symbols.setter_symbol, bundle.setter_selector, out);
  }
  if (!bundle.effective_getter_selector.empty()) {
    EmitObjc3IRRuntimePropertyCStringConstant(
        family, symbols.effective_getter_symbol,
        bundle.effective_getter_selector, out);
  }
  if (bundle.effective_setter_available) {
    EmitObjc3IRRuntimePropertyCStringConstant(
        family, symbols.effective_setter_symbol,
        bundle.effective_setter_selector, out);
  }
}

static void EmitObjc3IRRuntimePropertyStorageConstants(
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimeMetadataPropertyBundle &bundle,
    const Objc3IRRuntimePropertyDescriptorSymbols &symbols,
    std::ostringstream &out) {
  if (!bundle.ivar_binding_symbol.empty()) {
    EmitObjc3IRRuntimePropertyCStringConstant(
        family, symbols.ivar_binding_symbol, bundle.ivar_binding_symbol, out);
  }
  if (!bundle.executable_synthesized_binding_symbol.empty()) {
    EmitObjc3IRRuntimePropertyCStringConstant(
        family, symbols.synthesized_binding_symbol,
        bundle.executable_synthesized_binding_symbol, out);
  }
  if (!bundle.executable_ivar_layout_symbol.empty()) {
    EmitObjc3IRRuntimePropertyCStringConstant(
        family, symbols.ivar_layout_symbol,
        bundle.executable_ivar_layout_symbol, out);
  }
  if (!bundle.executable_ivar_layout_replay_key.empty()) {
    EmitObjc3IRRuntimePropertyCStringConstant(
        family, symbols.ivar_layout_replay_key_symbol,
        bundle.executable_ivar_layout_replay_key, out);
  }
}

static void EmitObjc3IRRuntimePropertyProfileConstants(
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimeMetadataPropertyBundle &bundle,
    const Objc3IRRuntimePropertyDescriptorSymbols &symbols,
    std::ostringstream &out) {
  if (!bundle.property_attribute_profile.empty()) {
    EmitObjc3IRRuntimePropertyCStringConstant(
        family, symbols.property_attribute_profile_symbol,
        bundle.property_attribute_profile, out);
  }
  if (!bundle.ownership_lifetime_profile.empty()) {
    EmitObjc3IRRuntimePropertyCStringConstant(
        family, symbols.ownership_lifetime_profile_symbol,
        bundle.ownership_lifetime_profile, out);
  }
  if (!bundle.ownership_runtime_hook_profile.empty()) {
    EmitObjc3IRRuntimePropertyCStringConstant(
        family, symbols.ownership_runtime_hook_profile_symbol,
        bundle.ownership_runtime_hook_profile, out);
  }
  if (!bundle.accessor_ownership_profile.empty()) {
    EmitObjc3IRRuntimePropertyCStringConstant(
        family, symbols.accessor_ownership_profile_symbol,
        bundle.accessor_ownership_profile, out);
  }
}

static void EmitObjc3IRRuntimePropertyDescriptorConstants(
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimeMetadataPropertyBundle &bundle,
    const Objc3IRRuntimePropertyDescriptorSymbols &symbols,
    std::ostringstream &out) {
  EmitObjc3IRRuntimePropertyIdentityConstants(family, bundle, symbols, out);
  EmitObjc3IRRuntimePropertyAccessorConstants(family, bundle, symbols, out);
  EmitObjc3IRRuntimePropertyStorageConstants(family, bundle, symbols, out);
  EmitObjc3IRRuntimePropertyProfileConstants(family, bundle, symbols, out);
}
