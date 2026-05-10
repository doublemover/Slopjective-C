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
