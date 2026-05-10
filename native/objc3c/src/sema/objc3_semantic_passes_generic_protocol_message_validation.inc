    return false;
  }
  std::size_t cursor = object_name.size();
  while (cursor < trimmed.size() &&
         std::isspace(static_cast<unsigned char>(trimmed[cursor])) != 0) {
    ++cursor;
  }
  if (cursor >= trimmed.size() || trimmed[cursor] != '<') {
    return true;
  }
  int depth = 0;
  for (std::size_t index = cursor; index < trimmed.size(); ++index) {
    const char c = trimmed[index];
    if (c == '<') {
      ++depth;
    } else if (c == '>') {
      --depth;
      if (depth == 0) {
        generic_suffix_text = trimmed.substr(cursor, index - cursor + 1u);
        return true;
      }
      if (depth < 0) {
        return false;
      }
    }
  }
  return false;
}

static SemanticTypeInfo MakeSemanticTypeFromGenericArgumentText(
    const std::string &argument_text) {
  const std::string argument = TrimAsciiWhitespace(argument_text);
  if (argument.rfind("id<", 0) == 0u && argument.size() > 2u) {
    SemanticTypeInfo info = MakeScalarSemanticType(ValueType::ObjCId);
    info.canonical_type = BuildCanonicalSemanticType(
        ValueType::ObjCId, false, "", 1u, true, false, false, false, false,
        "", true, argument.substr(2), false, 0u, {}, false, false, false,
        false, false, false, false, false, false, false, false, false);
    info.ownership_kind = SemanticOwnershipKind::Retained;
    return info;
  }

  const std::string object_name = GenericArgumentObjectTypeName(argument);
  if (!object_name.empty()) {
    std::string specialized_object_name;
    std::string generic_suffix_text;
    const bool has_specialization = ExtractGenericArgumentSpecialization(
        argument, specialized_object_name, generic_suffix_text);
    unsigned pointer_depth = 0;
    for (const char c : argument) {
      if (c == '*') {
        ++pointer_depth;
      }
    }
    if (pointer_depth == 0u) {
      pointer_depth = 1u;
    }
    SemanticTypeInfo info = MakeScalarSemanticType(ValueType::ObjCObjectPtr);
    info.canonical_type = BuildCanonicalSemanticType(
        ValueType::ObjCObjectPtr, false, "", 1u, false, false, false, false,
        true, object_name, has_specialization && !generic_suffix_text.empty(),
        generic_suffix_text, true, pointer_depth, {}, false, false,
        false, false, false, false, false, false, false, false, false,
        false);
    info.object_pointer_type_name = object_name;
    info.ownership_kind = SemanticOwnershipKind::Retained;
    return info;
  }

  return MakeScalarSemanticType(ValueType::Unknown);
}

static SemanticTypeInfo SubstituteGenericReceiverType(
    const SemanticTypeInfo &type,
    const Objc3SemanticCanonicalType &receiver_type,
    const Objc3SemanticIntegrationSurface &surface) {
  if (type.canonical_type.object_pointer_type_name.empty() ||
      receiver_type.object_pointer_type_name.empty() ||
      receiver_type.generic_arguments_source_order.empty()) {
    return type;
  }
  const auto interface_it =
      surface.interfaces.find(receiver_type.object_pointer_type_name);
  if (interface_it == surface.interfaces.end()) {
    return type;
  }
  const auto &parameter_names =
      interface_it->second.generic_parameter_names_source_order;
  const auto parameter_it =
      std::find(parameter_names.begin(), parameter_names.end(),
                type.canonical_type.object_pointer_type_name);
  if (parameter_it == parameter_names.end()) {
    return type;
  }
  const std::size_t index =
      static_cast<std::size_t>(parameter_it - parameter_names.begin());
  if (index >= receiver_type.generic_arguments_source_order.size()) {
    return type;
  }
  const SemanticTypeInfo substituted = MakeSemanticTypeFromGenericArgumentText(
      receiver_type.generic_arguments_source_order[index]);
  return IsUnknownSemanticType(substituted) ? type : substituted;
}

static bool GenericArgumentSatisfiesProtocolConstraint(
    const Objc3Program &ast,
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &protocol_definitions,
    const std::string &argument,
    const std::string &required_protocol) {
  const std::string trimmed = TrimAsciiWhitespace(argument);
  if (trimmed.rfind("id<", 0) == 0u) {
    const ProtocolCompositionParseResult parsed =
        ParseProtocolCompositionSuffixText(trimmed.substr(2));
    for (const auto &protocol_name : parsed.names_lexicographic) {
      std::unordered_set<std::string> active_protocols;
      if (ProtocolSatisfiesConstraint(protocol_definitions, protocol_name,
                                      required_protocol, active_protocols)) {
        return true;
      }
    }
    return false;
  }
  const std::string object_name = GenericArgumentObjectTypeName(trimmed);
  if (object_name.empty()) {
    return false;
  }
  for (const auto &interface_decl : ast.interfaces) {
    if (interface_decl.has_category || interface_decl.name != object_name) {
      continue;
    }
    for (const auto &adopted_protocol : interface_decl.adopted_protocols) {
      std::unordered_set<std::string> active_protocols;
      if (ProtocolSatisfiesConstraint(protocol_definitions, adopted_protocol,
                                      required_protocol, active_protocols)) {
        return true;
      }
    }
  }
  return false;
}

static bool IsProtocolQualifiedGenericArgument(const std::string &argument) {
  const std::string trimmed = TrimAsciiWhitespace(argument);
  return trimmed.rfind("id<", 0) == 0u;
}

static bool ValidateProtocolQualifiedGenericArgument(
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &protocol_definitions,
    const std::string &argument,
    const std::string &object_pointer_type_name,
    const std::string &parameter_name,
    unsigned line,
    unsigned column,
    std::vector<std::string> &diagnostics) {
  const std::string trimmed = TrimAsciiWhitespace(argument);
  if (trimmed.rfind("id<", 0) != 0u) {
    return true;
  }
  const ProtocolCompositionParseResult parsed =
      ParseProtocolCompositionSuffixText(trimmed.substr(2));
  if (!parsed.IsValid()) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S206",
        "type mismatch: generic argument '" + argument +
            "' for parameter '" + parameter_name + "' of interface '" +
            object_pointer_type_name +
            "' has an invalid protocol-qualified generic argument"));
    return false;
  }
  bool valid = true;
  for (const auto &protocol_name : parsed.names_lexicographic) {
    if (protocol_definitions.find(protocol_name) == protocol_definitions.end()) {
      diagnostics.push_back(MakeDiag(
          line, column, "O3S206",
          "type mismatch: generic argument '" + argument +
              "' for parameter '" + parameter_name + "' of interface '" +
              object_pointer_type_name +
              "' references unknown protocol '" + protocol_name + "'"));
      valid = false;
    }
  }
  return valid;
}

