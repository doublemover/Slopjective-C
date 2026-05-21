static bool IsFunctionGenericParameterName(const FunctionInfo &fn,
                                           const std::string &name,
                                           std::size_t *index = nullptr) {
  for (std::size_t i = 0; i < fn.generic_parameter_names_source_order.size();
       ++i) {
    if (fn.generic_parameter_names_source_order[i] == name) {
      if (index != nullptr) {
        *index = i;
      }
      return true;
    }
  }
  return false;
}

static bool IsDirectFunctionGenericTypeParameter(
    const FunctionInfo &fn, const SemanticTypeInfo &type,
    std::size_t *index = nullptr) {
  const std::string &name = type.canonical_type.object_pointer_type_name.empty()
                                ? type.object_pointer_type_name
                                : type.canonical_type.object_pointer_type_name;
  return !name.empty() && !type.canonical_type.has_generic_suffix &&
         IsFunctionGenericParameterName(fn, name, index);
}

static bool AreGenericFunctionBoundTypesEquivalent(
    const SemanticTypeInfo &lhs, const SemanticTypeInfo &rhs) {
  if (IsObjCReferenceSemanticType(lhs) || IsObjCReferenceSemanticType(rhs)) {
    return lhs.type == rhs.type &&
           lhs.canonical_type.object_pointer_type_name ==
               rhs.canonical_type.object_pointer_type_name &&
           lhs.canonical_type.generic_arguments_source_order ==
               rhs.canonical_type.generic_arguments_source_order &&
           lhs.canonical_type.protocol_composition_lexicographic ==
               rhs.canonical_type.protocol_composition_lexicographic &&
           lhs.canonical_type.nullability == rhs.canonical_type.nullability;
  }
  return IsSameSemanticType(lhs, rhs);
}

static bool GenericFunctionArgumentSatisfiesProtocolConstraint(
    const Objc3SemanticIntegrationSurface *surface,
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        *protocol_definitions,
    const SemanticTypeInfo &argument,
    const std::string &required_protocol) {
  if (surface == nullptr || protocol_definitions == nullptr) {
    return false;
  }
  if (argument.canonical_type.has_protocol_composition) {
    for (const auto &protocol_name :
         argument.canonical_type.protocol_composition_lexicographic) {
      std::unordered_set<std::string> active_protocols;
      if (ProtocolSatisfiesConstraint(*protocol_definitions, protocol_name,
                                      required_protocol, active_protocols)) {
        return true;
      }
    }
  }
  const std::string &object_name =
      argument.canonical_type.object_pointer_type_name.empty()
          ? argument.object_pointer_type_name
          : argument.canonical_type.object_pointer_type_name;
  const auto interface_it = surface->interfaces.find(object_name);
  if (interface_it == surface->interfaces.end()) {
    return false;
  }
  for (const auto &adopted_protocol :
       interface_it->second.adopted_protocols_lexicographic) {
    std::unordered_set<std::string> active_protocols;
    if (ProtocolSatisfiesConstraint(*protocol_definitions, adopted_protocol,
                                    required_protocol, active_protocols)) {
      return true;
    }
  }
  return false;
}

static void ValidateGenericFunctionBindingConstraints(
    const FunctionInfo &fn,
    const std::string &function_name,
    std::size_t generic_index,
    const SemanticTypeInfo &binding,
    const Objc3MessageSendResolutionContext &message_send_context,
    unsigned line,
    unsigned column,
    std::vector<std::string> &diagnostics) {
  if (generic_index >= fn.generic_parameter_names_source_order.size() ||
      generic_index >= fn.generic_parameter_constraints_lexicographic.size()) {
    return;
  }
  const std::string &parameter_name =
      fn.generic_parameter_names_source_order[generic_index];
  for (const auto &required_protocol :
       fn.generic_parameter_constraints_lexicographic[generic_index]) {
    if (!GenericFunctionArgumentSatisfiesProtocolConstraint(
            message_send_context.surface,
            message_send_context.protocol_definitions, binding,
            required_protocol)) {
      diagnostics.push_back(MakeDiag(
          line, column, "O3S206",
          "type mismatch: generic argument for parameter '" +
              parameter_name + "' of function '" + function_name +
              "' does not satisfy protocol constraint '" + required_protocol +
              "'"));
    }
  }
}

static SemanticTypeInfo SubstituteGenericFunctionTypeParameters(
    const FunctionInfo &fn,
    const SemanticTypeInfo &type,
    const std::unordered_map<std::string, SemanticTypeInfo> &bindings) {
  std::size_t generic_index = 0;
  if (!IsDirectFunctionGenericTypeParameter(fn, type, &generic_index) ||
      generic_index >= fn.generic_parameter_names_source_order.size()) {
    return type;
  }
  const auto binding_it =
      bindings.find(fn.generic_parameter_names_source_order[generic_index]);
  if (binding_it == bindings.end()) {
    return MakeScalarSemanticType(ValueType::Unknown);
  }
  return binding_it->second;
}

static void BindGenericFunctionParameterFromArgument(
    const FunctionInfo &fn,
    const std::string &function_name,
    std::size_t param_index,
    const SemanticTypeInfo &expected,
    const SemanticTypeInfo &argument,
    const Objc3MessageSendResolutionContext &message_send_context,
    unsigned line,
    unsigned column,
    std::unordered_map<std::string, SemanticTypeInfo> &bindings,
    std::vector<std::string> &diagnostics) {
  std::size_t generic_index = 0;
  if (!IsDirectFunctionGenericTypeParameter(fn, expected, &generic_index) ||
      generic_index >= fn.generic_parameter_names_source_order.size()) {
    return;
  }
  const std::string &generic_name =
      fn.generic_parameter_names_source_order[generic_index];
  const auto binding_it = bindings.find(generic_name);
  if (binding_it == bindings.end()) {
    bindings.emplace(generic_name, argument);
    ValidateGenericFunctionBindingConstraints(
        fn, function_name, generic_index, argument, message_send_context, line,
        column, diagnostics);
    return;
  }
  if (!AreGenericFunctionBoundTypesEquivalent(binding_it->second, argument)) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S206",
        "type mismatch: generic parameter '" + generic_name +
            "' of function '" + function_name +
            "' was already inferred as '" +
            SemanticTypeName(binding_it->second) + "' before parameter " +
            std::to_string(param_index) + ", got '" +
            SemanticTypeName(argument) + "'"));
  }
}

static void DiagnoseUnresolvedGenericFunctionReturn(
    const FunctionInfo &fn,
    const std::string &function_name,
    const SemanticTypeInfo &return_type,
    const std::unordered_map<std::string, SemanticTypeInfo> &bindings,
    unsigned line,
    unsigned column,
    std::vector<std::string> &diagnostics) {
  std::size_t generic_index = 0;
  if (!IsDirectFunctionGenericTypeParameter(fn, return_type, &generic_index) ||
      generic_index >= fn.generic_parameter_names_source_order.size()) {
    return;
  }
  const std::string &generic_name =
      fn.generic_parameter_names_source_order[generic_index];
  if (bindings.find(generic_name) == bindings.end()) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S206",
        "type mismatch: generic return parameter '" + generic_name +
            "' of function '" + function_name +
            "' cannot be inferred from call arguments"));
  }
}

