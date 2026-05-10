    method.unsafe_keyword_sites = profile.unsafe_keyword_sites;
    method.pointer_arithmetic_sites = profile.pointer_arithmetic_sites;
    method.raw_pointer_type_sites = profile.raw_pointer_type_sites;
    method.unsafe_operation_sites = profile.unsafe_operation_sites;
    method.unsafe_pointer_extension_normalized_sites = profile.normalized_sites;
    method.unsafe_pointer_extension_gate_blocked_sites = profile.gate_blocked_sites;
    method.unsafe_pointer_extension_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_unsafe_pointer_extension_handoff =
        profile.deterministic_unsafe_pointer_extension_handoff;
    method.unsafe_pointer_extension_profile = BuildUnsafePointerExtensionProfile(
        method.unsafe_pointer_extension_sites,
        method.unsafe_keyword_sites,
        method.pointer_arithmetic_sites,
        method.raw_pointer_type_sites,
        method.unsafe_operation_sites,
        method.unsafe_pointer_extension_normalized_sites,
        method.unsafe_pointer_extension_gate_blocked_sites,
        method.unsafe_pointer_extension_contract_violation_sites,
        method.deterministic_unsafe_pointer_extension_handoff);
    method.unsafe_pointer_extension_profile_is_normalized =
        IsUnsafePointerExtensionProfileNormalized(
            method.unsafe_pointer_extension_sites,
            method.unsafe_keyword_sites,
            method.pointer_arithmetic_sites,
            method.raw_pointer_type_sites,
            method.unsafe_operation_sites,
            method.unsafe_pointer_extension_normalized_sites,
            method.unsafe_pointer_extension_gate_blocked_sites,
            method.unsafe_pointer_extension_contract_violation_sites);
  }

  void FinalizeInlineAsmIntrinsicGovernanceProfile(FunctionDecl &fn) {
    FinalizeObjc3ParserCoreInlineAsmIntrinsicGovernanceProfile(fn);
  }

  void FinalizeInlineAsmIntrinsicGovernanceProfile(Objc3MethodDecl &method) {
    FinalizeObjc3ParserCoreInlineAsmIntrinsicGovernanceProfile(method);
  }

  void AssignObjcMethodLookupOverrideConflictSymbols(Objc3MethodDecl &method,
                                                     const std::string &lookup_owner_symbol,
                                                     const std::string &override_owner_symbol) {
    method.method_lookup_symbol = lookup_owner_symbol + "::" + BuildObjcMethodLookupSymbol(method);
    method.override_lookup_symbol = override_owner_symbol + "::" + BuildObjcMethodOverrideLookupSymbol(method);
    method.conflict_lookup_symbol = BuildObjcMethodConflictLookupSymbol(method);
  }

  void FinalizeObjcMethodLookupOverrideConflictPackets(
      const std::vector<Objc3MethodDecl> &methods,
      std::vector<std::string> &method_lookup_symbols_lexicographic,
      std::vector<std::string> &override_lookup_symbols_lexicographic,
      std::vector<std::string> &conflict_lookup_symbols_lexicographic) {
    method_lookup_symbols_lexicographic = BuildObjcMethodLookupSymbolsLexicographic(methods);
    override_lookup_symbols_lexicographic = BuildObjcMethodOverrideLookupSymbolsLexicographic(methods);
    conflict_lookup_symbols_lexicographic = BuildObjcMethodConflictLookupSymbolsLexicographic(methods);
  }

#include "parse/objc3_parser_core_objc_property_ivar_layout_finalizers.inc"

