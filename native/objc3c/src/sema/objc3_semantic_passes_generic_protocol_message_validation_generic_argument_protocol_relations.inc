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
