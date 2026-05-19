static bool CollectProtocolRequirementClosure(
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &definitions,
    const std::string &protocol_name,
    std::unordered_set<std::string> &active_protocols,
    Objc3ProtocolRequirementClosure &closure) {
  const auto definition_it = definitions.find(protocol_name);
  if (definition_it == definitions.end()) {
    closure.status = Objc3ProtocolRequirementClosureStatus::MissingProtocol;
    closure.failing_protocol_name = protocol_name;
    return false;
  }
  const Objc3ProtocolSemanticDefinition &definition = definition_it->second;
  if (definition.is_forward_declaration) {
    closure.status = Objc3ProtocolRequirementClosureStatus::ForwardDeclaration;
    closure.failing_protocol_name = protocol_name;
    return false;
  }
  if (!active_protocols.insert(protocol_name).second) {
    closure.status = Objc3ProtocolRequirementClosureStatus::InheritanceCycle;
    closure.failing_protocol_name = protocol_name;
    return false;
  }

  for (const auto &inherited_protocol_name :
       definition.inherited_protocols_lexicographic) {
    if (!CollectProtocolRequirementClosure(definitions, inherited_protocol_name,
                                          active_protocols, closure)) {
      active_protocols.erase(protocol_name);
      return false;
    }
  }

  for (const auto &entry : definition.required_properties_by_name) {
    const std::string &property_name = entry.first;
    const Objc3ProtocolPropertyRequirementInfo &requirement = entry.second;
    const auto existing_it =
        closure.required_properties_by_name.find(property_name);
    if (existing_it != closure.required_properties_by_name.end()) {
      if (!IsCompatiblePropertySignature(existing_it->second->property,
                                         requirement.property)) {
        closure.status = Objc3ProtocolRequirementClosureStatus::
            IncompatibleInheritedRequirement;
        closure.failing_protocol_name = protocol_name;
        closure.conflicting_member_name = requirement.property_name;
        closure.conflicting_member_is_method = false;
        closure.conflicting_selector_is_class_method = false;
        active_protocols.erase(protocol_name);
        return false;
      }
      continue;
    }
    const auto existing_optional_it =
        closure.optional_properties_by_name.find(property_name);
    if (existing_optional_it != closure.optional_properties_by_name.end() &&
        !IsCompatiblePropertySignature(existing_optional_it->second->property,
                                       requirement.property)) {
      closure.status = Objc3ProtocolRequirementClosureStatus::
          IncompatibleInheritedRequirement;
      closure.failing_protocol_name = protocol_name;
      closure.conflicting_member_name = requirement.property_name;
      closure.conflicting_member_is_method = false;
      closure.conflicting_selector_is_class_method = false;
      active_protocols.erase(protocol_name);
      return false;
    }
    closure.required_properties_by_name.emplace(property_name, &requirement);
  }

  for (const auto &entry : definition.optional_properties_by_name) {
    const std::string &property_name = entry.first;
    const Objc3ProtocolPropertyRequirementInfo &requirement = entry.second;
    const auto existing_required_it =
        closure.required_properties_by_name.find(property_name);
    if (existing_required_it != closure.required_properties_by_name.end()) {
      if (!IsCompatiblePropertySignature(existing_required_it->second->property,
                                         requirement.property)) {
        closure.status = Objc3ProtocolRequirementClosureStatus::
            IncompatibleInheritedRequirement;
        closure.failing_protocol_name = protocol_name;
        closure.conflicting_member_name = requirement.property_name;
        closure.conflicting_member_is_method = false;
        closure.conflicting_selector_is_class_method = false;
        active_protocols.erase(protocol_name);
        return false;
      }
      continue;
    }
    const auto existing_optional_it =
        closure.optional_properties_by_name.find(property_name);
    if (existing_optional_it != closure.optional_properties_by_name.end()) {
      if (!IsCompatiblePropertySignature(existing_optional_it->second->property,
                                         requirement.property)) {
        closure.status = Objc3ProtocolRequirementClosureStatus::
            IncompatibleInheritedRequirement;
        closure.failing_protocol_name = protocol_name;
        closure.conflicting_member_name = requirement.property_name;
        closure.conflicting_member_is_method = false;
        closure.conflicting_selector_is_class_method = false;
        active_protocols.erase(protocol_name);
        return false;
      }
      continue;
    }
    closure.optional_properties_by_name.emplace(property_name, &requirement);
  }

  for (const auto &entry : definition.required_methods_by_key) {
    const std::string &requirement_key = entry.first;
    const Objc3ProtocolRequirementInfo &requirement = entry.second;
    const auto existing_it =
        closure.required_methods_by_key.find(requirement_key);
    if (existing_it != closure.required_methods_by_key.end()) {
      if (!IsCompatibleMethodSignature(existing_it->second->method,
                                       requirement.method)) {
        closure.status = Objc3ProtocolRequirementClosureStatus::
            IncompatibleInheritedRequirement;
        closure.failing_protocol_name = protocol_name;
        closure.conflicting_member_name = requirement.selector;
        closure.conflicting_member_is_method = true;
        closure.conflicting_selector_is_class_method =
            requirement.method.is_class_method;
        active_protocols.erase(protocol_name);
        return false;
      }
      continue;
    }
    const auto existing_optional_it =
        closure.optional_methods_by_key.find(requirement_key);
    if (existing_optional_it != closure.optional_methods_by_key.end() &&
        !IsCompatibleMethodSignature(existing_optional_it->second->method,
                                     requirement.method)) {
      closure.status = Objc3ProtocolRequirementClosureStatus::
          IncompatibleInheritedRequirement;
      closure.failing_protocol_name = protocol_name;
      closure.conflicting_member_name = requirement.selector;
      closure.conflicting_member_is_method = true;
      closure.conflicting_selector_is_class_method =
          requirement.method.is_class_method;
      active_protocols.erase(protocol_name);
      return false;
    }
    closure.required_methods_by_key.emplace(requirement_key, &requirement);
  }

  for (const auto &entry : definition.optional_methods_by_key) {
    const std::string &requirement_key = entry.first;
    const Objc3ProtocolRequirementInfo &requirement = entry.second;
    const auto existing_required_it =
        closure.required_methods_by_key.find(requirement_key);
    if (existing_required_it != closure.required_methods_by_key.end()) {
      if (!IsCompatibleMethodSignature(existing_required_it->second->method,
                                       requirement.method)) {
        closure.status = Objc3ProtocolRequirementClosureStatus::
            IncompatibleInheritedRequirement;
        closure.failing_protocol_name = protocol_name;
        closure.conflicting_member_name = requirement.selector;
        closure.conflicting_member_is_method = true;
        closure.conflicting_selector_is_class_method =
            requirement.method.is_class_method;
        active_protocols.erase(protocol_name);
        return false;
      }
      continue;
    }
    const auto existing_optional_it =
        closure.optional_methods_by_key.find(requirement_key);
    if (existing_optional_it != closure.optional_methods_by_key.end()) {
      if (!IsCompatibleMethodSignature(existing_optional_it->second->method,
                                       requirement.method)) {
        closure.status = Objc3ProtocolRequirementClosureStatus::
            IncompatibleInheritedRequirement;
        closure.failing_protocol_name = protocol_name;
        closure.conflicting_member_name = requirement.selector;
        closure.conflicting_member_is_method = true;
        closure.conflicting_selector_is_class_method =
            requirement.method.is_class_method;
        active_protocols.erase(protocol_name);
        return false;
      }
      continue;
    }
    closure.optional_methods_by_key.emplace(requirement_key, &requirement);
  }

  active_protocols.erase(protocol_name);
  return true;
}
