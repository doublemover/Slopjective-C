  bool AtThrowsClauseKeyword() const {
    return At(TokenKind::Identifier) && Peek().text == "throws";
  }

  bool AtAsyncClauseKeyword() const {
    return At(TokenKind::KwAsync);
  }
