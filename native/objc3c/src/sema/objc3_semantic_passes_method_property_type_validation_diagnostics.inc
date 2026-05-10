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