#include "parse/objc3_parser_core_draft_syntax_surface_finalizers.inc"

  std::string BuildExecutablePropertyAttributeProfile(
      const Objc3PropertyDecl &property) {
    std::vector<std::string> attributes;
    attributes.reserve(property.attributes.size());
    for (const auto &attribute : property.attributes) {
      std::string rendered = attribute.name;
      if (attribute.has_value) {
        rendered += "=" + TrimAsciiWhitespaceCopy(attribute.value);
      }
      attributes.push_back(std::move(rendered));
    }
    std::sort(attributes.begin(), attributes.end());

    std::ostringstream out;
    out << "readonly=" << (property.is_readonly ? 1 : 0)
        << ";readwrite=" << (property.is_readwrite ? 1 : 0)
        << ";atomic=" << (property.is_atomic ? 1 : 0)
        << ";nonatomic=" << (property.is_nonatomic ? 1 : 0)
        << ";copy=" << (property.is_copy ? 1 : 0)
        << ";retain=" << (property.is_retain ? 1 : 0)
        << ";strong=" << (property.is_strong ? 1 : 0)
        << ";weak=" << (property.is_weak ? 1 : 0)
        << ";unowned=" << (property.is_unowned ? 1 : 0)
        << ";unsafe_unretained=" << (property.is_unsafe_unretained ? 1 : 0)
        << ";assign=" << (property.is_assign ? 1 : 0)
        << ";nullable=" << (property.is_nullable ? 1 : 0)
        << ";nonnull=" << (property.is_nonnull ? 1 : 0)
        << ";null_resettable=" << (property.is_null_resettable ? 1 : 0)
        << ";class=" << (property.is_class ? 1 : 0)
        << ";direct=" << (property.is_direct ? 1 : 0)
        << ";attributes=";
    for (std::size_t i = 0; i < attributes.size(); ++i) {
      if (i != 0u) {
        out << ",";
      }
      out << attributes[i];
    }
    return out.str();
  }

  std::string BuildExecutableAccessorOwnershipProfile(
      const Objc3PropertyDecl &property) {
    std::ostringstream out;
    out << "getter=" << property.effective_getter_selector
        << ";setter_available=" << (property.effective_setter_available ? 1 : 0)
        << ";setter=";
    if (property.effective_setter_available) {
      out << property.effective_setter_selector;
    } else {
      out << "<none>";
    }
    out << ";ownership_lifetime=" << property.ownership_lifetime_profile
        << ";runtime_hook=" << property.ownership_runtime_hook_profile;
    return out.str();
  }

  void AssignObjcExecutablePropertySourceModelFields(
      Objc3PropertyDecl &property, const std::string &storage_owner_symbol,
      std::size_t slot_index, bool supports_storage_layout) {
    property.property_attribute_profile =
        BuildExecutablePropertyAttributeProfile(property);
    property.effective_getter_selector =
        property.has_getter ? TrimAsciiWhitespaceCopy(property.getter_selector)
                            : property.name;
    property.effective_setter_available = !property.is_readonly;
    property.effective_setter_selector =
        property.effective_setter_available
            ? (property.has_setter
                   ? TrimAsciiWhitespaceCopy(property.setter_selector)
                   : BuildDefaultPropertySetterSelector(property.name))
            : std::string{};
    property.executable_synthesized_binding_kind =
        supports_storage_layout ? "implicit-ivar" : "none";
    property.executable_synthesized_binding_symbol =
        supports_storage_layout
            ? storage_owner_symbol + "::" + BuildObjcPropertySynthesisSymbol(property)
            : std::string{};
    if (supports_storage_layout) {
      const Objc3ExecutablePropertyLayoutShape layout_shape =
          ComputeExecutablePropertyLayoutShape(property);
      property.executable_ivar_layout_symbol =
          storage_owner_symbol + "::objc_property_layout:" + property.name;
      property.executable_ivar_layout_slot_index = slot_index;
      property.executable_ivar_layout_size_bytes = layout_shape.size_bytes;
      property.executable_ivar_layout_alignment_bytes =
          layout_shape.alignment_bytes;
      property.executable_ivar_layout_offset_bytes = 0;
      property.executable_ivar_layout_padding_bytes = 0;
      property.executable_ivar_layout_inherited_slot_count = 0;
      property.executable_ivar_layout_inherited_size_bytes = 0;
      property.executable_ivar_layout_owner_size_bytes = 0;
      property.executable_ivar_init_order_index = slot_index;
      property.executable_ivar_destroy_order_index = 0;
      property.executable_ivar_layout_valid = false;
      property.executable_ivar_layout_replay_key.clear();
    } else {
      property.executable_ivar_layout_symbol.clear();
      property.executable_ivar_layout_slot_index = 0;
      property.executable_ivar_layout_size_bytes = 0;
      property.executable_ivar_layout_alignment_bytes = 0;
      property.executable_ivar_layout_offset_bytes = 0;
      property.executable_ivar_layout_padding_bytes = 0;
      property.executable_ivar_layout_inherited_slot_count = 0;
      property.executable_ivar_layout_inherited_size_bytes = 0;
      property.executable_ivar_layout_owner_size_bytes = 0;
      property.executable_ivar_init_order_index = 0;
      property.executable_ivar_destroy_order_index = 0;
      property.executable_ivar_layout_valid = false;
      property.executable_ivar_layout_replay_key.clear();
    }
    property.accessor_ownership_profile =
        BuildExecutableAccessorOwnershipProfile(property);
  }

  void FinalizeObjcPropertySynthesisIvarBindingPackets(
      std::vector<Objc3PropertyDecl> &properties,
      std::vector<std::string> &property_synthesis_symbols_lexicographic,
      std::vector<std::string> &ivar_binding_symbols_lexicographic) {
    const std::size_t property_count = properties.size();
    for (auto &property : properties) {
      if (property.executable_ivar_layout_valid) {
        continue;
      }
      if (!property.executable_ivar_layout_symbol.empty()) {
        property.executable_ivar_init_order_index =
            property.executable_ivar_layout_slot_index;
        property.executable_ivar_destroy_order_index =
            property_count > property.executable_ivar_layout_slot_index
                ? (property_count - 1u) -
                      property.executable_ivar_layout_slot_index
                : 0u;
      } else {
        property.executable_ivar_init_order_index = 0;
        property.executable_ivar_destroy_order_index = 0;
      }
    }
    property_synthesis_symbols_lexicographic = BuildObjcPropertySynthesisSymbolsLexicographic(properties);
    ivar_binding_symbols_lexicographic = BuildObjcIvarBindingSymbolsLexicographic(properties);
  }

  void ConsumeBracedBodyTail() {
    int depth = 1;
    while (depth > 0 && !At(TokenKind::Eof)) {
      if (Match(TokenKind::LBrace)) {
        ++depth;
        continue;
      }
      if (Match(TokenKind::RBrace)) {
        --depth;
        continue;
      }
      Advance();
    }
  }

