static void ValidateProtocolCompositionSuffix(const std::string &suffix_text,
                                              unsigned line,
                                              unsigned column,
                                              const std::string &context,
                                              bool allow_generic_argument_suffix,
                                              std::vector<std::string> &diagnostics) {
  const ProtocolCompositionParseResult parsed = ParseProtocolCompositionSuffixText(suffix_text);
  if (allow_generic_argument_suffix && !parsed.IsValid() &&
      !ParseGenericArgumentSuffixTextSourceOrder(suffix_text).empty()) {
    return;
  }
  const std::string printable_suffix = suffix_text.empty() ? "<...>" : suffix_text;
  if (parsed.malformed_composition) {
    diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                   "type mismatch: malformed protocol composition suffix '" + printable_suffix +
                                       "' for " + context));
    return;
  }

  if (parsed.empty_composition) {
    diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                   "type mismatch: empty protocol composition suffix '" + printable_suffix +
                                       "' for " + context));
  }

  for (const auto &identifier : parsed.invalid_identifiers) {
    diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                   "type mismatch: invalid protocol identifier '" + identifier +
                                       "' in protocol composition suffix '" + printable_suffix + "' for " + context));
  }
  for (const auto &identifier : parsed.duplicate_identifiers) {
    diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                   "type mismatch: duplicate protocol identifier '" + identifier +
                                       "' in protocol composition suffix '" + printable_suffix + "' for " + context));
  }
}

static bool IsGenericInterfaceSpecializationSuffix(
    bool object_pointer_type_spelling, const std::string &object_pointer_type_name,
    const std::unordered_set<std::string> *generic_interface_names) {
  if (!object_pointer_type_spelling) {
    return false;
  }
  if (IsObjc3GenericCollectionKind(
          Objc3GenericCollectionKindFromSpelling(object_pointer_type_name))) {
    return true;
  }
  return generic_interface_names != nullptr &&
         generic_interface_names->find(object_pointer_type_name) !=
             generic_interface_names->end();
}

static bool ValidateGenericCollectionSpecializationSuffix(
    bool object_pointer_type_spelling,
    const std::string &object_pointer_type_name,
    const std::string &generic_suffix_text,
    unsigned line,
    unsigned column,
    const std::string &context,
    std::vector<std::string> &diagnostics) {
  if (!object_pointer_type_spelling) {
    return false;
  }
  if (!IsObjc3GenericCollectionKind(
          Objc3GenericCollectionKindFromSpelling(object_pointer_type_name))) {
    return false;
  }

  const Objc3GenericCollectionTypeModel model =
      BuildObjc3GenericCollectionTypeModel(
          object_pointer_type_name,
          ParseGenericArgumentSuffixTextSourceOrder(generic_suffix_text));
  if (!IsReadyObjc3GenericCollectionTypeModel(model)) {
    const std::string printable_suffix =
        generic_suffix_text.empty() ? "<...>" : generic_suffix_text;
    const std::string reason = model.diagnostic_reason.empty()
                                   ? "invalid generic collection descriptor"
                                   : model.diagnostic_reason;
    diagnostics.push_back(MakeDiag(
        line, column, "O3S206",
        "type mismatch: generic collection type suffix '" +
            printable_suffix + "' for " + context + " is invalid: " +
            reason));
  }
  return true;
}