static bool IsGenericArgumentAssignableTo(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &target_argument,
    const std::string &value_argument,
    unsigned recursion_depth = 0u) {
  if (recursion_depth > 16u) {
    return false;
  }
  const std::string normalized_target =
      NormalizeGenericArgumentTypeSpelling(target_argument);
  const std::string normalized_value =
      NormalizeGenericArgumentTypeSpelling(value_argument);
  if (normalized_target == normalized_value) {
    return true;
  }
  if (normalized_target == "id" || normalized_target == "id*") {
    return true;
  }
  if (normalized_target.rfind("id<", 0) == 0u) {
    const std::string target_suffix =
        normalized_target.size() > 2u ? normalized_target.substr(2) : "";
    const ProtocolCompositionParseResult target_protocols =
        ParseProtocolCompositionSuffixText(target_suffix);
    if (!target_protocols.IsValid()) {
      return false;
    }
    if (normalized_value.rfind("id<", 0) == 0u) {
      const std::string value_suffix =
          normalized_value.size() > 2u ? normalized_value.substr(2) : "";
      const ProtocolCompositionParseResult value_protocols =
          ParseProtocolCompositionSuffixText(value_suffix);
      if (!value_protocols.IsValid()) {
        return false;
      }
      return std::includes(value_protocols.names_lexicographic.begin(),
                           value_protocols.names_lexicographic.end(),
                           target_protocols.names_lexicographic.begin(),
                           target_protocols.names_lexicographic.end());
    }
    const std::string value_object_name =
        GenericArgumentObjectTypeName(normalized_value);
    if (value_object_name.empty()) {
      return false;
    }
    for (const auto &required_protocol :
         target_protocols.names_lexicographic) {
      if (!InterfaceAdoptsProtocolDirectly(surface, value_object_name,
                                           required_protocol)) {
        return false;
      }
    }
    return true;
  }

  const std::string target_object_name =
      GenericArgumentObjectTypeName(normalized_target);
  const std::string value_object_name =
      GenericArgumentObjectTypeName(normalized_value);
  if (target_object_name.empty() || value_object_name.empty()) {
    return false;
  }
  if (!IsSameOrDerivedInterface(surface, target_object_name,
                                value_object_name)) {
    return false;
  }

  std::string target_specialized_object_name;
  std::string target_generic_suffix_text;
  std::string value_specialized_object_name;
  std::string value_generic_suffix_text;
  if (!ExtractGenericArgumentSpecialization(
          normalized_target, target_specialized_object_name,
          target_generic_suffix_text) ||
      !ExtractGenericArgumentSpecialization(
          normalized_value, value_specialized_object_name,
          value_generic_suffix_text)) {
    return false;
  }
  if (target_specialized_object_name != value_specialized_object_name ||
      target_generic_suffix_text.empty() || value_generic_suffix_text.empty()) {
    return true;
  }

  SemanticTypeInfo target_info = MakeScalarSemanticType(ValueType::ObjCObjectPtr);
  target_info.canonical_type = BuildCanonicalSemanticType(
      ValueType::ObjCObjectPtr, false, "", 1u, false, false, false, false,
      true, target_specialized_object_name, true, target_generic_suffix_text,
      true, 1u, {}, false, false, false, false, false, false, false, false,
      false, false, false, false);
  SemanticTypeInfo value_info = MakeScalarSemanticType(ValueType::ObjCObjectPtr);
  value_info.canonical_type = BuildCanonicalSemanticType(
      ValueType::ObjCObjectPtr, false, "", 1u, false, false, false, false,
      true, value_specialized_object_name, true, value_generic_suffix_text,
      true, 1u, {}, false, false, false, false, false, false, false, false,
      false, false, false, false);
  const std::optional<bool> nested_compatibility =
      AreGenericSpecializationsVarianceAssignmentCompatible(target_info,
                                                            value_info,
                                                            &surface);
  return nested_compatibility.value_or(true);
}

static std::optional<bool> AreGenericSpecializationsVarianceAssignmentCompatible(
    const SemanticTypeInfo &target, const SemanticTypeInfo &value,
    const Objc3SemanticIntegrationSurface *surface) {
  if (surface == nullptr ||
      target.type != ValueType::ObjCObjectPtr ||
      value.type != ValueType::ObjCObjectPtr ||
      target.canonical_type.object_pointer_type_name.empty() ||
      value.canonical_type.object_pointer_type_name.empty() ||
      target.canonical_type.object_pointer_type_name !=
          value.canonical_type.object_pointer_type_name ||
      !target.canonical_type.has_generic_suffix ||
      !value.canonical_type.has_generic_suffix) {
    return std::nullopt;
  }
  const auto interface_it =
      surface->interfaces.find(target.canonical_type.object_pointer_type_name);
  if (interface_it == surface->interfaces.end() ||
      interface_it->second.generic_parameter_names_source_order.empty()) {
    return std::nullopt;
  }
  const auto &target_arguments =
      target.canonical_type.generic_arguments_source_order;
  const auto &value_arguments =
      value.canonical_type.generic_arguments_source_order;
  const auto &parameter_names =
      interface_it->second.generic_parameter_names_source_order;
  const auto &parameter_variance =
      interface_it->second.generic_parameter_variance_source_order;
  if (target_arguments.size() != value_arguments.size() ||
      target_arguments.size() != parameter_names.size() ||
      parameter_variance.size() != parameter_names.size()) {
    return false;
  }
  for (std::size_t index = 0; index < target_arguments.size(); ++index) {
    const std::string variance = parameter_variance[index];
    const std::string normalized_target =
        NormalizeGenericArgumentTypeSpelling(target_arguments[index]);
    const std::string normalized_value =
        NormalizeGenericArgumentTypeSpelling(value_arguments[index]);
    if (variance.empty() || variance == "__invariant") {
      if (normalized_target != normalized_value) {
        return false;
      }
      continue;
    }
    if (variance == "__covariant") {
      if (!IsGenericArgumentAssignableTo(*surface, target_arguments[index],
                                         value_arguments[index])) {
        return false;
      }
      continue;
    }
    if (variance == "__contravariant") {
      if (!IsGenericArgumentAssignableTo(*surface, value_arguments[index],
                                         target_arguments[index])) {
        return false;
      }
      continue;
    }
    return false;
  }
  return true;
}

