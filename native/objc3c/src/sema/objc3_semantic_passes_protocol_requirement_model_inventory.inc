static std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
BuildProtocolSemanticDefinitions(const Objc3Program &ast,
                                 bool arc_mode_enabled,
                                 std::vector<std::string> &diagnostics) {
  std::unordered_map<std::string, Objc3ProtocolSemanticDefinition> definitions;
  for (const auto &protocol_decl : ast.protocols) {
    constexpr const char *kContainerLabel = "protocol";
    const std::string container_name = protocol_decl.name;
    auto definition_it = definitions.find(protocol_decl.name);
    if (definition_it == definitions.end()) {
      Objc3ProtocolSemanticDefinition definition;
      definition.name = protocol_decl.name;
      definition.is_forward_declaration = protocol_decl.is_forward_declaration;
      definition.inherited_protocols_lexicographic =
          BuildSortedUniqueStringsLocal(protocol_decl.inherited_protocols);
      definition_it =
          definitions.emplace(protocol_decl.name, std::move(definition)).first;
    } else if (!definition_it->second.is_forward_declaration &&
               !protocol_decl.is_forward_declaration) {
      diagnostics.push_back(MakeDiag(protocol_decl.line, protocol_decl.column,
                                     "O3S200",
                                     "duplicate protocol '" + protocol_decl.name +
                                         "'"));
      continue;
    } else if (definition_it->second.is_forward_declaration &&
               !protocol_decl.is_forward_declaration) {
      definition_it->second.is_forward_declaration = false;
      definition_it->second.inherited_protocols_lexicographic =
          BuildSortedUniqueStringsLocal(protocol_decl.inherited_protocols);
      definition_it->second.required_methods_by_key.clear();
      definition_it->second.optional_methods_by_key.clear();
      definition_it->second.required_properties_by_name.clear();
      definition_it->second.optional_properties_by_name.clear();
    } else {
      continue;
    }

    if (protocol_decl.is_forward_declaration) {
      continue;
    }

    std::unordered_map<std::string, Objc3PropertyInfo> protocol_properties;
    std::unordered_map<std::string, PropertyAccessorSelectorOwnerState>
        getter_owner_states;
    std::unordered_map<std::string, PropertyAccessorSelectorOwnerState>
        setter_owner_states;
    for (const auto &property_decl : protocol_decl.properties) {
      ValidatePropertyTypeSuffixes(property_decl, container_name, kContainerLabel,
                                   diagnostics);
      Objc3PropertyInfo property_info =
          BuildPropertyInfo(property_decl, container_name, kContainerLabel,
                            arc_mode_enabled,
                            diagnostics);
      const auto property_insert =
          protocol_properties.emplace(property_decl.name, std::move(property_info));
      if (!property_insert.second) {
        diagnostics.push_back(
            MakeDiag(property_decl.line, property_decl.column, "O3S200",
                     "duplicate protocol property '" + property_decl.name +
                         "' in protocol '" + protocol_decl.name + "'"));
        continue;
      }
      ValidatePropertyAccessorSelectorUniqueness(
          property_insert.first->second,
          property_decl.name,
          container_name,
          kContainerLabel,
          protocol_properties,
          getter_owner_states,
          setter_owner_states,
          diagnostics);
      Objc3ProtocolPropertyRequirementInfo requirement;
      requirement.property_name = property_decl.name;
      requirement.property = property_insert.first->second;
      requirement.protocol_name = protocol_decl.name;
      requirement.line = property_decl.line;
      requirement.column = property_decl.column;
      if (property_decl.protocol_requirement_kind ==
          Objc3ProtocolRequirementKind::Optional) {
        definition_it->second.optional_properties_by_name.emplace(
            requirement.property_name, std::move(requirement));
        continue;
      }
      definition_it->second.required_properties_by_name.emplace(
          requirement.property_name, std::move(requirement));
    }

    for (const auto &method_decl : protocol_decl.methods) {
      const MethodSelectorNormalizationContractInfo selector_contract =
          BuildMethodSelectorNormalizationContractInfo(method_decl);
      ValidateMethodSelectorNormalizationContract(method_decl, container_name,
                                                 kContainerLabel,
                                                 selector_contract, diagnostics);
      ValidateMethodReturnTypeSuffixes(method_decl, container_name,
                                       kContainerLabel, diagnostics);
      ValidateMethodParameterTypeSuffixes(method_decl, container_name,
                                          kContainerLabel, diagnostics);
      if (method_decl.objc_macro_declared ||
          method_decl.objc_macro_package_declared ||
          method_decl.objc_macro_provenance_declared) {
        diagnostics.push_back(MakeDiag(
            method_decl.line, method_decl.column, "O3S325",
            "macro expansion is currently limited to free functions; " +
                std::string(kContainerLabel) + " selector in '" +
                container_name + "' is not sandbox-admitted"));
      }
      if (method_decl.has_body) {
        diagnostics.push_back(MakeDiag(
            method_decl.line, method_decl.column, "O3S206",
            "type mismatch: protocol selector '" +
                selector_contract.normalized_selector + "' in '" +
                protocol_decl.name + "' must not define a body"));
      }

      const std::string requirement_key = BuildProtocolRequirementKey(
          selector_contract.normalized_selector, method_decl.is_class_method);
      if (definition_it->second.required_methods_by_key.find(requirement_key) !=
              definition_it->second.required_methods_by_key.end() ||
          definition_it->second.optional_methods_by_key.find(requirement_key) !=
              definition_it->second.optional_methods_by_key.end()) {
        diagnostics.push_back(MakeDiag(
            method_decl.line, method_decl.column, "O3S200",
            "duplicate protocol selector '" +
                FormatMethodSelectorForDiagnostic(
                    selector_contract.normalized_selector,
                    method_decl.is_class_method) +
                "' in protocol '" + protocol_decl.name + "'"));
        continue;
      }
      Objc3ProtocolRequirementInfo requirement;
      requirement.selector = selector_contract.normalized_selector;
      requirement.method =
          BuildMethodInfo(method_decl, selector_contract, arc_mode_enabled);
      requirement.protocol_name = protocol_decl.name;
      requirement.line = method_decl.line;
      requirement.column = method_decl.column;
      if (method_decl.protocol_requirement_kind ==
          Objc3ProtocolRequirementKind::Optional) {
        definition_it->second.optional_methods_by_key.emplace(requirement_key,
                                                              std::move(requirement));
        continue;
      }
      definition_it->second.required_methods_by_key.emplace(requirement_key,
                                                            std::move(requirement));
    }
  }
  return definitions;
}
