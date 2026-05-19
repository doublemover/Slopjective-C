static bool IsGenericInterfaceSpecializationSuffix(
    bool object_pointer_type_spelling,
    const std::string &object_pointer_type_name,
    const std::unordered_set<std::string> *generic_interface_names);

static ProtocolCompositionInfo BuildProtocolCompositionInfoFromParam(
    const FuncParam &param,
    const std::unordered_set<std::string> *generic_interface_names = nullptr) {
  ProtocolCompositionInfo info;
  if (!param.has_generic_suffix) {
    return info;
  }
  if (IsGenericInterfaceSpecializationSuffix(
          param.object_pointer_type_spelling, param.object_pointer_type_name,
          generic_interface_names)) {
    return info;
  }

  const ProtocolCompositionParseResult parsed = ParseProtocolCompositionSuffixText(param.generic_suffix_text);
  info.has_protocol_composition = true;
  info.names_lexicographic = parsed.names_lexicographic;
  info.has_invalid_protocol_composition = !SupportsGenericParamTypeSuffix(param) || !parsed.IsValid();
  return info;
}

static ProtocolCompositionInfo BuildProtocolCompositionInfoFromFunctionReturn(
    const FunctionDecl &fn,
    const std::unordered_set<std::string> *generic_interface_names = nullptr) {
  ProtocolCompositionInfo info;
  if (!fn.has_return_generic_suffix) {
    return info;
  }
  if (IsGenericInterfaceSpecializationSuffix(
          fn.return_object_pointer_type_spelling,
          fn.return_object_pointer_type_name, generic_interface_names)) {
    return info;
  }

  const ProtocolCompositionParseResult parsed = ParseProtocolCompositionSuffixText(fn.return_generic_suffix_text);
  info.has_protocol_composition = true;
  info.names_lexicographic = parsed.names_lexicographic;
  info.has_invalid_protocol_composition = !SupportsGenericReturnTypeSuffix(fn) || !parsed.IsValid();
  return info;
}

static ProtocolCompositionInfo BuildProtocolCompositionInfoFromMethodReturn(
    const Objc3MethodDecl &method,
    const std::unordered_set<std::string> *generic_interface_names = nullptr) {
  ProtocolCompositionInfo info;
  if (!method.has_return_generic_suffix) {
    return info;
  }
  if (IsGenericInterfaceSpecializationSuffix(
          method.return_object_pointer_type_spelling,
          method.return_object_pointer_type_name, generic_interface_names)) {
    return info;
  }

  const ProtocolCompositionParseResult parsed = ParseProtocolCompositionSuffixText(method.return_generic_suffix_text);
  info.has_protocol_composition = true;
  info.names_lexicographic = parsed.names_lexicographic;
  info.has_invalid_protocol_composition = !SupportsGenericReturnTypeSuffix(method) || !parsed.IsValid();
  return info;
}
