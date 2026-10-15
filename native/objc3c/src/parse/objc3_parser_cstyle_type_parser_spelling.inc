namespace {

bool ParseObjc3CStyleGenericSuffix(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    Objc3CStyleFunctionTypeParseResult &result,
    std::vector<std::string> &diagnostics) {
  if (!MatchTokenKind(tokens, index, Objc3LexTokenKind::Less)) {
    return true;
  }
  const Objc3LexToken &open = tokens[index - 1u];
  result.has_generic_suffix = true;
  result.generic_suffix_terminated = false;
  result.generic_suffix_text = "<";
  result.generic_line = open.line;
  result.generic_column = open.column;

  int depth = 1;
  while (depth > 0 && !AtTokenKind(tokens, index, Objc3LexTokenKind::Eof)) {
    if (MatchTokenKind(tokens, index, Objc3LexTokenKind::Less)) {
      result.generic_suffix_text += "<";
      ++depth;
      continue;
    }
    if (MatchTokenKind(tokens, index, Objc3LexTokenKind::Greater)) {
      result.generic_suffix_text += ">";
      --depth;
      if (depth == 0) {
        result.generic_suffix_terminated = true;
      }
      continue;
    }
    if (MatchTokenKind(tokens, index, Objc3LexTokenKind::GreaterGreater)) {
      result.generic_suffix_text += ">>";
      depth -= 2;
      if (depth == 0) {
        result.generic_suffix_terminated = true;
      }
      continue;
    }
    result.generic_suffix_text += AdvanceToken(tokens, index).text;
  }

  if (!result.generic_suffix_terminated) {
    diagnostics.push_back(MakeDiag(
        open.line, open.column, "O3P101",
        "unterminated generic suffix in C-style type spelling"));
    return false;
  }
  return true;
}

bool ParseObjc3CStyleTypeSpelling(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    Objc3CStyleFunctionTypeParseResult &result,
    std::vector<std::string> &diagnostics) {
  Objc3CStyleTypeTokenClassification classification;
  if (ClassifyObjc3CStyleBuiltinTypeToken(PeekToken(tokens, index), classification)) {
    result.type = classification.type;
    result.id_spelling = classification.id_spelling;
    result.class_spelling = classification.class_spelling;
    result.sel_spelling = classification.sel_spelling;
    result.instancetype_spelling = classification.instancetype_spelling;
    AdvanceToken(tokens, index);
    return true;
  }

  if (!AtTokenKind(tokens, index, Objc3LexTokenKind::Identifier)) {
    return false;
  }

  const Objc3LexToken object_name = AdvanceToken(tokens, index);
  if (object_name.text == "Optional") {
    result.type = ValueType::Optional;
    result.value_optional.present = true;
    result.value_optional.source_type_admitted = true;
    result.value_optional.semantic_carrier_modeled = true;
    result.value_optional.line = object_name.line;
    result.value_optional.column = object_name.column;
    return ParseObjc3CStyleGenericSuffix(tokens, index, result, diagnostics);
  }
  result.type = ValueType::ObjCObjectPtr;
  result.object_pointer_spelling = true;
  result.object_pointer_name = object_name.text;
  return ParseObjc3CStyleGenericSuffix(tokens, index, result, diagnostics);
}

}  // namespace