static void ValidateGenericSpecializationType(
    const Objc3Program &ast,
    const std::unordered_map<std::string, Objc3InterfaceGenericDefinition>
        &generic_definitions,
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &protocol_definitions,
    const std::string &object_pointer_type_name, bool has_generic_suffix,
    const std::string &generic_suffix_text, unsigned line, unsigned column,
    const std::string &context, std::vector<std::string> &diagnostics,
    unsigned recursion_depth = 0u) {
  if (recursion_depth > 16u) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S206",
        "type mismatch: generic specialization for " + context +
            " exceeds supported nested generic depth"));
    return;
  }
  const auto definition_it = generic_definitions.find(object_pointer_type_name);
  if (definition_it == generic_definitions.end()) {
    return;
  }
  const Objc3InterfaceGenericDefinition &definition = definition_it->second;
  if (!has_generic_suffix) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S206",
        "type mismatch: generic interface '" + object_pointer_type_name +
            "' used by " + context + " requires " +
            std::to_string(definition.parameters.size()) +
            " generic argument(s)"));
    return;
  }
  const std::vector<std::string> arguments =
      ParseGenericArgumentSuffixTextSourceOrder(generic_suffix_text);
  if (arguments.size() != definition.parameters.size()) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S206",
        "type mismatch: generic interface '" + object_pointer_type_name +
            "' used by " + context + " expects " +
            std::to_string(definition.parameters.size()) +
            " generic argument(s), got " + std::to_string(arguments.size())));
    return;
  }
  for (std::size_t index = 0; index < arguments.size(); ++index) {
    const auto &param = definition.parameters[index];
    if (IsProtocolQualifiedGenericArgument(arguments[index])) {
      if (!ValidateProtocolQualifiedGenericArgument(
              protocol_definitions, arguments[index], object_pointer_type_name,
              param.name, line, column, diagnostics)) {
        continue;
      }
    } else {
      std::string nested_object_name;
      std::string nested_generic_suffix_text;
      if (!ExtractGenericArgumentSpecialization(arguments[index],
                                               nested_object_name,
                                               nested_generic_suffix_text)) {
        diagnostics.push_back(MakeDiag(
            line, column, "O3S206",
            "type mismatch: generic argument '" + arguments[index] +
                "' for parameter '" + param.name + "' of interface '" +
                object_pointer_type_name +
                "' has an unterminated nested generic specialization"));
      } else if (!nested_object_name.empty() &&
                 generic_definitions.find(nested_object_name) !=
                     generic_definitions.end()) {
        ValidateGenericSpecializationType(
            ast, generic_definitions, protocol_definitions, nested_object_name,
            !nested_generic_suffix_text.empty(), nested_generic_suffix_text,
            line, column,
            "nested generic argument '" + arguments[index] + "' in " + context,
            diagnostics, recursion_depth + 1u);
      }
    }
    for (const auto &required_protocol :
         param.constraint_protocols_lexicographic) {
      if (!GenericArgumentSatisfiesProtocolConstraint(
              ast, protocol_definitions, arguments[index],
              required_protocol)) {
        diagnostics.push_back(MakeDiag(
            line, column, "O3S206",
            "type mismatch: generic argument '" + arguments[index] +
                "' for parameter '" + param.name + "' of interface '" +
                object_pointer_type_name +
                "' does not satisfy protocol constraint '" +
                required_protocol + "'"));
      }
    }
  }
}

static void ValidateInterfaceGenericSpecializations(
    const Objc3Program &ast,
    const std::unordered_map<std::string, Objc3InterfaceGenericDefinition>
        &generic_definitions,
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &protocol_definitions,
    std::vector<std::string> &diagnostics) {
  auto validate_param = [&](const FuncParam &param,
                            const std::string &context) {
    if (!param.object_pointer_type_spelling) {
      return;
    }
    ValidateGenericSpecializationType(
        ast, generic_definitions, protocol_definitions,
        param.object_pointer_type_name, param.has_generic_suffix,
        param.generic_suffix_text, param.line, param.column, context,
        diagnostics);
  };
  auto validate_method = [&](const Objc3MethodDecl &method,
                             const std::string &context) {
    if (method.return_object_pointer_type_spelling) {
      ValidateGenericSpecializationType(
          ast, generic_definitions, protocol_definitions,
          method.return_object_pointer_type_name,
          method.has_return_generic_suffix,
          method.return_generic_suffix_text, method.line,
          method.column, context + " return type", diagnostics);
    }
    for (const auto &param : method.params) {
      validate_param(param, context + " parameter '" + param.name + "'");
    }
  };
  auto validate_property = [&](const Objc3PropertyDecl &property,
                               const std::string &context) {
    if (!property.object_pointer_type_spelling) {
      return;
    }
    ValidateGenericSpecializationType(
        ast, generic_definitions, protocol_definitions,
        property.object_pointer_type_name, property.has_generic_suffix,
        property.generic_suffix_text, property.line, property.column, context,
        diagnostics);
  };

  for (const auto &fn : ast.functions) {
    if (fn.return_object_pointer_type_spelling) {
      ValidateGenericSpecializationType(
          ast, generic_definitions, protocol_definitions,
          fn.return_object_pointer_type_name, fn.has_return_generic_suffix,
          fn.return_generic_suffix_text, fn.line, fn.column,
          "function '" + fn.name + "' return type", diagnostics);
    }
    for (const auto &param : fn.params) {
      validate_param(param, "function '" + fn.name + "' parameter '" +
                                param.name + "'");
    }
  }
  for (const auto &interface_decl : ast.interfaces) {
    const std::string container_name = FormatObjcContainerName(
        interface_decl.name, interface_decl.has_category,
        interface_decl.category_name);
    for (const auto &property : interface_decl.properties) {
      validate_property(property, "interface '" + container_name +
                                      "' property '" + property.name + "'");
    }
    for (const auto &method : interface_decl.methods) {
      validate_method(method, "interface '" + container_name + "' selector '" +
                                  method.selector + "'");
    }
  }
  for (const auto &implementation_decl : ast.implementations) {
    const std::string container_name = FormatObjcContainerName(
        implementation_decl.name, implementation_decl.has_category,
        implementation_decl.category_name);
    for (const auto &property : implementation_decl.properties) {
      validate_property(property, "implementation '" + container_name +
                                      "' property '" + property.name + "'");
    }
    for (const auto &method : implementation_decl.methods) {
      validate_method(method, "implementation '" + container_name +
                                  "' selector '" +
                                  method.selector + "'");
    }
  }
}

