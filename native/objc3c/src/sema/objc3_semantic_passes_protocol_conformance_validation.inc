// protocol-conformance enforcement anchor: sema now consumes the
// parser-owned required/optional partition directly and rejects missing or
// incompatible adopted protocol members before runtime metadata consumption.
static void ValidateDeclaredProtocolConformance(
    const Objc3Program &ast,
    const Objc3SemanticIntegrationSurface &surface,
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &protocol_definitions,
    const std::unordered_map<std::string, Objc3InterfaceInfo> &category_interfaces,
    const std::unordered_map<std::string, Objc3ImplementationInfo>
        &category_implementations,
    std::vector<std::string> &diagnostics) {
  for (const auto &interface_decl : ast.interfaces) {
    if (interface_decl.adopted_protocols_lexicographic.empty()) {
      continue;
    }

    const std::string container_name = FormatObjcContainerName(
        interface_decl.name, interface_decl.has_category,
        interface_decl.category_name);
    const std::string container_label =
        interface_decl.has_category ? "category interface" : "interface";

    const std::vector<std::string> adopted_protocols_lexicographic =
        BuildSortedUniqueStringsLocal(interface_decl.adopted_protocols);
    for (const auto &adopted_protocol_name :
         adopted_protocols_lexicographic) {
      Objc3ProtocolRequirementClosure closure;
      std::unordered_set<std::string> active_protocols;
      if (!CollectProtocolRequirementClosure(protocol_definitions,
                                            adopted_protocol_name,
                                            active_protocols, closure)) {
        std::string detail;
        switch (closure.status) {
        case Objc3ProtocolRequirementClosureStatus::MissingProtocol:
          detail = "adopts unknown protocol '" + closure.failing_protocol_name +
                   "'";
          break;
        case Objc3ProtocolRequirementClosureStatus::ForwardDeclaration:
          detail = "adopts forward-declared protocol '" +
                   closure.failing_protocol_name + "'";
          break;
        case Objc3ProtocolRequirementClosureStatus::InheritanceCycle:
          detail = "adopts protocol '" + closure.failing_protocol_name +
                   "' with cyclic inherited requirements";
          break;
        case Objc3ProtocolRequirementClosureStatus::
            IncompatibleInheritedRequirement:
          if (closure.conflicting_member_is_method) {
            detail = "adopts protocol '" + closure.failing_protocol_name +
                     "' with incompatible required selector '" +
                     FormatMethodSelectorForDiagnostic(
                         closure.conflicting_member_name,
                         closure.conflicting_selector_is_class_method) +
                     "'";
          } else {
            detail = "adopts protocol '" + closure.failing_protocol_name +
                     "' with incompatible required property '" +
                     closure.conflicting_member_name + "'";
          }
          break;
        case Objc3ProtocolRequirementClosureStatus::Ready:
          break;
        }
        diagnostics.push_back(MakeDiag(
            interface_decl.line, interface_decl.column, "O3S218",
            "protocol conformance failure: " + container_label + " '" +
                container_name + "' " + detail));
        continue;
      }

      for (const auto &entry : closure.required_methods_by_key) {
        const Objc3ProtocolRequirementInfo &requirement = *entry.second;
        const Objc3MethodInfo *candidate = nullptr;
        if (interface_decl.has_category) {
          candidate = ResolveCategoryConformanceMethod(
              surface, category_interfaces, category_implementations,
              interface_decl.name, interface_decl.category_name,
              requirement.selector, requirement.method.is_class_method);
        } else {
          candidate = ResolveDeclaredConformanceMethod(
              surface, interface_decl.name, requirement.selector,
              requirement.method.is_class_method);
        }

        if (candidate == nullptr) {
          diagnostics.push_back(MakeDiag(
              interface_decl.line, interface_decl.column, "O3S218",
              "protocol conformance failure: " + container_label + " '" +
                  container_name + "' is missing required selector '" +
                  FormatMethodSelectorForDiagnostic(
                      requirement.selector, requirement.method.is_class_method) +
                  "' from protocol '" + requirement.protocol_name + "'"));
          continue;
        }
        if (!IsCompatibleMethodSignature(*candidate, requirement.method)) {
          diagnostics.push_back(MakeDiag(
              interface_decl.line, interface_decl.column, "O3S218",
              "protocol conformance failure: " + container_label + " '" +
                  container_name + "' selector '" +
                  FormatMethodSelectorForDiagnostic(
                  requirement.selector, requirement.method.is_class_method) +
                  "' is incompatible with protocol '" +
                  requirement.protocol_name + "'"));
        }
      }

      for (const auto &entry : closure.required_properties_by_name) {
        const Objc3ProtocolPropertyRequirementInfo &requirement = *entry.second;
        const Objc3PropertyInfo *candidate = nullptr;
        if (interface_decl.has_category) {
          candidate = ResolveCategoryConformanceProperty(
              surface, category_interfaces, category_implementations,
              interface_decl.name, interface_decl.category_name,
              requirement.property_name);
        } else {
          candidate = ResolveDeclaredConformanceProperty(
              surface, interface_decl.name, requirement.property_name);
        }

        if (candidate == nullptr) {
          diagnostics.push_back(MakeDiag(
              interface_decl.line, interface_decl.column, "O3S218",
              "protocol conformance failure: " + container_label + " '" +
                  container_name + "' is missing required property '" +
                  requirement.property_name + "' from protocol '" +
                  requirement.protocol_name + "'"));
          continue;
        }
        if (!IsCompatiblePropertySignature(*candidate, requirement.property)) {
          diagnostics.push_back(MakeDiag(
              interface_decl.line, interface_decl.column, "O3S218",
              "protocol conformance failure: " + container_label + " '" +
                  container_name + "' property '" +
                  requirement.property_name +
                  "' is incompatible with protocol '" +
                  requirement.protocol_name + "'"));
        }
      }
    }
  }
}
