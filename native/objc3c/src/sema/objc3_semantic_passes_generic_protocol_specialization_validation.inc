static void ValidateGenericSpecializationType(
    const Objc3Program &ast,
    const std::unordered_map<std::string, Objc3InterfaceGenericDefinition>
        &generic_definitions,
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &protocol_definitions,
    const std::string &object_pointer_type_name, bool has_generic_suffix,
    const std::string &generic_suffix_text, unsigned line, unsigned column,
    const std::string &context, std::vector<std::string> &diagnostics,
    unsigned recursion_depth = 0u) {
  if (recursion_depth > 16u) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S206",
        "type mismatch: generic specialization for " + context +
            " exceeds supported nested generic depth"));
    return;
  }
  const auto definition_it = generic_definitions.find(object_pointer_type_name);
  if (definition_it == generic_definitions.end()) {
    return;
  }
  const Objc3InterfaceGenericDefinition &definition = definition_it->second;
  if (!has_generic_suffix) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S206",
        "type mismatch: generic interface '" + object_pointer_type_name +
            "' used by " + context + " requires " +
            std::to_string(definition.parameters.size()) +
            " generic argument(s)"));
    return;
  }
  const std::vector<std::string> arguments =
      ParseGenericArgumentSuffixTextSourceOrder(generic_suffix_text);
  if (arguments.size() != definition.parameters.size()) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S206",
        "type mismatch: generic interface '" + object_pointer_type_name +
            "' used by " + context + " expects " +
            std::to_string(definition.parameters.size()) +
            " generic argument(s), got " + std::to_string(arguments.size())));
    return;
  }
  for (std::size_t index = 0; index < arguments.size(); ++index) {
    const auto &param = definition.parameters[index];
    if (IsProtocolQualifiedGenericArgument(arguments[index])) {
      if (!ValidateProtocolQualifiedGenericArgument(
              protocol_definitions, arguments[index], object_pointer_type_name,
              param.name, line, column, diagnostics)) {
        continue;
      }
    } else {
      std::string nested_object_name;
      std::string nested_generic_suffix_text;
      if (!ExtractGenericArgumentSpecialization(arguments[index],
                                               nested_object_name,
                                               nested_generic_suffix_text)) {
        diagnostics.push_back(MakeDiag(
            line, column, "O3S206",
            "type mismatch: generic argument '" + arguments[index] +
                "' for parameter '" + param.name + "' of interface '" +
                object_pointer_type_name +
                "' has an unterminated nested generic specialization"));
      } else if (!nested_object_name.empty() &&
                 generic_definitions.find(nested_object_name) !=
                     generic_definitions.end()) {
        ValidateGenericSpecializationType(
            ast, generic_definitions, protocol_definitions, nested_object_name,
            !nested_generic_suffix_text.empty(), nested_generic_suffix_text,
            line, column,
            "nested generic argument '" + arguments[index] + "' in " + context,
            diagnostics, recursion_depth + 1u);
      }
    }
    for (const auto &required_protocol :
         param.constraint_protocols_lexicographic) {
      if (!GenericArgumentSatisfiesProtocolConstraint(
              ast, protocol_definitions, arguments[index],
              required_protocol)) {
        diagnostics.push_back(MakeDiag(
            line, column, "O3S206",
            "type mismatch: generic argument '" + arguments[index] +
                "' for parameter '" + param.name + "' of interface '" +
                object_pointer_type_name +
                "' does not satisfy protocol constraint '" +
                required_protocol + "'"));
      }
    }
  }
}

static void ValidateInterfaceGenericSpecializations(
    const Objc3Program &ast,
    const std::unordered_map<std::string, Objc3InterfaceGenericDefinition>
        &generic_definitions,
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &protocol_definitions,
    std::vector<std::string> &diagnostics) {
  auto validate_param = [&](const FuncParam &param,
                            const std::string &context) {
    if (!param.object_pointer_type_spelling) {
      return;
    }
    ValidateGenericSpecializationType(
        ast, generic_definitions, protocol_definitions,
        param.object_pointer_type_name, param.has_generic_suffix,
        param.generic_suffix_text, param.line, param.column, context,
        diagnostics);
  };
  auto validate_method = [&](const Objc3MethodDecl &method,
                             const std::string &context) {
    if (method.return_object_pointer_type_spelling) {
      ValidateGenericSpecializationType(
          ast, generic_definitions, protocol_definitions,
          method.return_object_pointer_type_name,
          method.has_return_generic_suffix,
          method.return_generic_suffix_text, method.line,
          method.column, context + " return type", diagnostics);
    }
    for (const auto &param : method.params) {
      validate_param(param, context + " parameter '" + param.name + "'");
    }
  };
  auto validate_property = [&](const Objc3PropertyDecl &property,
                               const std::string &context) {
    if (!property.object_pointer_type_spelling) {
      return;
    }
    ValidateGenericSpecializationType(
        ast, generic_definitions, protocol_definitions,
        property.object_pointer_type_name, property.has_generic_suffix,
        property.generic_suffix_text, property.line, property.column, context,
        diagnostics);
  };

  for (const auto &fn : ast.functions) {
    if (fn.return_object_pointer_type_spelling) {
      ValidateGenericSpecializationType(
          ast, generic_definitions, protocol_definitions,
          fn.return_object_pointer_type_name, fn.has_return_generic_suffix,
          fn.return_generic_suffix_text, fn.line, fn.column,
          "function '" + fn.name + "' return type", diagnostics);
    }
    for (const auto &param : fn.params) {
      validate_param(param, "function '" + fn.name + "' parameter '" +
                                param.name + "'");
    }
  }
  for (const auto &interface_decl : ast.interfaces) {
    const std::string container_name = FormatObjcContainerName(
        interface_decl.name, interface_decl.has_category,
        interface_decl.category_name);
    for (const auto &property : interface_decl.properties) {
      validate_property(property, "interface '" + container_name +
                                      "' property '" + property.name + "'");
    }
    for (const auto &method : interface_decl.methods) {
      validate_method(method, "interface '" + container_name + "' selector '" +
                                  method.selector + "'");
    }
  }
  for (const auto &implementation_decl : ast.implementations) {
    const std::string container_name = FormatObjcContainerName(
        implementation_decl.name, implementation_decl.has_category,
        implementation_decl.category_name);
    for (const auto &property : implementation_decl.properties) {
      validate_property(property, "implementation '" + container_name +
                                      "' property '" + property.name + "'");
    }
    for (const auto &method : implementation_decl.methods) {
      validate_method(method, "implementation '" + container_name +
                                  "' selector '" +
                                  method.selector + "'");
    }
  }
}