static void ValidateProtocolCompositionIdentifierBindings(
    const Objc3Program &ast,
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &definitions,
    std::vector<std::string> &diagnostics) {
  auto validate_suffix =
      [&](bool has_generic_suffix, bool supports_generic_suffix,
          const std::string &suffix_text, unsigned line, unsigned column,
          const std::string &context) {
        if (!has_generic_suffix || !supports_generic_suffix) {
          return;
        }
        const ProtocolCompositionParseResult parsed =
            ParseProtocolCompositionSuffixText(suffix_text);
        if (!parsed.IsValid()) {
          return;
        }
        for (const auto &protocol_name : parsed.names_lexicographic) {
          if (definitions.find(protocol_name) == definitions.end()) {
            diagnostics.push_back(MakeDiag(
                line, column, "O3S206",
                "type mismatch: protocol-qualified type for " + context +
                    " references unknown protocol '" + protocol_name + "'"));
          }
        }
      };

  auto validate_property = [&](const Objc3PropertyDecl &property,
                               const std::string &context) {
    validate_suffix(property.has_generic_suffix,
                    SupportsGenericPropertyTypeSuffix(property),
                    property.generic_suffix_text, property.generic_line,
                    property.generic_column, context);
  };

  auto validate_method = [&](const Objc3MethodDecl &method,
                             const std::string &context) {
    validate_suffix(method.has_return_generic_suffix,
                    SupportsGenericReturnTypeSuffix(method),
                    method.return_generic_suffix_text,
                    method.return_generic_line, method.return_generic_column,
                    context + " return type");
    for (const auto &param : method.params) {
      validate_suffix(param.has_generic_suffix,
                      SupportsGenericParamTypeSuffix(param),
                      param.generic_suffix_text, param.generic_line,
                      param.generic_column,
                      context + " parameter '" + param.name + "'");
    }
  };

  for (const auto &fn : ast.functions) {
    validate_suffix(fn.has_return_generic_suffix,
                    SupportsGenericReturnTypeSuffix(fn),
                    fn.return_generic_suffix_text, fn.return_generic_line,
                    fn.return_generic_column,
                    "function '" + fn.name + "' return type");
    for (const auto &param : fn.params) {
      validate_suffix(param.has_generic_suffix,
                      SupportsGenericParamTypeSuffix(param),
                      param.generic_suffix_text, param.generic_line,
                      param.generic_column,
                      "function '" + fn.name + "' parameter '" +
                          param.name + "'");
    }
  }

  for (const auto &protocol_decl : ast.protocols) {
    for (const auto &property : protocol_decl.properties) {
      validate_property(property, "protocol '" + protocol_decl.name +
                                      "' property '" + property.name + "'");
    }
    for (const auto &method : protocol_decl.methods) {
      validate_method(method, "protocol '" + protocol_decl.name +
                                  "' selector '" + MethodSelectorName(method) +
                                  "'");
    }
  }

  for (const auto &interface_decl : ast.interfaces) {
    const std::string container_name = FormatObjcContainerName(
        interface_decl.name, interface_decl.has_category,
        interface_decl.category_name);
    const std::string container_label =
        interface_decl.has_category ? "category interface" : "interface";
    for (const auto &property : interface_decl.properties) {
      validate_property(property, container_label + " '" + container_name +
                                      "' property '" + property.name + "'");
    }
    for (const auto &method : interface_decl.methods) {
      validate_method(method, container_label + " '" + container_name +
                                  "' selector '" + MethodSelectorName(method) +
                                  "'");
    }
  }

  for (const auto &implementation_decl : ast.implementations) {
    const std::string container_name = FormatObjcContainerName(
        implementation_decl.name, implementation_decl.has_category,
        implementation_decl.category_name);
    const std::string container_label = implementation_decl.has_category
                                            ? "category implementation"
                                            : "implementation";
    for (const auto &property : implementation_decl.properties) {
      validate_property(property, container_label + " '" + container_name +
                                      "' property '" + property.name + "'");
    }
    for (const auto &method : implementation_decl.methods) {
      validate_method(method, container_label + " '" + container_name +
                                  "' selector '" + MethodSelectorName(method) +
                                  "'");
    }
  }
}

static bool IsCompatibleMethodSignature(const Objc3MethodInfo &lhs, const Objc3MethodInfo &rhs) {
  if (lhs.arity != rhs.arity || lhs.return_type != rhs.return_type || lhs.return_is_vector != rhs.return_is_vector ||
      lhs.is_class_method != rhs.is_class_method ||
      !IsCompatibleCanonicalSemanticType(lhs.return_canonical_type,
                                         rhs.return_canonical_type) ||
      lhs.return_has_ownership_qualifier != rhs.return_has_ownership_qualifier ||
      lhs.return_ownership_insert_retain != rhs.return_ownership_insert_retain ||
      lhs.return_ownership_insert_release != rhs.return_ownership_insert_release ||
      lhs.return_ownership_insert_autorelease != rhs.return_ownership_insert_autorelease ||
      lhs.return_ownership_is_weak_reference != rhs.return_ownership_is_weak_reference ||
      lhs.return_ownership_is_unowned_reference != rhs.return_ownership_is_unowned_reference ||
      lhs.return_ownership_is_unowned_safe_reference != rhs.return_ownership_is_unowned_safe_reference ||
      lhs.return_ownership_arc_diagnostic_candidate != rhs.return_ownership_arc_diagnostic_candidate ||
      lhs.return_ownership_arc_fixit_available != rhs.return_ownership_arc_fixit_available ||
      lhs.return_ownership_arc_diagnostic_profile != rhs.return_ownership_arc_diagnostic_profile ||
      lhs.return_ownership_arc_fixit_hint != rhs.return_ownership_arc_fixit_hint) {
    return false;
  }
  if (lhs.return_is_vector &&
      (lhs.return_vector_base_spelling != rhs.return_vector_base_spelling ||
       lhs.return_vector_lane_count != rhs.return_vector_lane_count)) {
    return false;
  }
  if (lhs.param_canonical_types.size() != lhs.arity ||
      rhs.param_canonical_types.size() != rhs.arity ||
      lhs.param_has_ownership_qualifier.size() != lhs.arity ||
      rhs.param_has_ownership_qualifier.size() != rhs.arity) {
    return false;
  }
  if (lhs.param_ownership_insert_retain.size() != lhs.arity ||
      rhs.param_ownership_insert_retain.size() != rhs.arity ||
      lhs.param_ownership_insert_release.size() != lhs.arity ||
      rhs.param_ownership_insert_release.size() != rhs.arity ||
      lhs.param_ownership_insert_autorelease.size() != lhs.arity ||
      rhs.param_ownership_insert_autorelease.size() != rhs.arity ||
      lhs.param_ownership_is_weak_reference.size() != lhs.arity ||
      rhs.param_ownership_is_weak_reference.size() != rhs.arity ||
      lhs.param_ownership_is_unowned_reference.size() != lhs.arity ||
      rhs.param_ownership_is_unowned_reference.size() != rhs.arity ||
      lhs.param_ownership_is_unowned_safe_reference.size() != lhs.arity ||
      rhs.param_ownership_is_unowned_safe_reference.size() != rhs.arity ||
      lhs.param_ownership_arc_diagnostic_candidate.size() != lhs.arity ||
      rhs.param_ownership_arc_diagnostic_candidate.size() != rhs.arity ||
      lhs.param_ownership_arc_fixit_available.size() != lhs.arity ||
      rhs.param_ownership_arc_fixit_available.size() != rhs.arity ||
      lhs.param_ownership_arc_diagnostic_profile.size() != lhs.arity ||
      rhs.param_ownership_arc_diagnostic_profile.size() != rhs.arity ||
      lhs.param_ownership_arc_fixit_hint.size() != lhs.arity ||
      rhs.param_ownership_arc_fixit_hint.size() != rhs.arity) {
    return false;
  }
  if (!AreEquivalentProtocolCompositions(lhs.return_has_protocol_composition,
                                         lhs.return_protocol_composition_lexicographic,
                                         rhs.return_has_protocol_composition,
                                         rhs.return_protocol_composition_lexicographic)) {
    return false;
  }
  for (std::size_t i = 0; i < lhs.arity; ++i) {
    if (i >= lhs.param_types.size() || i >= lhs.param_canonical_types.size() ||
        i >= lhs.param_is_vector.size() || i >= lhs.param_vector_base_spelling.size() ||
        i >= lhs.param_vector_lane_count.size() || i >= lhs.param_has_protocol_composition.size() ||
        i >= lhs.param_protocol_composition_lexicographic.size() || i >= rhs.param_types.size() ||
        i >= rhs.param_canonical_types.size() ||
        i >= rhs.param_is_vector.size() || i >= rhs.param_vector_base_spelling.size() ||
        i >= rhs.param_vector_lane_count.size() || i >= rhs.param_has_protocol_composition.size() ||
        i >= rhs.param_protocol_composition_lexicographic.size()) {
      return false;
    }
    if (i >= lhs.param_has_ownership_qualifier.size() || i >= rhs.param_has_ownership_qualifier.size()) {
      return false;
    }
    if (lhs.param_types[i] != rhs.param_types[i] || lhs.param_is_vector[i] != rhs.param_is_vector[i]) {
      return false;
    }
    if (!IsCompatibleCanonicalSemanticType(lhs.param_canonical_types[i],
                                           rhs.param_canonical_types[i])) {
      return false;
    }
    if (lhs.param_has_ownership_qualifier[i] != rhs.param_has_ownership_qualifier[i]) {
      return false;
    }
    if (lhs.param_ownership_insert_retain[i] != rhs.param_ownership_insert_retain[i] ||
        lhs.param_ownership_insert_release[i] != rhs.param_ownership_insert_release[i] ||
        lhs.param_ownership_insert_autorelease[i] != rhs.param_ownership_insert_autorelease[i] ||
        lhs.param_ownership_is_weak_reference[i] != rhs.param_ownership_is_weak_reference[i] ||
        lhs.param_ownership_is_unowned_reference[i] != rhs.param_ownership_is_unowned_reference[i] ||
        lhs.param_ownership_is_unowned_safe_reference[i] != rhs.param_ownership_is_unowned_safe_reference[i] ||
        lhs.param_ownership_arc_diagnostic_candidate[i] != rhs.param_ownership_arc_diagnostic_candidate[i] ||
        lhs.param_ownership_arc_fixit_available[i] != rhs.param_ownership_arc_fixit_available[i] ||
        lhs.param_ownership_arc_diagnostic_profile[i] != rhs.param_ownership_arc_diagnostic_profile[i] ||
        lhs.param_ownership_arc_fixit_hint[i] != rhs.param_ownership_arc_fixit_hint[i]) {
      return false;
    }
    if (lhs.param_is_vector[i] &&
        (lhs.param_vector_base_spelling[i] != rhs.param_vector_base_spelling[i] ||
         lhs.param_vector_lane_count[i] != rhs.param_vector_lane_count[i])) {
      return false;
    }
    if (!AreEquivalentProtocolCompositions(lhs.param_has_protocol_composition[i],
                                           lhs.param_protocol_composition_lexicographic[i],
                                           rhs.param_has_protocol_composition[i],
                                           rhs.param_protocol_composition_lexicographic[i])) {
      return false;
    }
  }
  return true;
}

