struct Objc3InterfaceGenericParameterDefinition {
  std::string name;
  std::string variance_spelling;
  std::vector<std::string> constraint_protocols_lexicographic;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3InterfaceGenericDefinition {
  std::string name;
  std::vector<Objc3InterfaceGenericParameterDefinition> parameters;
};

static std::unordered_map<std::string, Objc3InterfaceGenericDefinition>
BuildInterfaceGenericDefinitions(
    const Objc3Program &ast,
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &protocol_definitions,
    std::vector<std::string> &diagnostics) {
  std::unordered_map<std::string, Objc3InterfaceGenericDefinition> definitions;
  for (const auto &interface_decl : ast.interfaces) {
    if (interface_decl.has_category || interface_decl.generic_params.empty()) {
      continue;
    }
    Objc3InterfaceGenericDefinition definition;
    definition.name = interface_decl.name;
    std::unordered_set<std::string> seen_names;
    for (const auto &param_decl : interface_decl.generic_params) {
      if (!seen_names.insert(param_decl.name).second) {
        diagnostics.push_back(MakeDiag(
            param_decl.line, param_decl.column, "O3S200",
            "duplicate generic parameter '" + param_decl.name +
                "' in interface '" + interface_decl.name + "'"));
        continue;
      }
      Objc3InterfaceGenericParameterDefinition param;
      param.name = param_decl.name;
      param.variance_spelling = param_decl.variance_spelling;
      param.line = param_decl.line;
      param.column = param_decl.column;
      if (param_decl.has_constraint) {
        if (param_decl.constraint_type_name != "id" ||
            !param_decl.has_constraint_generic_suffix) {
          diagnostics.push_back(MakeDiag(
              param_decl.constraint_line, param_decl.constraint_column,
              "O3S206",
              "type mismatch: generic parameter '" + param_decl.name +
                  "' in interface '" + interface_decl.name +
                  "' requires an id<Protocol> constraint"));
        } else {
          const ProtocolCompositionParseResult parsed =
              ParseProtocolCompositionSuffixText(
                  param_decl.constraint_generic_suffix_text);
          if (!parsed.IsValid()) {
            diagnostics.push_back(MakeDiag(
                param_decl.constraint_line, param_decl.constraint_column,
                "O3S206",
                "type mismatch: generic parameter '" + param_decl.name +
                    "' in interface '" + interface_decl.name +
                    "' has an invalid protocol constraint"));
          } else {
            param.constraint_protocols_lexicographic =
                parsed.names_lexicographic;
            for (const auto &protocol_name :
                 param.constraint_protocols_lexicographic) {
              if (protocol_definitions.find(protocol_name) ==
                  protocol_definitions.end()) {
                diagnostics.push_back(MakeDiag(
                    param_decl.constraint_line, param_decl.constraint_column,
                    "O3S206",
                    "type mismatch: generic parameter '" + param_decl.name +
                        "' in interface '" + interface_decl.name +
                        "' references unknown constraint protocol '" +
                        protocol_name + "'"));
              }
            }
          }
        }
      }
      definition.parameters.push_back(std::move(param));
    }
    definitions[definition.name] = std::move(definition);
  }
  return definitions;
}
