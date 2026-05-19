struct PropertyAccessorSelectorOwnerState {
  std::string property_name;
  bool owner_marked_conflict = false;
};

static void FinalizePropertyInvalidAttributeContract(Objc3PropertyInfo &property) {
  property.has_invalid_attribute_contract =
      property.has_unknown_attribute || property.has_duplicate_attribute ||
      property.has_readwrite_conflict || property.has_atomicity_conflict ||
      property.has_ownership_conflict ||
      property.has_accessor_selector_contract_violation ||
      property.invalid_attribute_entries > 0 ||
      property.property_contract_violations > 0;
}

static void EmitPropertyContractViolation(Objc3PropertyInfo &property,
                                          unsigned line,
                                          unsigned column,
                                          const std::string &message,
                                          std::vector<std::string> &diagnostics,
                                          bool accessor_selector_violation) {
  if (accessor_selector_violation) {
    property.has_accessor_selector_contract_violation = true;
  }
  diagnostics.push_back(MakeDiag(line, column, "O3S206", message));
  ++property.property_contract_violations;
  FinalizePropertyInvalidAttributeContract(property);
}

static bool EmitImplementationPropertyReflectionAccessorCompatibilityDiagnostics(
    const Objc3PropertyInfo &interface_property,
    Objc3PropertyInfo &implementation_property,
    const std::string &property_name,
    const std::string &implementation_name,
    std::vector<std::string> &diagnostics) {
  bool emitted = false;
  const auto emit = [&](const std::string &message,
                        bool accessor_selector_violation) {
    EmitPropertyContractViolation(implementation_property,
                                  implementation_property.line,
                                  implementation_property.column,
                                  message,
                                  diagnostics,
                                  accessor_selector_violation);
    emitted = true;
  };

  const std::string implementation_label =
      "implementation '" + implementation_name + "'";

  const bool getter_profile_mismatch =
      interface_property.getter_selector != implementation_property.getter_selector ||
      interface_property.effective_getter_selector !=
          implementation_property.effective_getter_selector;
  if (getter_profile_mismatch) {
    emit("type mismatch: effective getter selector profile for property '" +
             property_name + "' in " + implementation_label +
             " drifted from the interface declaration",
         true);
  }

  const bool setter_profile_mismatch =
      interface_property.has_setter != implementation_property.has_setter ||
      interface_property.setter_selector != implementation_property.setter_selector ||
      interface_property.effective_setter_available !=
          implementation_property.effective_setter_available ||
      interface_property.effective_setter_selector !=
          implementation_property.effective_setter_selector;
  if (setter_profile_mismatch) {
    emit("type mismatch: effective setter selector profile for property '" +
             property_name + "' in " + implementation_label +
             " drifted from the interface declaration",
         true);
  }

  const bool reflection_profile_mismatch =
      interface_property.property_attribute_profile !=
          implementation_property.property_attribute_profile ||
      interface_property.ownership_lifetime_profile !=
          implementation_property.ownership_lifetime_profile ||
      interface_property.ownership_runtime_hook_profile !=
          implementation_property.ownership_runtime_hook_profile ||
      interface_property.accessor_ownership_profile !=
          implementation_property.accessor_ownership_profile;
  if (reflection_profile_mismatch) {
    emit("type mismatch: reflected property attribute and ownership profile for property '" +
             property_name + "' in " + implementation_label +
             " drifted from the interface declaration",
         false);
  }

  return emitted;
}

static void ValidatePropertyAccessorSelectorUniqueness(
    Objc3PropertyInfo &property,
    const std::string &property_name,
    const std::string &container_name,
    const std::string &container_label,
    std::unordered_map<std::string, Objc3PropertyInfo> &properties,
    std::unordered_map<std::string, PropertyAccessorSelectorOwnerState>
        &getter_owner_states,
    std::unordered_map<std::string, PropertyAccessorSelectorOwnerState>
        &setter_owner_states,
    std::vector<std::string> &diagnostics) {
  const auto validate_selector =
      [&](const std::string &selector,
          const char *accessor_kind,
          std::unordered_map<std::string, PropertyAccessorSelectorOwnerState>
              &owner_states) {
        if (selector.empty()) {
          return;
        }

        auto owner_it = owner_states.find(selector);
        if (owner_it == owner_states.end()) {
          owner_states.emplace(selector,
                               PropertyAccessorSelectorOwnerState{
                                   property_name,
                                   false,
                               });
          return;
        }
        if (owner_it->second.property_name == property_name) {
          return;
        }

        const std::string message =
            "type mismatch: duplicate effective " +
            std::string(accessor_kind) + " selector '" + selector +
            "' for properties '" + owner_it->second.property_name + "' and '" +
            property_name + "' in " + container_label + " '" + container_name +
            "'";

        auto previous_property_it = properties.find(owner_it->second.property_name);
        if (previous_property_it != properties.end() &&
            !owner_it->second.owner_marked_conflict) {
          EmitPropertyContractViolation(previous_property_it->second,
                                        previous_property_it->second.line,
                                        previous_property_it->second.column,
                                        message,
                                        diagnostics,
                                        true);
          owner_it->second.owner_marked_conflict = true;
        }

        EmitPropertyContractViolation(property,
                                      property.line,
                                      property.column,
                                      message,
                                      diagnostics,
                                      true);
      };

  if (IsValidPropertyGetterSelector(property.effective_getter_selector)) {
    validate_selector(property.effective_getter_selector,
                      "getter",
                      getter_owner_states);
  }
  if (property.effective_setter_available &&
      IsValidPropertySetterSelector(property.effective_setter_selector)) {
    validate_selector(property.effective_setter_selector,
                      "setter",
                      setter_owner_states);
  }
}