// Legacy extraction anchors retained for contract tests:
// static ValueType ValidateMessageSendExpr(
// if (receiver_type != ValueType::Unknown && !IsMessageCompatibleType(receiver_type)) {
static SemanticTypeInfo ValidateMessageSendExpr(const Expr *expr,
                                                const std::vector<SemanticScope> &scopes,
                                                const std::unordered_map<std::string, ValueType> &globals,
                                                const std::unordered_map<std::string, FunctionInfo> &functions,
                                                std::vector<std::string> &diagnostics,
                                                std::size_t max_message_send_args,
                                                const Objc3MessageSendResolutionContext &message_send_context);

static bool ResolveBlockCaptureSemanticType(
    const std::vector<SemanticScope> &scopes,
    const std::unordered_map<std::string, ValueType> &globals,
    const std::string &name,
    SemanticTypeInfo &resolved_type) {
  resolved_type = ScopeLookupType(scopes, name);
  if (!IsUnknownSemanticType(resolved_type)) {
    return true;
  }
  const auto global_it = globals.find(name);
  if (global_it != globals.end()) {
    resolved_type = MakeSemanticTypeFromGlobal(global_it->second);
    return true;
  }
  return false;
}

static void ValidateBlockLiteralCaptureLegality(
    const Expr *expr,
    const std::vector<SemanticScope> &scopes,
    const std::unordered_map<std::string, ValueType> &globals,
    std::vector<std::string> &diagnostics) {
  if (expr == nullptr || expr->kind != Expr::Kind::BlockLiteral) {
    return;
  }

  Expr *mutable_expr = const_cast<Expr *>(expr);
  mutable_expr->block_runtime_owned_object_capture_count = 0;
  mutable_expr->block_runtime_weak_object_capture_count = 0;
  mutable_expr->block_runtime_unowned_object_capture_count = 0;
  mutable_expr->block_runtime_owned_object_capture_names_lexicographic.clear();
  mutable_expr->block_runtime_weak_object_capture_names_lexicographic.clear();
  mutable_expr->block_runtime_unowned_object_capture_names_lexicographic.clear();
  mutable_expr->block_storage_mutable_capture_count = expr->block_mutated_capture_count;
  mutable_expr->block_storage_byref_slot_count = expr->block_byref_capture_count;
  mutable_expr->block_storage_requires_byref_cells =
      expr->block_byref_capture_count > 0u;
  mutable_expr->block_storage_escape_analysis_enabled = true;
  // escaping-block runtime-hook anchor: the live lowering profile
  // now inherits the parser-owned escape-shape heap candidate directly, so
  // later lane-C emission can distinguish true escaping block values from the
  // nonescaping stack-only slice.
  mutable_expr->block_storage_escape_to_heap =
      expr->block_escape_shape_promotes_to_heap_candidate;
  mutable_expr->block_runtime_copy_helper_required = false;
  mutable_expr->block_runtime_dispose_helper_required = false;
  mutable_expr->block_runtime_capture_ownership_is_normalized = false;
  mutable_expr->block_runtime_capture_ownership_profile.clear();
  mutable_expr->block_storage_escape_profile_is_normalized =
      expr->block_source_storage_annotations_are_normalized;
  mutable_expr->block_copy_dispose_profile_is_normalized =
      expr->block_source_storage_annotations_are_normalized;
  mutable_expr->block_storage_escape_profile.clear();
  mutable_expr->block_storage_byref_layout_symbol.clear();
  mutable_expr->block_copy_dispose_profile.clear();
  mutable_expr->block_copy_helper_symbol.clear();
  mutable_expr->block_dispose_helper_symbol.clear();

  if (expr->block_parameter_types_source_order.size() != expr->block_parameter_count) {
    diagnostics.push_back(
        MakeDiag(expr->line,
                 expr->column,
                 "O3S206",
                 "type mismatch: block invocation parameter typing is incomplete"));
  }
  if (expr->block_capture_names_lexicographic.size() != expr->block_capture_count ||
      !IsSortedUniqueStrings(expr->block_capture_names_lexicographic)) {
    diagnostics.push_back(
        MakeDiag(expr->line,
                 expr->column,
                 "O3S206",
                 "type mismatch: block capture inventory is not normalized"));
  }
  if (expr->block_mutated_capture_names_lexicographic.size() != expr->block_mutated_capture_count ||
      !IsSortedUniqueStrings(expr->block_mutated_capture_names_lexicographic)) {
    diagnostics.push_back(
        MakeDiag(expr->line,
                 expr->column,
                 "O3S206",
                 "type mismatch: block mutated-capture inventory is not normalized"));
  }
  if (expr->block_byref_capture_names_lexicographic.size() != expr->block_byref_capture_count ||
      !IsSortedUniqueStrings(expr->block_byref_capture_names_lexicographic)) {
    diagnostics.push_back(
        MakeDiag(expr->line,
                 expr->column,
                 "O3S206",
                 "type mismatch: block byref-capture inventory is not normalized"));
  }
  if (!expr->block_source_storage_annotations_are_normalized) {
    diagnostics.push_back(
        MakeDiag(expr->line,
                 expr->column,
                 "O3S206",
                 "type mismatch: block source storage annotations are not normalized"));
  }

  std::unordered_set<std::string> capture_names(
      expr->block_capture_names_lexicographic.begin(),
      expr->block_capture_names_lexicographic.end());
  std::unordered_set<std::string> mutated_names(
      expr->block_mutated_capture_names_lexicographic.begin(),
      expr->block_mutated_capture_names_lexicographic.end());
  std::unordered_set<std::string> explicit_byref_capture_names;
  for (const auto &item : expr->block_explicit_capture_items_source_order) {
    if (item.mode == "byref") {
      explicit_byref_capture_names.insert(item.name);
    }
  }
  std::unordered_set<std::string> explicit_capture_names;
  std::size_t owned_object_capture_count = 0;
  std::size_t weak_object_capture_count = 0;
  std::size_t unowned_object_capture_count = 0;
  for (const auto &mutated_name : expr->block_mutated_capture_names_lexicographic) {
    if (capture_names.count(mutated_name) == 0u) {
      diagnostics.push_back(
          MakeDiag(expr->line,
                   expr->column,
                   "O3S206",
                   "type mismatch: block mutated capture '" + mutated_name +
                       "' is not present in the capture inventory"));
    }
  }
  for (const auto &byref_name : expr->block_byref_capture_names_lexicographic) {
    if (mutated_names.count(byref_name) == 0u &&
        explicit_byref_capture_names.count(byref_name) == 0u) {
      diagnostics.push_back(
          MakeDiag(expr->line,
                   expr->column,
                   "O3S206",
                   "type mismatch: block byref capture '" + byref_name +
                       "' is not present in the mutated-capture inventory or explicit byref capture list"));
    }
  }
  for (const auto &item : expr->block_explicit_capture_items_source_order) {
    if (!explicit_capture_names.insert(item.name).second) {
      diagnostics.push_back(
          MakeDiag(expr->line,
                   expr->column,
                   "O3S301",
                   "capture-list legality failed: duplicate explicit capture '" +
                       item.name + "'"));
      continue;
    }

    SemanticTypeInfo resolved_type = MakeScalarSemanticType(ValueType::Unknown);
    const bool resolved_capture =
        ResolveBlockCaptureSemanticType(scopes, globals, item.name, resolved_type);
    if (!resolved_capture) {
      diagnostics.push_back(
          MakeDiag(expr->line,
                   expr->column,
                   "O3S202",
                   "undefined capture '" + item.name + "' in block literal"));
      continue;
    }
    if (capture_names.count(item.name) == 0u) {
      diagnostics.push_back(
          MakeDiag(expr->line,
                   expr->column,
                   "O3S303",
                   "capture-list legality failed: explicit capture '" +
                       item.name + "' is not used by the block body"));
    }
    if ((item.mode == "weak" || item.mode == "unowned") &&
        !IsObjCReferenceSemanticType(resolved_type)) {
      diagnostics.push_back(
          MakeDiag(expr->line,
                   expr->column,
                   "O3S302",
                   "capture-list legality failed: explicit " + item.mode +
                       " capture '" + item.name +
                       "' requires an Objective-C reference type"));
    }
  }
  for (const auto &capture_name : expr->block_capture_names_lexicographic) {
    SemanticTypeInfo resolved_type = MakeScalarSemanticType(ValueType::Unknown);
    if (!ResolveBlockCaptureSemanticType(scopes, globals, capture_name, resolved_type)) {
      diagnostics.push_back(
          MakeDiag(expr->line,
                   expr->column,
                   "O3S202",
                   "undefined capture '" + capture_name + "' in block literal"));
      continue;
    }

    if (IsOwnedObjCReferenceSemanticType(resolved_type)) {
      ++owned_object_capture_count;
      mutable_expr->block_runtime_owned_object_capture_names_lexicographic
          .push_back(capture_name);
    } else if (IsWeakObjCReferenceSemanticType(resolved_type)) {
      ++weak_object_capture_count;
      mutable_expr->block_runtime_weak_object_capture_names_lexicographic
          .push_back(capture_name);
    } else if (IsUnownedObjCReferenceSemanticType(resolved_type)) {
      ++unowned_object_capture_count;
      mutable_expr->block_runtime_unowned_object_capture_names_lexicographic
          .push_back(capture_name);
    }

    if (mutated_names.count(capture_name) != 0u &&
        (IsWeakObjCReferenceSemanticType(resolved_type) ||
         IsUnownedObjCReferenceSemanticType(resolved_type))) {
      diagnostics.push_back(
          MakeDiag(expr->line,
                   expr->column,
                   "O3S206",
                   "type mismatch: block mutated capture '" + capture_name +
                       "' requires owned runtime-backed storage"));
    }
  }

  mutable_expr->block_runtime_owned_object_capture_count =
      owned_object_capture_count;
  mutable_expr->block_runtime_weak_object_capture_count =
      weak_object_capture_count;
  mutable_expr->block_runtime_unowned_object_capture_count =
      unowned_object_capture_count;
  const bool runtime_copy_dispose_required =
      expr->block_byref_capture_count > 0u || owned_object_capture_count > 0u;
  const bool runtime_dispose_required =
      runtime_copy_dispose_required ||
      expr->block_explicit_capture_move_count > 0u;
  mutable_expr->block_runtime_copy_helper_required =
      runtime_copy_dispose_required;
  mutable_expr->block_runtime_dispose_helper_required =
      runtime_dispose_required;
  mutable_expr->block_runtime_capture_ownership_is_normalized =
      expr->block_capture_names_lexicographic.size() == expr->block_capture_count &&
      expr->block_mutated_capture_names_lexicographic.size() ==
          expr->block_mutated_capture_count &&
      expr->block_byref_capture_names_lexicographic.size() ==
      expr->block_byref_capture_count &&
      IsSortedUniqueStrings(expr->block_capture_names_lexicographic) &&
      IsSortedUniqueStrings(expr->block_mutated_capture_names_lexicographic) &&
      IsSortedUniqueStrings(expr->block_byref_capture_names_lexicographic) &&
      expr->block_runtime_owned_object_capture_names_lexicographic.size() ==
          owned_object_capture_count &&
      expr->block_runtime_weak_object_capture_names_lexicographic.size() ==
          weak_object_capture_count &&
      expr->block_runtime_unowned_object_capture_names_lexicographic.size() ==
          unowned_object_capture_count &&
      IsSortedUniqueStrings(
          expr->block_runtime_owned_object_capture_names_lexicographic) &&
      IsSortedUniqueStrings(
          expr->block_runtime_weak_object_capture_names_lexicographic) &&
      IsSortedUniqueStrings(
          expr->block_runtime_unowned_object_capture_names_lexicographic) &&
      expr->block_source_storage_annotations_are_normalized;
  std::ostringstream ownership_profile;
  // byref/copy-dispose/object-ownership anchor: sema upgrades the
  // frozen source-only block path with ownership-sensitive helper eligibility
  // and fail-closed non-owning mutation rejection before runnable block object
  // lowering lands.
  ownership_profile << "block-runtime-capture-ownership:owned="
                    << owned_object_capture_count
                    << ";weak=" << weak_object_capture_count
                    << ";unowned=" << unowned_object_capture_count
                    << ";copy-helper="
                    << (runtime_copy_dispose_required ? "enabled" : "elided")
                    << ";dispose-helper="
                    << (runtime_copy_dispose_required ? "enabled" : "elided");
  mutable_expr->block_runtime_capture_ownership_profile =
      ownership_profile.str();

  std::ostringstream storage_escape_profile;
  storage_escape_profile << "block-storage:mutable-captures="
                         << mutable_expr->block_storage_mutable_capture_count
                         << ";byref-slots="
                         << mutable_expr->block_storage_byref_slot_count
                         << ";escape="
                         << (mutable_expr->block_storage_escape_to_heap ? "heap"
                                                                        : "stack")
                         << ";body-statements="
                         << expr->block_body_statement_count;
  mutable_expr->block_storage_escape_profile = storage_escape_profile.str();
  if (mutable_expr->block_storage_requires_byref_cells) {
    std::ostringstream byref_layout_symbol;
    byref_layout_symbol << "__objc3_block_byref_layout_" << expr->line << "_"
                        << expr->column << "_m"
                        << mutable_expr->block_storage_mutable_capture_count
                        << "_b" << mutable_expr->block_storage_byref_slot_count
                        << "_"
                        << (mutable_expr->block_storage_escape_to_heap ? "heap"
                                                                      : "stack");
    mutable_expr->block_storage_byref_layout_symbol =
        byref_layout_symbol.str();
  }

  std::ostringstream copy_dispose_profile;
  copy_dispose_profile << mutable_expr->block_runtime_capture_ownership_profile
                       << ";block-copy-dispose:copy-helper="
                       << (mutable_expr->block_runtime_copy_helper_required
                               ? "enabled"
                               : "elided")
                       << ";dispose-helper="
                       << (mutable_expr->block_runtime_dispose_helper_required
                               ? "enabled"
                               : "elided")
                       << ";escape="
                       << (mutable_expr->block_storage_escape_to_heap ? "heap"
                                                                      : "stack")
                       << ";body-statements=" << expr->block_body_statement_count;
  mutable_expr->block_copy_dispose_profile = copy_dispose_profile.str();
  if (mutable_expr->block_runtime_copy_helper_required) {
    std::ostringstream copy_helper_symbol;
    copy_helper_symbol << "__objc3_block_copy_helper_" << expr->line << "_"
                       << expr->column << "_m"
                       << mutable_expr->block_storage_mutable_capture_count
                       << "_b" << mutable_expr->block_storage_byref_slot_count
                       << "_"
                       << (mutable_expr->block_storage_escape_to_heap ? "heap"
                                                                     : "stack");
    mutable_expr->block_copy_helper_symbol = copy_helper_symbol.str();
  }
  if (mutable_expr->block_runtime_dispose_helper_required) {
    std::ostringstream dispose_helper_symbol;
    dispose_helper_symbol << "__objc3_block_dispose_helper_" << expr->line
                          << "_" << expr->column << "_m"
                          << mutable_expr->block_storage_mutable_capture_count
                          << "_b" << mutable_expr->block_storage_byref_slot_count
                          << "_"
                          << (mutable_expr->block_storage_escape_to_heap ? "heap"
                                                                        : "stack");
    mutable_expr->block_dispose_helper_symbol = dispose_helper_symbol.str();
  }
}

