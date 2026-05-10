struct Objc3ProtocolRequirementInfo {
  std::string selector;
  Objc3MethodInfo method;
  std::string protocol_name;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ProtocolPropertyRequirementInfo {
  std::string property_name;
  Objc3PropertyInfo property;
  std::string protocol_name;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ProtocolSemanticDefinition {
  std::string name;
  bool is_forward_declaration = false;
  std::vector<std::string> inherited_protocols_lexicographic;
  std::unordered_map<std::string, Objc3ProtocolPropertyRequirementInfo>
      required_properties_by_name;
  std::unordered_map<std::string, Objc3ProtocolPropertyRequirementInfo>
      optional_properties_by_name;
  std::unordered_map<std::string, Objc3ProtocolRequirementInfo>
      required_methods_by_key;
  std::unordered_map<std::string, Objc3ProtocolRequirementInfo>
      optional_methods_by_key;
};

struct Objc3InterfaceGenericParameterDefinition {
  std::string name;
  std::string variance_spelling;
  std::vector<std::string> constraint_protocols_lexicographic;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3InterfaceGenericDefinition {
  std::string name;
  std::vector<Objc3InterfaceGenericParameterDefinition> parameters;
};

enum class Objc3ProtocolRequirementClosureStatus {
  Ready,
  MissingProtocol,
  ForwardDeclaration,
  InheritanceCycle,
  IncompatibleInheritedRequirement,
};

struct Objc3ProtocolRequirementClosure {
  Objc3ProtocolRequirementClosureStatus status =
      Objc3ProtocolRequirementClosureStatus::Ready;
  std::string failing_protocol_name;
  std::string conflicting_member_name;
  bool conflicting_member_is_method = false;
  bool conflicting_selector_is_class_method = false;
  std::unordered_map<std::string, const Objc3ProtocolPropertyRequirementInfo *>
      required_properties_by_name;
  std::unordered_map<std::string, const Objc3ProtocolPropertyRequirementInfo *>
      optional_properties_by_name;
  std::unordered_map<std::string, const Objc3ProtocolRequirementInfo *>
      required_methods_by_key;
  std::unordered_map<std::string, const Objc3ProtocolRequirementInfo *>
      optional_methods_by_key;
};

struct Objc3ProtocolQualifiedMessageRequirementResolution {
  enum class Status {
    NotProtocolQualified,
    MissingRequirementClosure,
    MissingSelector,
    AmbiguousSelector,
    Resolved,
  };

  Status status = Status::NotProtocolQualified;
  const Objc3ProtocolRequirementInfo *requirement = nullptr;
  const Objc3ProtocolPropertyRequirementInfo *property_requirement = nullptr;
  bool property_requirement_is_setter = false;
  std::string protocol_name;
  std::string failing_protocol_name;
};

static bool CollectProtocolRequirementClosure(
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &definitions,
    const std::string &protocol_name,
    std::unordered_set<std::string> &active_protocols,
    Objc3ProtocolRequirementClosure &closure);

static Objc3ProtocolQualifiedMessageRequirementResolution
ResolveProtocolQualifiedMessageRequirement(
    const Objc3SemanticCanonicalType &receiver_type,
    const std::string &selector,
    bool is_class_method,
    const Objc3MessageSendResolutionContext &context);

static std::string BuildProtocolRequirementKey(const std::string &selector,
                                               bool is_class_method) {
  return std::string(is_class_method ? "+" : "-") + selector;
}

static std::string FormatMethodSelectorForDiagnostic(const std::string &selector,
                                                     bool is_class_method) {
  return std::string(is_class_method ? "+" : "-") + selector;
}

static std::string BuildCategoryOwnerIdentity(const std::string &class_name,
                                              const std::string &category_name) {
  return "category:" + class_name + "(" + category_name + ")";
}

struct Objc3CategoryMergeOrderEntry {
  std::string class_name;
  std::string category_name;
  std::string owner_identity;
  std::string container_name;
  unsigned line = 1;
  unsigned column = 1;
};

static std::string FormatObjcContainerName(const std::string &name,
                                           bool has_category,
                                           const std::string &category_name) {
  return has_category ? name + "(" + category_name + ")" : name;
}

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

static bool ProtocolSatisfiesConstraint(
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &protocol_definitions,
    const std::string &candidate_protocol,
    const std::string &required_protocol,
    std::unordered_set<std::string> &active_protocols) {
  if (candidate_protocol == required_protocol) {
    return true;
  }
  if (!active_protocols.insert(candidate_protocol).second) {
    return false;
  }
  const auto definition_it = protocol_definitions.find(candidate_protocol);
  if (definition_it == protocol_definitions.end()) {
    active_protocols.erase(candidate_protocol);
    return false;
  }
  for (const auto &inherited :
       definition_it->second.inherited_protocols_lexicographic) {
    if (ProtocolSatisfiesConstraint(protocol_definitions, inherited,
                                    required_protocol, active_protocols)) {
      active_protocols.erase(candidate_protocol);
      return true;
    }
  }
  active_protocols.erase(candidate_protocol);
  return false;
}

static std::unordered_map<std::string, Objc3InterfaceGenericDefinition>
BuildInterfaceGenericDefinitions(
    const Objc3Program &ast,
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &protocol_definitions,
    std::vector<std::string> &diagnostics) {
  std::unordered_map<std::string, Objc3InterfaceGenericDefinition> definitions;
  for (const auto &interface_decl : ast.interfaces) {
    if (interface_decl.has_category || interface_decl.generic_params.empty()) {
      continue;
    }
    Objc3InterfaceGenericDefinition definition;
    definition.name = interface_decl.name;
    std::unordered_set<std::string> seen_names;
    for (const auto &param_decl : interface_decl.generic_params) {
      if (!seen_names.insert(param_decl.name).second) {
        diagnostics.push_back(MakeDiag(
            param_decl.line, param_decl.column, "O3S200",
            "duplicate generic parameter '" + param_decl.name +
                "' in interface '" + interface_decl.name + "'"));
        continue;
      }
      Objc3InterfaceGenericParameterDefinition param;
      param.name = param_decl.name;
      param.variance_spelling = param_decl.variance_spelling;
      param.line = param_decl.line;
      param.column = param_decl.column;
      if (param_decl.has_constraint) {
        if (param_decl.constraint_type_name != "id" ||
            !param_decl.has_constraint_generic_suffix) {
          diagnostics.push_back(MakeDiag(
              param_decl.constraint_line, param_decl.constraint_column,
              "O3S206",
              "type mismatch: generic parameter '" + param_decl.name +
                  "' in interface '" + interface_decl.name +
                  "' requires an id<Protocol> constraint"));
        } else {
          const ProtocolCompositionParseResult parsed =
              ParseProtocolCompositionSuffixText(
                  param_decl.constraint_generic_suffix_text);
          if (!parsed.IsValid()) {
            diagnostics.push_back(MakeDiag(
                param_decl.constraint_line, param_decl.constraint_column,
                "O3S206",
                "type mismatch: generic parameter '" + param_decl.name +
                    "' in interface '" + interface_decl.name +
                    "' has an invalid protocol constraint"));
          } else {
            param.constraint_protocols_lexicographic =
                parsed.names_lexicographic;
            for (const auto &protocol_name :
                 param.constraint_protocols_lexicographic) {
              if (protocol_definitions.find(protocol_name) ==
                  protocol_definitions.end()) {
                diagnostics.push_back(MakeDiag(
                    param_decl.constraint_line, param_decl.constraint_column,
                    "O3S206",
                    "type mismatch: generic parameter '" + param_decl.name +
                        "' in interface '" + interface_decl.name +
                        "' references unknown constraint protocol '" +
                        protocol_name + "'"));
              }
            }
          }
        }
      }
      definition.parameters.push_back(std::move(param));
    }
    definitions[definition.name] = std::move(definition);
  }
  return definitions;
}
