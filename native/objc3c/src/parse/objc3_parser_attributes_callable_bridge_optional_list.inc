  template <typename TCallableDecl>
  bool ParseOptionalCallableBridgeAttributesImpl(TCallableDecl &decl) {
    while (AtIdentifierText("__attribute__")) {
      Advance();
      if (!Match(TokenKind::LParen) || !Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P285",
            "malformed __attribute__ callable annotation"));
        return false;
      }

      do {
        if (!ParseSingleCallableBridgeAttribute(decl)) {
          return false;
        }
      } while (Match(TokenKind::Comma));

      if (!Match(TokenKind::RParen) || !Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P286",
            "missing '))' after callable attribute list"));
        return false;
      }
    }
    return true;
  }
