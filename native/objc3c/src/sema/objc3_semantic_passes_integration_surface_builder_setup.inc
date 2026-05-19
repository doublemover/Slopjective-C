  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  Objc3SemanticIntegrationSurface surface;
  std::unordered_map<std::string, int> resolved_global_values;
  Objc3InterfaceImplementationSummary interface_implementation_summary;
  const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
      protocol_definitions =
          BuildProtocolSemanticDefinitions(ast, arc_mode_enabled, diagnostics);
  const std::unordered_map<std::string, Objc3InterfaceGenericDefinition>
      generic_definitions =
          BuildInterfaceGenericDefinitions(ast, protocol_definitions,
                                           diagnostics);
  std::unordered_set<std::string> generic_interface_names;
  for (const auto &entry : generic_definitions) {
    if (!entry.second.parameters.empty()) {
      generic_interface_names.insert(entry.first);
    }
  }
  ValidateInterfaceGenericSpecializations(ast, generic_definitions,
                                          protocol_definitions, diagnostics);
  ValidateProtocolCompositionIdentifierBindings(ast, protocol_definitions,
                                                diagnostics);
  // diagnostic precision anchor: class interface/implementation
  // summaries exclude category containers so valid class-plus-category
  // programs do not degrade into duplicate class-owner diagnostics before the
  // runtime metadata blocker computes attachment collisions and ambiguity.
  interface_implementation_summary.declared_interfaces = static_cast<std::size_t>(
      std::count_if(ast.interfaces.begin(), ast.interfaces.end(),
                    [](const Objc3InterfaceDecl &decl) {
                      return !decl.has_category;
                    }));
  interface_implementation_summary.declared_implementations =
      static_cast<std::size_t>(
          std::count_if(ast.implementations.begin(), ast.implementations.end(),
                        [](const Objc3ImplementationDecl &decl) {
                          return !decl.has_category;
                        }));

  const auto interface_container_label = [](bool has_category) {
    return has_category ? "category interface" : "interface";
  };
  const auto implementation_container_label = [](bool has_category) {
    return has_category ? "category implementation" : "implementation";
  };
