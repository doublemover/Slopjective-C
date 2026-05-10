void SynchronizeObjc3ParserTopLevel(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index) {
  if (!Objc3ParserRecoveryBoundaryIsHardCutoverOwned() ||
      Objc3ParserRecoveryBoundaryCountsAsSuccess()) {
    return;
  }
  while (!At(tokens, index, TokenKind::Eof)) {
    if (Match(tokens, index, TokenKind::Semicolon)) {
      return;
    }
    if (IsObjc3TopLevelRecoveryBoundaryToken(PeekToken(tokens, index).kind)) {
      return;
    }
    Advance(tokens, index);
  }
}
