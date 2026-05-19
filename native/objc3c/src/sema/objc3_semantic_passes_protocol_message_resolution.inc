static Objc3ProtocolQualifiedMessageRequirementResolution
ResolveProtocolQualifiedMessageRequirement(
    const Objc3SemanticCanonicalType &receiver_type,
    const std::string &selector,
    bool is_class_method,
    const Objc3MessageSendResolutionContext &context) {
  Objc3ProtocolQualifiedMessageRequirementResolution resolution;
  if (!receiver_type.has_protocol_composition ||
      receiver_type.protocol_composition_lexicographic.empty()) {
    return resolution;
  }
  if (context.protocol_definitions == nullptr) {
    resolution.status =
        Objc3ProtocolQualifiedMessageRequirementResolution::Status::
            NotProtocolQualified;
    return resolution;
  }

  resolution.status =
      Objc3ProtocolQualifiedMessageRequirementResolution::Status::
          MissingSelector;
  const std::string requirement_key =
      BuildProtocolRequirementKey(selector, is_class_method);
  const auto consider_property =
      [&](const Objc3ProtocolPropertyRequirementInfo *candidate,
          bool is_setter) {
        if (candidate == nullptr ||
            candidate->property.is_class != is_class_method) {
          return true;
        }
        if (resolution.property_requirement != nullptr) {
          if (resolution.property_requirement_is_setter != is_setter ||
              !IsCompatiblePropertySignature(
                  resolution.property_requirement->property,
                  candidate->property)) {
            resolution.status =
                Objc3ProtocolQualifiedMessageRequirementResolution::Status::
                    AmbiguousSelector;
            return false;
          }
          return true;
        }
        if (resolution.requirement != nullptr) {
          const Objc3MethodInfo &method = resolution.requirement->method;
          const bool compatible_getter =
              !is_setter && method.arity == 0u &&
              IsCompatibleCanonicalSemanticType(method.return_canonical_type,
                                                candidate->property.canonical_type);
          const bool compatible_setter =
              is_setter && method.arity == 1u &&
              !method.param_canonical_types.empty() &&
              IsCompatibleCanonicalSemanticType(
                  method.param_canonical_types.front(),
                  candidate->property.canonical_type);
          if (!compatible_getter && !compatible_setter) {
            resolution.status =
                Objc3ProtocolQualifiedMessageRequirementResolution::Status::
                    AmbiguousSelector;
            return false;
          }
          return true;
        }
        resolution.property_requirement = candidate;
        resolution.property_requirement_is_setter = is_setter;
        resolution.protocol_name = candidate->protocol_name;
        return true;
      };
  const auto consider_property_map =
      [&](const std::unordered_map<
          std::string, const Objc3ProtocolPropertyRequirementInfo *> &properties) {
        for (const auto &entry : properties) {
          const Objc3ProtocolPropertyRequirementInfo *property = entry.second;
          if (property == nullptr) {
            continue;
          }
          if (property->property.effective_getter_selector == selector) {
            if (!consider_property(property, false)) {
              return false;
            }
          }
          if (property->property.effective_setter_available &&
              property->property.effective_setter_selector == selector) {
            if (!consider_property(property, true)) {
              return false;
            }
          }
        }
        return true;
      };
  for (const auto &protocol_name :
       receiver_type.protocol_composition_lexicographic) {
    Objc3ProtocolRequirementClosure closure;
    std::unordered_set<std::string> active_protocols;
    if (!CollectProtocolRequirementClosure(*context.protocol_definitions,
                                          protocol_name, active_protocols,
                                          closure)) {
      resolution.status =
          Objc3ProtocolQualifiedMessageRequirementResolution::Status::
              MissingRequirementClosure;
      resolution.failing_protocol_name = closure.failing_protocol_name.empty()
                                             ? protocol_name
                                             : closure.failing_protocol_name;
      return resolution;
    }

    const Objc3ProtocolRequirementInfo *candidate = nullptr;
    const auto required_it =
        closure.required_methods_by_key.find(requirement_key);
    if (required_it != closure.required_methods_by_key.end()) {
      candidate = required_it->second;
    } else {
      const auto optional_it =
          closure.optional_methods_by_key.find(requirement_key);
      if (optional_it != closure.optional_methods_by_key.end()) {
        candidate = optional_it->second;
      }
    }
    if (candidate == nullptr) {
      if (!consider_property_map(closure.required_properties_by_name) ||
          !consider_property_map(closure.optional_properties_by_name)) {
        resolution.protocol_name = protocol_name;
        return resolution;
      }
      continue;
    }
    if (resolution.requirement != nullptr &&
        !IsCompatibleMethodSignature(resolution.requirement->method,
                                     candidate->method)) {
      resolution.status =
          Objc3ProtocolQualifiedMessageRequirementResolution::Status::
              AmbiguousSelector;
      resolution.protocol_name = protocol_name;
      return resolution;
    }
    if (resolution.property_requirement != nullptr) {
      const Objc3PropertyInfo &property =
          resolution.property_requirement->property;
      const bool compatible_getter =
          !resolution.property_requirement_is_setter && candidate->method.arity == 0u &&
          IsCompatibleCanonicalSemanticType(candidate->method.return_canonical_type,
                                            property.canonical_type);
      const bool compatible_setter =
          resolution.property_requirement_is_setter &&
          candidate->method.arity == 1u &&
          !candidate->method.param_canonical_types.empty() &&
          IsCompatibleCanonicalSemanticType(
              candidate->method.param_canonical_types.front(),
              property.canonical_type);
      if (!compatible_getter && !compatible_setter) {
        resolution.status =
            Objc3ProtocolQualifiedMessageRequirementResolution::Status::
                AmbiguousSelector;
        resolution.protocol_name = protocol_name;
        return resolution;
      }
    }
    if (resolution.requirement == nullptr &&
        resolution.property_requirement == nullptr) {
      resolution.requirement = candidate;
      resolution.protocol_name = candidate->protocol_name;
    }
  }
  if (resolution.requirement != nullptr ||
      resolution.property_requirement != nullptr) {
    resolution.status =
        Objc3ProtocolQualifiedMessageRequirementResolution::Status::Resolved;
  }
  return resolution;
}
