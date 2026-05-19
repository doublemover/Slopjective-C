bool IsObjc3CStyleTypeLeadToken(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index) {
  if (index >= tokens.size()) {
    return false;
  }
  Objc3CStyleTypeTokenClassification classification;
  return ClassifyObjc3CStyleBuiltinTypeToken(tokens[index], classification);
}

bool IsObjc3CStyleFunctionDeclarationStart(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index) {
  if (!IsObjc3CStyleTypeLeadToken(tokens, index)) {
    return false;
  }
  std::size_t cursor = index + 1u;
  while (cursor < tokens.size() &&
         tokens[cursor].kind == Objc3LexTokenKind::Star) {
    ++cursor;
  }
  if (cursor >= tokens.size() ||
      tokens[cursor].kind != Objc3LexTokenKind::Identifier) {
    return false;
  }
  ++cursor;
  return cursor < tokens.size() &&
         tokens[cursor].kind == Objc3LexTokenKind::LParen;
}
