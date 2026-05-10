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