static void ValidateParameterTypeSuffixes(
    const FunctionDecl &fn, std::vector<std::string> &diagnostics,
    const std::unordered_set<std::string> *generic_interface_names = nullptr) {
  for (const auto &param : fn.params) {
    if (param.has_generic_suffix && !SupportsGenericParamTypeSuffix(param)) {
      std::string suffix = param.generic_suffix_text;
      if (suffix.empty()) {
        suffix = "<...>";
      }
      diagnostics.push_back(MakeDiag(param.generic_line, param.generic_column, "O3S206",
                                     "type mismatch: generic parameter type suffix '" + suffix +
                                         "' is unsupported for non-id/Class/instancetype parameter annotation '" +
                                         param.name + "'"));
    } else if (param.has_generic_suffix && !param.value_optional.present &&
               !ValidateGenericCollectionSpecializationSuffix(
                   param.object_pointer_type_spelling,
                   param.object_pointer_type_name,
                   param.generic_suffix_text, param.generic_line,
                   param.generic_column,
                   "parameter '" + param.name + "' in function '" + fn.name +
                       "'",
                   diagnostics)) {
      ValidateProtocolCompositionSuffix(
          param.generic_suffix_text,
          param.generic_line,
          param.generic_column,
          "parameter '" + param.name + "' in function '" + fn.name + "'",
          IsGenericInterfaceSpecializationSuffix(
              param.object_pointer_type_spelling,
              param.object_pointer_type_name, generic_interface_names),
          diagnostics);
    }
    if (!SupportsPointerParamTypeDeclarator(param)) {
      for (const auto &token : param.pointer_declarator_tokens) {
        diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                       "type mismatch: pointer parameter type declarator '" + token.text +
                                           "' is unsupported for non-id/Class/instancetype parameter annotation '" +
                                           param.name + "'"));
      }
    }
    if (!SupportsNullabilityParamTypeSuffix(param)) {
      for (const auto &token : param.nullability_suffix_tokens) {
        diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                       "type mismatch: nullability parameter type suffix '" + token.text +
                                           "' is unsupported for non-id/Class/instancetype parameter annotation '" +
                                           param.name + "'"));
      }
    }
    if (!SupportsOwnershipQualifierParamTypeSuffix(param)) {
      for (const auto &token : param.ownership_qualifier_tokens) {
        diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                       "type mismatch: ownership parameter type qualifier '" + token.text +
                                           "' is unsupported for non-object parameter annotation '" + param.name +
                                           "'"));
      }
    }
  }
}

static void ValidateReturnTypeSuffixes(
    const FunctionDecl &fn, std::vector<std::string> &diagnostics,
    const std::unordered_set<std::string> *generic_interface_names = nullptr) {
  ValidateGenericCallableParameterList(
      fn.generic_params, "function '" + fn.name + "'",
      fn.generic_callable_reification_marker_declared,
      fn.generic_callable_reification_marker_line,
      fn.generic_callable_reification_marker_column, diagnostics);
  if (fn.has_return_generic_suffix && !SupportsGenericReturnTypeSuffix(fn)) {
    std::string suffix = fn.return_generic_suffix_text;
    if (suffix.empty()) {
      suffix = "<...>";
    }
    diagnostics.push_back(MakeDiag(fn.return_generic_line, fn.return_generic_column, "O3S206",
                                   "type mismatch: unsupported function return type suffix '" + suffix +
                                       "' for non-id/Class/instancetype return annotation in function '" + fn.name +
                                       "'"));
  } else if (fn.has_return_generic_suffix && !fn.return_value_optional.present &&
             !ValidateGenericCollectionSpecializationSuffix(
                 fn.return_object_pointer_type_spelling,
                 fn.return_object_pointer_type_name,
                 fn.return_generic_suffix_text, fn.return_generic_line,
                 fn.return_generic_column,
                 "return annotation in function '" + fn.name + "'",
                 diagnostics)) {
    ValidateProtocolCompositionSuffix(
        fn.return_generic_suffix_text,
        fn.return_generic_line,
        fn.return_generic_column,
        "return annotation in function '" + fn.name + "'",
        IsGenericInterfaceSpecializationSuffix(
            fn.return_object_pointer_type_spelling,
            fn.return_object_pointer_type_name, generic_interface_names),
        diagnostics);
  }
  if (!SupportsPointerReturnTypeDeclarator(fn)) {
    for (const auto &token : fn.return_pointer_declarator_tokens) {
      diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                     "type mismatch: unsupported function return type declarator '" + token.text +
                                         "' for non-id/Class/instancetype return annotation in function '" + fn.name +
                                         "'"));
    }
  }
  if (!SupportsNullabilityReturnTypeSuffix(fn)) {
    for (const auto &token : fn.return_nullability_suffix_tokens) {
      diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                     "type mismatch: unsupported function return type suffix '" + token.text +
                                         "' for non-id/Class/instancetype return annotation in function '" + fn.name +
                                         "'"));
    }
  }
  if (!SupportsOwnershipQualifierReturnTypeSuffix(fn)) {
    for (const auto &token : fn.return_ownership_qualifier_tokens) {
      diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                     "type mismatch: unsupported function return ownership qualifier '" + token.text +
                                         "' for non-object return annotation in function '" + fn.name + "'"));
    }
  }
}
