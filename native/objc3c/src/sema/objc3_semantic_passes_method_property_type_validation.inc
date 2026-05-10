struct MethodSelectorNormalizationContractInfo {
  std::string normalized_selector;
  std::size_t selector_piece_count = 0;
  std::size_t selector_parameter_piece_count = 0;
  bool selector_contract_normalized = false;
  bool selector_had_pieceless_form = false;
  bool selector_has_spelling_mismatch = false;
  bool selector_has_arity_mismatch = false;
  bool selector_has_parameter_linkage_mismatch = false;
  bool selector_has_normalization_flag_mismatch = false;
  bool selector_has_missing_piece_keyword = false;
};

static std::string BuildNormalizedMethodSelectorFromPieces(
    const std::vector<Objc3MethodDecl::SelectorPiece> &pieces) {
  std::string normalized;
  for (const auto &piece : pieces) {
    normalized += piece.keyword;
    if (piece.has_parameter) {
      normalized += ":";
    }
  }
  return normalized;
}

static MethodSelectorNormalizationContractInfo BuildMethodSelectorNormalizationContractInfo(
    const Objc3MethodDecl &method) {
  MethodSelectorNormalizationContractInfo info;
  info.selector_piece_count = method.selector_pieces.size();
  info.selector_had_pieceless_form = method.selector_pieces.empty();

  std::size_t linked_param_index = 0;
  for (const auto &piece : method.selector_pieces) {
    if (piece.keyword.empty()) {
      info.selector_has_missing_piece_keyword = true;
    }
    if (!piece.has_parameter) {
      continue;
    }

    ++info.selector_parameter_piece_count;
    if (linked_param_index >= method.params.size() || piece.parameter_name != method.params[linked_param_index].name) {
      info.selector_has_parameter_linkage_mismatch = true;
    }
    ++linked_param_index;
  }
  info.selector_has_arity_mismatch = info.selector_parameter_piece_count != method.params.size();

  if (method.selector_pieces.empty()) {
    info.normalized_selector = method.selector;
  } else {
    info.normalized_selector = BuildNormalizedMethodSelectorFromPieces(method.selector_pieces);
    info.selector_has_spelling_mismatch = method.selector != info.normalized_selector;
  }

  if (info.normalized_selector.empty()) {
    info.normalized_selector = "<unknown>";
  }

  info.selector_has_normalization_flag_mismatch = !method.selector_is_normalized;
  info.selector_contract_normalized = !info.selector_had_pieceless_form &&
                                      !info.selector_has_spelling_mismatch &&
                                      !info.selector_has_arity_mismatch &&
                                      !info.selector_has_parameter_linkage_mismatch &&
                                      !info.selector_has_normalization_flag_mismatch &&
                                      !info.selector_has_missing_piece_keyword &&
                                      info.normalized_selector != "<unknown>";
  return info;
}

static std::string MethodSelectorName(const Objc3MethodDecl &method) {
  const MethodSelectorNormalizationContractInfo selector_contract =
      BuildMethodSelectorNormalizationContractInfo(method);
  return selector_contract.normalized_selector;
}

