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
