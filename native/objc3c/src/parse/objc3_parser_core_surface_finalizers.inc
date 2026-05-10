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

  void AssignObjcPropertySynthesisIvarBindingSymbols(Objc3PropertyDecl &property,
                                                     const std::string &synthesis_owner_symbol) {
    property.property_synthesis_symbol = synthesis_owner_symbol + "::" + BuildObjcPropertySynthesisSymbol(property);
    property.ivar_binding_symbol = synthesis_owner_symbol + "::" + BuildObjcIvarBindingSymbol(property);
  }

  struct Objc3ExecutablePropertyLayoutShape {
    std::size_t size_bytes = 0;
    std::size_t alignment_bytes = 1;
  };

  std::string TrimAsciiWhitespaceCopy(const std::string &text) {
    std::size_t start = 0;
    while (start < text.size() &&
           std::isspace(static_cast<unsigned char>(text[start])) != 0) {
      ++start;
    }
    std::size_t end = text.size();
    while (end > start &&
           std::isspace(static_cast<unsigned char>(text[end - 1])) != 0) {
      --end;
    }
    return text.substr(start, end - start);
  }

  std::string BuildDefaultPropertySetterSelector(const std::string &property_name) {
    if (property_name.empty()) {
      return {};
    }
    std::string selector = "set";
    selector.push_back(static_cast<char>(
        std::toupper(static_cast<unsigned char>(property_name.front()))));
    selector.append(property_name.substr(1));
    selector.push_back(':');
    return selector;
  }

  Objc3ExecutablePropertyLayoutShape ComputeExecutablePropertyLayoutShape(
      const Objc3PropertyDecl &property) {
    const auto scalar_layout = [&](std::size_t size_bytes,
                                   std::size_t alignment_bytes) {
      return Objc3ExecutablePropertyLayoutShape{size_bytes, alignment_bytes};
    };
    if (property.vector_spelling) {
      const std::size_t lane_width =
          property.type == ValueType::Bool ? 1u : 4u;
      const std::size_t size_bytes =
          lane_width * static_cast<std::size_t>(property.vector_lane_count);
      return scalar_layout(size_bytes, std::min<std::size_t>(size_bytes, 16u));
    }
    switch (property.type) {
    case ValueType::Bool:
      return scalar_layout(1u, 1u);
    case ValueType::I32:
      return scalar_layout(4u, 4u);
    case ValueType::Void:
      return scalar_layout(0u, 1u);
    case ValueType::ObjCId:
    case ValueType::ObjCClass:
    case ValueType::ObjCSel:
    case ValueType::ObjCProtocol:
    case ValueType::ObjCInstancetype:
    case ValueType::ObjCObjectPtr:
    case ValueType::Function:
    case ValueType::Unknown:
    default:
      return scalar_layout(8u, 8u);
    }
  }

  std::size_t AlignObjcIvarLayoutOffset(std::size_t offset,
                                        std::size_t alignment) const {
    const std::size_t effective_alignment =
        std::max<std::size_t>(alignment, 1u);
    const std::size_t remainder = offset % effective_alignment;
    return remainder == 0u ? offset
                           : offset + (effective_alignment - remainder);
  }

  bool IsValidObjcIvarLayoutAlignment(std::size_t alignment) const {
    return alignment != 0u && (alignment & (alignment - 1u)) == 0u;
  }

  std::string BuildObjcIvarLayoutReplayKey(
      const Objc3PropertyDecl &property, const Objc3InterfaceDecl &owner) const {
    std::ostringstream out;
    out << "owner=" << owner.semantic_link_symbol
        << ";property=" << property.name
        << ";slot=" << property.executable_ivar_layout_slot_index
        << ";offset=" << property.executable_ivar_layout_offset_bytes
        << ";size=" << property.executable_ivar_layout_size_bytes
        << ";alignment=" << property.executable_ivar_layout_alignment_bytes
        << ";padding=" << property.executable_ivar_layout_padding_bytes
        << ";inherited_slots="
        << property.executable_ivar_layout_inherited_slot_count
        << ";inherited_size="
        << property.executable_ivar_layout_inherited_size_bytes
        << ";owner_size=" << property.executable_ivar_layout_owner_size_bytes;
    return out.str();
  }

  void ClearObjcPropertyIvarLayoutClosure(Objc3PropertyDecl &property) {
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

  Objc3InterfaceDecl *FindObjcInterfaceLayoutOwner(
      Objc3ParsedProgram &program, const std::string &name) const {
    if (name.empty()) {
      return nullptr;
    }
    for (auto &decl : MutableObjc3ParsedProgramAst(program).interfaces) {
      if (!decl.has_category && decl.name == name) {
        return &decl;
      }
    }
    return nullptr;
  }

  Objc3PropertyDecl *FindObjcPropertyDeclByName(
      std::vector<Objc3PropertyDecl> &properties,
      const std::string &name) const {
    for (auto &property : properties) {
      if (property.name == name) {
        return &property;
      }
    }
    return nullptr;
  }

  void CopyObjcIvarLayoutClosure(Objc3PropertyDecl &target,
                                 const Objc3PropertyDecl &source) {
    target.executable_ivar_layout_symbol =
        source.executable_ivar_layout_symbol;
    target.executable_ivar_layout_slot_index =
        source.executable_ivar_layout_slot_index;
    target.executable_ivar_layout_size_bytes =
        source.executable_ivar_layout_size_bytes;
    target.executable_ivar_layout_alignment_bytes =
        source.executable_ivar_layout_alignment_bytes;
    target.executable_ivar_layout_offset_bytes =
        source.executable_ivar_layout_offset_bytes;
    target.executable_ivar_layout_padding_bytes =
        source.executable_ivar_layout_padding_bytes;
    target.executable_ivar_layout_inherited_slot_count =
        source.executable_ivar_layout_inherited_slot_count;
    target.executable_ivar_layout_inherited_size_bytes =
        source.executable_ivar_layout_inherited_size_bytes;
    target.executable_ivar_layout_owner_size_bytes =
        source.executable_ivar_layout_owner_size_bytes;
    target.executable_ivar_init_order_index =
        source.executable_ivar_init_order_index;
    target.executable_ivar_destroy_order_index =
        source.executable_ivar_destroy_order_index;
    target.executable_ivar_layout_valid =
        source.executable_ivar_layout_valid;
    target.executable_ivar_layout_replay_key =
        source.executable_ivar_layout_replay_key;
  }

  bool FinalizeObjcInterfacePropertyIvarLayoutClosure(
      Objc3ParsedProgram &program, Objc3InterfaceDecl &decl,
      std::unordered_set<std::string> &visiting,
      std::unordered_set<std::string> &completed) {
    if (decl.has_category) {
      for (auto &property : decl.properties) {
        ClearObjcPropertyIvarLayoutClosure(property);
      }
      FinalizeObjcPropertySynthesisIvarBindingPackets(
          decl.properties, decl.property_synthesis_symbols_lexicographic,
          decl.ivar_binding_symbols_lexicographic);
      return true;
    }

    if (completed.count(decl.name) != 0u) {
      return true;
    }
    if (visiting.count(decl.name) != 0u) {
      diagnostics_.push_back(MakeDiag(
          decl.line, decl.column, "O3P150",
          "cyclic Objective-C interface inheritance cannot produce a stable ivar layout"));
      for (auto &property : decl.properties) {
        ClearObjcPropertyIvarLayoutClosure(property);
      }
      completed.insert(decl.name);
      return false;
    }

    visiting.insert(decl.name);
    std::size_t inherited_slot_count = 0u;
    std::size_t inherited_size_bytes = 0u;
    if (!decl.super_name.empty()) {
      Objc3InterfaceDecl *super_decl =
          FindObjcInterfaceLayoutOwner(program, decl.super_name);
      if (super_decl != nullptr) {
        FinalizeObjcInterfacePropertyIvarLayoutClosure(
            program, *super_decl, visiting, completed);
        for (const auto &property : super_decl->properties) {
          if (property.executable_ivar_layout_valid) {
            inherited_slot_count = std::max(
                inherited_slot_count,
                property.executable_ivar_layout_slot_index + 1u);
            inherited_size_bytes = std::max(
                inherited_size_bytes,
                property.executable_ivar_layout_owner_size_bytes);
          }
        }
      }
    }

    std::size_t running_slot = inherited_slot_count;
    std::size_t running_offset = inherited_size_bytes;
    std::size_t max_alignment = 1u;
    bool layout_valid = true;
    for (auto &property : decl.properties) {
      if (property.executable_ivar_layout_symbol.empty()) {
        ClearObjcPropertyIvarLayoutClosure(property);
        continue;
      }
      if (property.executable_ivar_layout_size_bytes == 0u ||
          !IsValidObjcIvarLayoutAlignment(
              property.executable_ivar_layout_alignment_bytes)) {
        diagnostics_.push_back(MakeDiag(
            property.line, property.column, "O3P151",
            "invalid Objective-C property ivar layout size or alignment"));
        ClearObjcPropertyIvarLayoutClosure(property);
        layout_valid = false;
        continue;
      }

      const std::size_t aligned_offset = AlignObjcIvarLayoutOffset(
          running_offset, property.executable_ivar_layout_alignment_bytes);
      if (aligned_offset < running_offset ||
          aligned_offset >
              std::numeric_limits<std::size_t>::max() -
                  property.executable_ivar_layout_size_bytes) {
        diagnostics_.push_back(MakeDiag(
            property.line, property.column, "O3P152",
            "Objective-C property ivar layout offset overflow"));
        ClearObjcPropertyIvarLayoutClosure(property);
        layout_valid = false;
        continue;
      }

      property.executable_ivar_layout_slot_index = running_slot;
      property.executable_ivar_layout_offset_bytes = aligned_offset;
      property.executable_ivar_layout_padding_bytes =
          aligned_offset - running_offset;
      property.executable_ivar_layout_inherited_slot_count =
          inherited_slot_count;
      property.executable_ivar_layout_inherited_size_bytes =
          inherited_size_bytes;
      property.executable_ivar_init_order_index = running_slot;
      property.executable_ivar_layout_valid = true;
      running_offset =
          aligned_offset + property.executable_ivar_layout_size_bytes;
      max_alignment =
          std::max(max_alignment,
                   property.executable_ivar_layout_alignment_bytes);
      ++running_slot;
    }

    const std::size_t owner_size =
        AlignObjcIvarLayoutOffset(running_offset, max_alignment);
    for (auto &property : decl.properties) {
      if (!property.executable_ivar_layout_valid) {
        continue;
      }
      property.executable_ivar_layout_owner_size_bytes = owner_size;
      property.executable_ivar_destroy_order_index =
          running_slot > property.executable_ivar_layout_slot_index
              ? (running_slot - 1u) -
                    property.executable_ivar_layout_slot_index
              : 0u;
      property.executable_ivar_layout_replay_key =
          BuildObjcIvarLayoutReplayKey(property, decl);
    }

    FinalizeObjcPropertySynthesisIvarBindingPackets(
        decl.properties, decl.property_synthesis_symbols_lexicographic,
        decl.ivar_binding_symbols_lexicographic);
    visiting.erase(decl.name);
    completed.insert(decl.name);
    return layout_valid;
  }

  void FinalizeObjcProgramPropertyIvarLayoutClosure(
      Objc3ParsedProgram &program) {
    std::unordered_set<std::string> visiting;
    std::unordered_set<std::string> completed;
    for (auto &decl : MutableObjc3ParsedProgramAst(program).interfaces) {
      FinalizeObjcInterfacePropertyIvarLayoutClosure(program, decl, visiting,
                                                     completed);
    }

    for (auto &implementation :
         MutableObjc3ParsedProgramAst(program).implementations) {
      Objc3InterfaceDecl *interface_decl =
          FindObjcInterfaceLayoutOwner(program, implementation.name);
      if (implementation.has_category || interface_decl == nullptr) {
        for (auto &property : implementation.properties) {
          ClearObjcPropertyIvarLayoutClosure(property);
        }
      } else {
        for (auto &property : implementation.properties) {
          Objc3PropertyDecl *interface_property =
              FindObjcPropertyDeclByName(interface_decl->properties,
                                         property.name);
          if (interface_property == nullptr) {
            continue;
          }
          CopyObjcIvarLayoutClosure(property, *interface_property);
        }
      }
      FinalizeObjcPropertySynthesisIvarBindingPackets(
          implementation.properties,
          implementation.property_synthesis_symbols_lexicographic,
          implementation.ivar_binding_symbols_lexicographic);
    }
  }

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