static void ValidateMethodSelectorNormalizationContract(
    const Objc3MethodDecl &method,
    const std::string &owner_name,
    const std::string &owner_kind,
    const MethodSelectorNormalizationContractInfo &selector_contract,
    std::vector<std::string> &diagnostics) {
  const std::string selector = selector_contract.normalized_selector.empty()
                                   ? std::string("<unknown>")
                                   : selector_contract.normalized_selector;
  if (selector_contract.selector_had_pieceless_form) {
    diagnostics.push_back(MakeDiag(method.line,
                                   method.column,
                                   "O3S206",
                                   "type mismatch: selector normalization requires selector pieces for selector '" +
                                       selector + "' in " + owner_kind + " '" + owner_name + "'"));
  }
  if (selector_contract.selector_has_spelling_mismatch) {
    const std::string raw_selector = method.selector.empty() ? "<unknown>" : method.selector;
    diagnostics.push_back(MakeDiag(method.line,
                                   method.column,
                                   "O3S206",
                                   "type mismatch: selector normalization mismatch in " + owner_kind + " '" +
                                       owner_name + "' for selector '" + raw_selector + "' (expected '" + selector +
                                       "')"));
  }
  if (selector_contract.selector_has_normalization_flag_mismatch) {
    diagnostics.push_back(MakeDiag(method.line,
                                   method.column,
                                   "O3S206",
                                   "type mismatch: selector normalization flag mismatch for selector '" + selector +
                                       "' in " + owner_kind + " '" + owner_name + "'"));
  }
  if (selector_contract.selector_has_missing_piece_keyword) {
    for (const auto &piece : method.selector_pieces) {
      if (!piece.keyword.empty()) {
        continue;
      }
      diagnostics.push_back(MakeDiag(piece.line,
                                     piece.column,
                                     "O3S206",
                                     "type mismatch: selector piece keyword must be non-empty for selector '" +
                                         selector + "' in " + owner_kind + " '" + owner_name + "'"));
    }
  }
  if (selector_contract.selector_has_arity_mismatch) {
    diagnostics.push_back(MakeDiag(method.line,
                                   method.column,
                                   "O3S206",
                                   "type mismatch: selector arity mismatch for selector '" + selector + "' in " +
                                       owner_kind + " '" + owner_name + "' (selector parameters=" +
                                       std::to_string(selector_contract.selector_parameter_piece_count) +
                                       ", declaration parameters=" + std::to_string(method.params.size()) + ")"));
  }
  if (selector_contract.selector_has_parameter_linkage_mismatch) {
    std::size_t linked_param_index = 0;
    for (const auto &piece : method.selector_pieces) {
      if (!piece.has_parameter) {
        continue;
      }
      const bool missing_decl_param = linked_param_index >= method.params.size();
      const std::string expected_param = piece.parameter_name.empty() ? "<unnamed>" : piece.parameter_name;
      const std::string actual_param =
          missing_decl_param
              ? "<missing>"
              : (method.params[linked_param_index].name.empty() ? std::string("<unnamed>")
                                                                : method.params[linked_param_index].name);
      if (missing_decl_param || expected_param != actual_param) {
        diagnostics.push_back(MakeDiag(piece.line,
                                       piece.column,
                                       "O3S206",
                                       "type mismatch: selector parameter linkage mismatch for selector '" +
                                           selector + "' in " + owner_kind + " '" + owner_name + "' piece '" +
                                           piece.keyword + ":' (piece parameter='" + expected_param +
                                           "', declaration parameter='" + actual_param + "')"));
      }
      ++linked_param_index;
    }

    while (linked_param_index < method.params.size()) {
      const FuncParam &param = method.params[linked_param_index];
      const std::string param_name = param.name.empty() ? "<unnamed>" : param.name;
      diagnostics.push_back(MakeDiag(param.line,
                                     param.column,
                                     "O3S206",
                                     "type mismatch: selector parameter linkage mismatch for selector '" + selector +
                                         "' in " + owner_kind + " '" + owner_name +
                                         "' (declaration parameter '" + param_name +
                                         "' has no selector piece linkage)"));
      ++linked_param_index;
    }
  }
}

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
    } else if (param.has_generic_suffix) {
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
  if (method.has_return_generic_suffix && !SupportsGenericReturnTypeSuffix(method)) {
    std::string suffix = method.return_generic_suffix_text;
    if (suffix.empty()) {
      suffix = "<...>";
    }
    diagnostics.push_back(MakeDiag(method.return_generic_line, method.return_generic_column, "O3S206",
                                   "type mismatch: unsupported method return type suffix '" + suffix +
                                       "' for selector '" + selector + "' in " + owner_kind + " '" + owner_name +
                                       "'"));
  } else if (method.has_return_generic_suffix) {
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

static void ValidatePropertyTypeSuffixes(const Objc3PropertyDecl &property,
                                         const std::string &owner_name,
                                         const std::string &owner_kind,
                                         std::vector<std::string> &diagnostics,
                                         const std::unordered_set<std::string> *generic_interface_names = nullptr) {
  if (property.has_generic_suffix && !SupportsGenericPropertyTypeSuffix(property)) {
    std::string suffix = property.generic_suffix_text;
    if (suffix.empty()) {
      suffix = "<...>";
    }
    diagnostics.push_back(MakeDiag(property.generic_line,
                                   property.generic_column,
                                   "O3S206",
                                   "type mismatch: generic property type suffix '" + suffix +
                                       "' is unsupported for property '" + property.name + "' in " + owner_kind +
                                       " '" + owner_name + "'"));
  } else if (property.has_generic_suffix) {
    ValidateProtocolCompositionSuffix(property.generic_suffix_text,
                                     property.generic_line,
                                     property.generic_column,
                                     "property '" + property.name + "' in " + owner_kind + " '" + owner_name +
                                         "' type annotation",
                                     IsGenericInterfaceSpecializationSuffix(
                                         property.object_pointer_type_spelling,
                                         property.object_pointer_type_name,
                                         generic_interface_names),
                                     diagnostics);
  }
  if (!SupportsPointerPropertyTypeDeclarator(property)) {
    for (const auto &token : property.pointer_declarator_tokens) {
      diagnostics.push_back(MakeDiag(token.line,
                                     token.column,
                                     "O3S206",
                                     "type mismatch: unsupported property type declarator '" + token.text +
                                         "' for property '" + property.name + "' in " + owner_kind + " '" +
                                         owner_name + "'"));
    }
  }
  if (!SupportsNullabilityPropertyTypeSuffix(property)) {
    for (const auto &token : property.nullability_suffix_tokens) {
      diagnostics.push_back(MakeDiag(token.line,
                                     token.column,
                                     "O3S206",
                                     "type mismatch: unsupported property type suffix '" + token.text +
                                         "' for property '" + property.name + "' in " + owner_kind + " '" +
                                         owner_name + "'"));
    }
  }
  if (!SupportsOwnershipQualifierPropertyTypeSuffix(property)) {
    for (const auto &token : property.ownership_qualifier_tokens) {
      diagnostics.push_back(MakeDiag(token.line,
                                     token.column,
                                     "O3S206",
                                     "type mismatch: unsupported property ownership qualifier '" + token.text +
                                         "' for property '" + property.name + "' in " + owner_kind + " '" +
                                         owner_name + "'"));
    }
  }
}
