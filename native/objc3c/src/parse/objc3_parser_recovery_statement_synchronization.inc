void SynchronizeObjc3ParserStatement(
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
    if (IsObjc3StatementRecoveryBoundaryToken(PeekToken(tokens, index).kind) ||
        IsObjc3IdentifierAssignmentStatementLead(tokens, index) ||
        IsObjc3IdentifierUpdateStatementLead(tokens, index) ||
        IsObjc3PrefixUpdateStatementLead(tokens, index)) {
      return;
    }
    Advance(tokens, index);
  }
}
