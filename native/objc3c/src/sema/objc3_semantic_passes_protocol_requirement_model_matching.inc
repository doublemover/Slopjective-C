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
