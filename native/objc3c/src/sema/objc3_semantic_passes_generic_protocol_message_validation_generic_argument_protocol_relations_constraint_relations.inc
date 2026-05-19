static bool GenericArgumentSatisfiesProtocolConstraint(
    const Objc3Program &ast,
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &protocol_definitions,
    const std::string &argument,
    const std::string &required_protocol) {
  const std::string trimmed =
      objc3c::support::TrimAsciiWhitespace(argument);
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
