  bool At(TokenKind kind) const { return AtTokenKind(tokens_, index_, kind); }

  const Token &Peek() const { return PeekToken(tokens_, index_); }

  const Token &Previous() const { return PreviousToken(tokens_, index_); }

  const Token &Advance() { return AdvanceToken(tokens_, index_); }

  bool Match(TokenKind kind) { return MatchTokenKind(tokens_, index_, kind); }

  bool AtCStyleTypeLeadToken() const {
    return IsObjc3CStyleTypeLeadToken(tokens_, index_);
  }

  bool AtCStyleTopLevelFunctionDeclStart() const {
    return IsObjc3CStyleFunctionDeclarationStart(tokens_, index_);
  }
