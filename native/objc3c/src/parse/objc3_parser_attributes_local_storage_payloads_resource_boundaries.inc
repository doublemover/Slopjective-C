  bool ParseLocalResourceAttributePayloadOpen() {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      return RejectLocalStorageAttributeAt(
          token, "O3P304", "missing '(' after objc_resource");
    }
    return true;
  }

  bool ParseLocalResourceAttributePayloadClose() {
    if (!Match(TokenKind::RParen) ||
        !Match(TokenKind::RParen) ||
        !Match(TokenKind::RParen)) {
      const Token &token = Peek();
      return RejectLocalStorageAttributeAt(
          token, "O3P311",
          "missing ')))' after objc_resource local annotation");
    }
    return true;
  }
