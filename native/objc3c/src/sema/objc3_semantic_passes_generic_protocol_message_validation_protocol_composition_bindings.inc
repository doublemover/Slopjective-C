static void ValidateProtocolCompositionIdentifierBindings(
    const Objc3Program &ast,
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &definitions,
    std::vector<std::string> &diagnostics) {
  auto validate_suffix =
      [&](bool has_generic_suffix, bool supports_generic_suffix,
          const std::string &suffix_text, unsigned line, unsigned column,
          const std::string &context) {
        if (!has_generic_suffix || !supports_generic_suffix) {
          return;
        }
        const ProtocolCompositionParseResult parsed =
            ParseProtocolCompositionSuffixText(suffix_text);
        if (!parsed.IsValid()) {
          return;
        }
        for (const auto &protocol_name : parsed.names_lexicographic) {
          if (definitions.find(protocol_name) == definitions.end()) {
            diagnostics.push_back(MakeDiag(
                line, column, "O3S206",
                "type mismatch: protocol-qualified type for " + context +
                    " references unknown protocol '" + protocol_name + "'"));
          }
        }
      };

  auto validate_property = [&](const Objc3PropertyDecl &property,
                               const std::string &context) {
    validate_suffix(property.has_generic_suffix,
                    SupportsGenericPropertyTypeSuffix(property),
                    property.generic_suffix_text, property.generic_line,
                    property.generic_column, context);
  };

  auto validate_method = [&](const Objc3MethodDecl &method,
                             const std::string &context) {
    validate_suffix(method.has_return_generic_suffix,
                    SupportsGenericReturnTypeSuffix(method),
                    method.return_generic_suffix_text,
                    method.return_generic_line, method.return_generic_column,
                    context + " return type");
    for (const auto &param : method.params) {
      validate_suffix(param.has_generic_suffix,
                      SupportsGenericParamTypeSuffix(param),
                      param.generic_suffix_text, param.generic_line,
                      param.generic_column,
                      context + " parameter '" + param.name + "'");
    }
  };

  for (const auto &fn : ast.functions) {
    validate_suffix(fn.has_return_generic_suffix,
                    SupportsGenericReturnTypeSuffix(fn),
                    fn.return_generic_suffix_text, fn.return_generic_line,
                    fn.return_generic_column,
                    "function '" + fn.name + "' return type");
    for (const auto &param : fn.params) {
      validate_suffix(param.has_generic_suffix,
                      SupportsGenericParamTypeSuffix(param),
                      param.generic_suffix_text, param.generic_line,
                      param.generic_column,
                      "function '" + fn.name + "' parameter '" +
                          param.name + "'");
    }
  }

  for (const auto &protocol_decl : ast.protocols) {
    for (const auto &property : protocol_decl.properties) {
      validate_property(property, "protocol '" + protocol_decl.name +
                                      "' property '" + property.name + "'");
    }
    for (const auto &method : protocol_decl.methods) {
      validate_method(method, "protocol '" + protocol_decl.name +
                                  "' selector '" + MethodSelectorName(method) +
                                  "'");
    }
  }

  for (const auto &interface_decl : ast.interfaces) {
    const std::string container_name = FormatObjcContainerName(
        interface_decl.name, interface_decl.has_category,
        interface_decl.category_name);
    const std::string container_label =
        interface_decl.has_category ? "category interface" : "interface";
    for (const auto &property : interface_decl.properties) {
      validate_property(property, container_label + " '" + container_name +
                                      "' property '" + property.name + "'");
    }
    for (const auto &method : interface_decl.methods) {
      validate_method(method, container_label + " '" + container_name +
                                  "' selector '" + MethodSelectorName(method) +
                                  "'");
    }
  }

  for (const auto &implementation_decl : ast.implementations) {
    const std::string container_name = FormatObjcContainerName(
        implementation_decl.name, implementation_decl.has_category,
        implementation_decl.category_name);
    const std::string container_label = implementation_decl.has_category
                                            ? "category implementation"
                                            : "implementation";
    for (const auto &property : implementation_decl.properties) {
      validate_property(property, container_label + " '" + container_name +
                                      "' property '" + property.name + "'");
    }
    for (const auto &method : implementation_decl.methods) {
      validate_method(method, container_label + " '" + container_name +
                                  "' selector '" + MethodSelectorName(method) +
                                  "'");
    }
  }
}