static SemanticTypeInfo ValidateExpr(const Expr *expr, const std::vector<SemanticScope> &scopes,
                                     const std::unordered_map<std::string, ValueType> &globals,
                                     const std::unordered_map<std::string, FunctionInfo> &functions,
                                     std::vector<std::string> &diagnostics,
                                     std::size_t max_message_send_args,
                                     const Objc3MessageSendResolutionContext &message_send_context) {
  if (expr == nullptr) {
    return MakeScalarSemanticType(ValueType::Unknown);
  }
  if (expr->await_expression_enabled &&
      !message_send_context.inside_async_context) {
    diagnostics.push_back(MakeDiag(
        expr->line, expr->column, "O3S223",
        "async semantics failed: 'await' is only valid inside an async function or method"));
  }
  switch (expr->kind) {
    case Expr::Kind::Number:
      return MakeScalarSemanticType(ValueType::I32);
    case Expr::Kind::BoolLiteral:
      return MakeScalarSemanticType(ValueType::Bool);
    case Expr::Kind::NilLiteral: {
      SemanticTypeInfo nil_type = MakeScalarSemanticType(ValueType::ObjCId);
      nil_type.canonical_type.nullability =
          Objc3SemanticCanonicalNullability::Nullable;
      nil_type.has_nullability_suffix = true;
      return nil_type;
    }
    case Expr::Kind::Identifier: {
      if (expr->typed_keypath_literal_enabled) {
        if (!expr->typed_keypath_literal_is_normalized ||
            expr->typed_keypath_components.empty()) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: typed key-path literal normalization failed"));
          return MakeScalarSemanticType(ValueType::Unknown);
        }
        std::string validated_owner_name;
        bool is_class_root = false;
        if (!TryResolveTypedKeyPathValidatedOwner(
                *expr, scopes, globals, functions, message_send_context,
                validated_owner_name, is_class_root)) {
          diagnostics.push_back(MakeDiag(
              expr->line, expr->column, "O3S206",
              "type mismatch: typed key-path root '" +
                  expr->typed_keypath_root_name +
                  "' must resolve to 'self', a known class type, or an ObjC-reference-compatible identifier"));
          return MakeScalarSemanticType(ValueType::Unknown);
        }
        if (is_class_root) {
          if (expr->typed_keypath_components.size() != 1u) {
            diagnostics.push_back(MakeDiag(
                expr->line, expr->column, "O3S206",
                "type mismatch: typed key-path member chain '" +
                    JoinStringVector(expr->typed_keypath_components, ".") +
                    "' is unsupported until executable typed key-path lowering is enabled"));
            return MakeScalarSemanticType(ValueType::Unknown);
          }
          const std::string &component = expr->typed_keypath_components.front();
          if (FindTypedKeyPathPropertyOnOwner(
                  *message_send_context.surface, validated_owner_name,
                  component) == nullptr) {
            diagnostics.push_back(MakeDiag(
                expr->line, expr->column, "O3S206",
                "type mismatch: typed key-path component '" + component +
                    "' is not a readable property on root '" +
                    validated_owner_name + "'"));
            return MakeScalarSemanticType(ValueType::Unknown);
          }
        }
        return MakeScalarSemanticType(ValueType::ObjCId);
      }
      if (expr->ident == "super" && !message_send_context.inside_method) {
        diagnostics.push_back(MakeDiag(
            expr->line, expr->column, "O3S216",
            "selector resolution failed: 'super' is only valid inside an implementation method"));
        return MakeScalarSemanticType(ValueType::Unknown);
      }
      const SemanticTypeInfo local_type = ScopeLookupType(scopes, expr->ident);
      if (!IsUnknownSemanticType(local_type)) {
        return local_type;
      }
      auto global_it = globals.find(expr->ident);
      if (global_it != globals.end()) {
        return MakeSemanticTypeFromGlobal(global_it->second);
      }
      if (functions.find(expr->ident) != functions.end()) {
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                       "type mismatch: function '" + expr->ident +
                                           "' cannot be used as a value"));
        return MakeScalarSemanticType(ValueType::Function);
      }
      diagnostics.push_back(
          MakeDiag(expr->line, expr->column, "O3S202", "undefined identifier '" + expr->ident + "'"));
      return MakeScalarSemanticType(ValueType::Unknown);
    }
    case Expr::Kind::Binary: {
      const SemanticTypeInfo lhs = ValidateExpr(
          expr->left.get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);
      const SemanticTypeInfo rhs = ValidateExpr(
          expr->right.get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);

      if (expr->op == "??") {
        const bool lhs_reference_like =
            IsObjCReferenceSemanticType(lhs) || IsCallableSemanticType(lhs);
        const bool rhs_reference_like =
            IsObjCReferenceSemanticType(rhs) || IsCallableSemanticType(rhs);
        if (!IsUnknownSemanticType(lhs) && !lhs_reference_like) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: nil-coalescing lhs must be ObjC-reference-compatible"));
        }
        if (!IsUnknownSemanticType(rhs) && !rhs_reference_like) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: nil-coalescing rhs must be ObjC-reference-compatible"));
        }
        if (IsUnknownSemanticType(lhs)) {
          return rhs;
        }
        if (IsUnknownSemanticType(rhs)) {
          return MakeNonnullRefinedSemanticType(lhs);
        }
        if (AreObjCReferenceTypesAssignmentCompatible(lhs, rhs)) {
          if (lhs.type == rhs.type) {
            return rhs.has_nullability_suffix ? lhs : MakeNonnullRefinedSemanticType(lhs);
          }
          return MakeNonnullRefinedSemanticType(MakeScalarSemanticType(ValueType::ObjCId));
        }
        if (IsCallableSemanticType(lhs) && IsCallableSemanticType(rhs) &&
            lhs.callable_param_types == rhs.callable_param_types &&
            lhs.callable_return_type == rhs.callable_return_type) {
          return lhs;
        }
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                       "type mismatch: nil-coalescing operands must be type-compatible"));
        return MakeScalarSemanticType(ValueType::Unknown);
      }

      if (expr->op == "+" || expr->op == "-" || expr->op == "*" || expr->op == "/" || expr->op == "%") {
        if (!IsUnknownSemanticType(lhs) && !IsScalarI32CompatibleType(lhs)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for arithmetic lhs, got '" +
                                             SemanticTypeName(lhs) + "'"));
        }
        if (!IsUnknownSemanticType(rhs) && !IsScalarI32CompatibleType(rhs)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for arithmetic rhs, got '" +
                                             SemanticTypeName(rhs) + "'"));
        }
        return MakeScalarSemanticType(ValueType::I32);
      }

      if (expr->op == "&" || expr->op == "|" || expr->op == "^" || expr->op == "<<" || expr->op == ">>") {
        if (!IsUnknownSemanticType(lhs) && !IsScalarI32CompatibleType(lhs)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for bitwise lhs, got '" +
                                             SemanticTypeName(lhs) + "'"));
        }
        if (!IsUnknownSemanticType(rhs) && !IsScalarI32CompatibleType(rhs)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for bitwise rhs, got '" +
                                             SemanticTypeName(rhs) + "'"));
        }
        return MakeScalarSemanticType(ValueType::I32);
      }

      if (expr->op == "==" || expr->op == "!=") {
        if (lhs.is_vector || rhs.is_vector) {
          if (!IsUnknownSemanticType(lhs) && !IsUnknownSemanticType(rhs) && !IsSameSemanticType(lhs, rhs)) {
            diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                           "type mismatch: equality compares '" + SemanticTypeName(lhs) +
                                               "' with '" + SemanticTypeName(rhs) + "'"));
          }
          return MakeScalarSemanticType(ValueType::Bool);
        }

        const bool bool_to_i32_literal =
            (lhs.type == ValueType::Bool && rhs.type == ValueType::I32 && IsBoolLikeI32Literal(expr->right.get())) ||
            (rhs.type == ValueType::Bool && lhs.type == ValueType::I32 && IsBoolLikeI32Literal(expr->left.get()));
        const bool i32_alias_coercion = AreScalarI32AliasCompatible(lhs, rhs);
        if (!IsUnknownSemanticType(lhs) && !IsUnknownSemanticType(rhs) && lhs.type != rhs.type &&
            !bool_to_i32_literal && !i32_alias_coercion) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: equality compares '" + SemanticTypeName(lhs) +
                                             "' with '" + SemanticTypeName(rhs) + "'"));
        }
        return MakeScalarSemanticType(ValueType::Bool);
      }

      if (expr->op == "<" || expr->op == "<=" || expr->op == ">" || expr->op == ">=") {
        if (!IsUnknownSemanticType(lhs) && !IsScalarI32CompatibleType(lhs)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for relational lhs, got '" +
                                             SemanticTypeName(lhs) + "'"));
        }
        if (!IsUnknownSemanticType(rhs) && !IsScalarI32CompatibleType(rhs)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for relational rhs, got '" +
                                             SemanticTypeName(rhs) + "'"));
        }
        return MakeScalarSemanticType(ValueType::Bool);
      }

      if (expr->op == "&&" || expr->op == "||") {
        if (!IsUnknownSemanticType(lhs) && (lhs.is_vector || (lhs.type != ValueType::Bool && lhs.type != ValueType::I32))) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected bool for logical lhs, got '" +
                                             SemanticTypeName(lhs) + "'"));
        }
        if (!IsUnknownSemanticType(rhs) && (rhs.is_vector || (rhs.type != ValueType::Bool && rhs.type != ValueType::I32))) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected bool for logical rhs, got '" +
                                             SemanticTypeName(rhs) + "'"));
        }
        return MakeScalarSemanticType(ValueType::Bool);
      }

      return MakeScalarSemanticType(ValueType::Unknown);
    }
    case Expr::Kind::Conditional: {
      if (expr->left == nullptr || expr->right == nullptr || expr->third == nullptr) {
        return MakeScalarSemanticType(ValueType::Unknown);
      }

      const SemanticTypeInfo condition_type = ValidateExpr(
          expr->left.get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);
      if (!IsUnknownSemanticType(condition_type) && !IsScalarBoolCompatibleType(condition_type)) {
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                       "type mismatch: conditional condition must be bool-compatible"));
      }

      const SemanticTypeInfo then_type = ValidateExpr(
          expr->right.get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);
      const SemanticTypeInfo else_type = ValidateExpr(
          expr->third.get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);

      if (IsUnknownSemanticType(then_type)) {
        return else_type;
      }
      if (IsUnknownSemanticType(else_type)) {
        return then_type;
      }
      const bool then_scalar = IsScalarSemanticType(then_type) &&
                               (then_type.type == ValueType::I32 || then_type.type == ValueType::Bool);
      const bool else_scalar = IsScalarSemanticType(else_type) &&
                               (else_type.type == ValueType::I32 || else_type.type == ValueType::Bool);
      if (then_scalar && else_scalar) {
        if (then_type.type == else_type.type) {
          return then_type;
        }
        return MakeScalarSemanticType(ValueType::I32);
      }
      if (AreScalarI32AliasCompatible(then_type, else_type)) {
        if (then_type.type == else_type.type) {
          return then_type;
        }
        return MakeScalarSemanticType(ValueType::I32);
