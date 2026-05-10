void SynchronizeObjc3ParserFunctionTail(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index) {
  if (!Objc3ParserRecoveryBoundaryIsHardCutoverOwned() ||
      Objc3ParserRecoveryBoundaryCountsAsSuccess()) {
    return;
  }
  if (At(tokens, index, TokenKind::LBrace)) {
    int depth = 0;
    while (!At(tokens, index, TokenKind::Eof)) {
      if (Match(tokens, index, TokenKind::LBrace)) {
        ++depth;
        continue;
      }
      if (Match(tokens, index, TokenKind::RBrace)) {
        --depth;
        if (depth <= 0) {
          return;
        }
        continue;
      }
      Advance(tokens, index);
    }
    return;
  }
  SynchronizeObjc3ParserTopLevel(tokens, index);
}
