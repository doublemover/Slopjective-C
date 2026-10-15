static void ValidateMethodParameterTypeSuffixes(const Objc3MethodDecl &method,
                                                const std::string &owner_name,
                                                const std::string &owner_kind,
                                                std::vector<std::string> &diagnostics,
                                                const std::unordered_set<std::string> *generic_interface_names = nullptr) {
  const std::string selector = MethodSelectorName(method);
  for (const auto &param : method.params) {
    if (param.has_generic_suffix && !SupportsGenericParamTypeSuffix(param)) {
      std::string suffix = param.generic_suffix_text;
      if (suffix.empty()) {
        suffix = "<...>";
      }
      diagnostics.push_back(MakeDiag(param.generic_line, param.generic_column, "O3S206",
                                     "type mismatch: generic parameter type suffix '" + suffix +
                                         "' is unsupported for selector '" + selector + "' parameter '" + param.name +
                                         "' in " + owner_kind + " '" + owner_name + "'"));
    } else if (param.has_generic_suffix && !param.value_optional.present &&
               !ValidateGenericCollectionSpecializationSuffix(
                   param.object_pointer_type_spelling,
                   param.object_pointer_type_name,
                   param.generic_suffix_text, param.generic_line,
                   param.generic_column,
                   "selector '" + selector + "' parameter '" + param.name +
                       "' in " + owner_kind + " '" + owner_name + "'",
                   diagnostics)) {
      ValidateProtocolCompositionSuffix(
          param.generic_suffix_text,
          param.generic_line,
          param.generic_column,
          "selector '" + selector + "' parameter '" + param.name + "' in " + owner_kind + " '" + owner_name + "'",
          IsGenericInterfaceSpecializationSuffix(
              param.object_pointer_type_spelling,
              param.object_pointer_type_name, generic_interface_names),
          diagnostics);
    }
    if (!SupportsPointerParamTypeDeclarator(param)) {
      for (const auto &token : param.pointer_declarator_tokens) {
        diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                       "type mismatch: pointer parameter type declarator '" + token.text +
                                           "' is unsupported for selector '" + selector + "' parameter '" +
                                           param.name + "' in " + owner_kind + " '" + owner_name + "'"));
      }
    }
    if (!SupportsNullabilityParamTypeSuffix(param)) {
      for (const auto &token : param.nullability_suffix_tokens) {
        diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                       "type mismatch: nullability parameter type suffix '" + token.text +
                                           "' is unsupported for selector '" + selector + "' parameter '" +
                                           param.name + "' in " + owner_kind + " '" + owner_name + "'"));
      }
    }
    if (!SupportsOwnershipQualifierParamTypeSuffix(param)) {
      for (const auto &token : param.ownership_qualifier_tokens) {
        diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                       "type mismatch: ownership parameter type qualifier '" + token.text +
                                           "' is unsupported for selector '" + selector + "' parameter '" +
                                           param.name + "' in " + owner_kind + " '" + owner_name + "'"));
      }
    }
  }
}

static void ValidateMethodReturnTypeSuffixes(const Objc3MethodDecl &method,
                                             const std::string &owner_name,
                                             const std::string &owner_kind,
                                             std::vector<std::string> &diagnostics,
                                             const std::unordered_set<std::string> *generic_interface_names = nullptr) {
  const std::string selector = MethodSelectorName(method);
  ValidateGenericCallableParameterList(
      method.generic_params,
      owner_kind + " selector '" + selector + "' in '" + owner_name + "'",
      method.generic_callable_reification_marker_declared,
      method.generic_callable_reification_marker_line,
      method.generic_callable_reification_marker_column,
      diagnostics);
  if (method.has_return_generic_suffix && !SupportsGenericReturnTypeSuffix(method)) {
    std::string suffix = method.return_generic_suffix_text;
    if (suffix.empty()) {
      suffix = "<...>";
    }
    diagnostics.push_back(MakeDiag(method.return_generic_line, method.return_generic_column, "O3S206",
                                   "type mismatch: unsupported method return type suffix '" + suffix +
                                       "' for selector '" + selector + "' in " + owner_kind + " '" + owner_name +
                                       "'"));
  } else if (method.has_return_generic_suffix &&
             !method.return_value_optional.present &&
             !ValidateGenericCollectionSpecializationSuffix(
                 method.return_object_pointer_type_spelling,
                 method.return_object_pointer_type_name,
                 method.return_generic_suffix_text, method.return_generic_line,
                 method.return_generic_column,
                 "selector '" + selector + "' in " + owner_kind + " '" +
                     owner_name + "' return annotation",
                 diagnostics)) {
    ValidateProtocolCompositionSuffix(method.return_generic_suffix_text,
                                     method.return_generic_line,
                                     method.return_generic_column,
                                     "selector '" + selector + "' in " + owner_kind + " '" + owner_name +
                                         "' return annotation",
                                     IsGenericInterfaceSpecializationSuffix(
                                         method.return_object_pointer_type_spelling,
                                         method.return_object_pointer_type_name,
                                         generic_interface_names),
                                     diagnostics);
  }
  if (!SupportsPointerReturnTypeDeclarator(method)) {
    for (const auto &token : method.return_pointer_declarator_tokens) {
      diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                     "type mismatch: unsupported method return type declarator '" + token.text +
                                         "' for selector '" + selector + "' in " + owner_kind + " '" + owner_name +
                                         "'"));
    }
  }
  if (!SupportsNullabilityReturnTypeSuffix(method)) {
    for (const auto &token : method.return_nullability_suffix_tokens) {
      diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                     "type mismatch: unsupported method return type suffix '" + token.text +
                                         "' for selector '" + selector + "' in " + owner_kind + " '" + owner_name +
                                         "'"));
    }
  }
  if (!SupportsOwnershipQualifierReturnTypeSuffix(method)) {
    for (const auto &token : method.return_ownership_qualifier_tokens) {
      diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                     "type mismatch: unsupported method return ownership qualifier '" + token.text +
                                         "' for selector '" + selector + "' in " + owner_kind + " '" + owner_name +
                                         "'"));
    }
  }
}
