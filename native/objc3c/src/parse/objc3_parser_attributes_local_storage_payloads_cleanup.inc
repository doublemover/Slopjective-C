  bool ParseLocalCleanupAttribute(LetStmt &stmt, const Token &) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      return RejectLocalStorageAttributeAt(
          token, "O3P301", "missing '(' after cleanup");
    }
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      return RejectLocalStorageAttributeAt(
          token, "O3P302", "cleanup requires cleanup function identifier");
    }
    stmt.cleanup_function_symbol = Advance().text;
    if (!Match(TokenKind::RParen) ||
        !Match(TokenKind::RParen) ||
        !Match(TokenKind::RParen)) {
      const Token &token = Peek();
      return RejectLocalStorageAttributeAt(
          token, "O3P303", "missing ')))' after cleanup local annotation");
    }
    stmt.cleanup_attribute_declared = true;
    stmt.cleanup_profile_is_normalized = true;
    stmt.cleanup_profile =
        BuildCleanupAttributeProfile(true, stmt.cleanup_function_symbol);
    return true;
  }
