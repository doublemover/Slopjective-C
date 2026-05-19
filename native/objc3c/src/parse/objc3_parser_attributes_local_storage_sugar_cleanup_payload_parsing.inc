  bool ParseCleanupLocalStorageSugarPayload(LetStmt &stmt) {
    if (!Match(TokenKind::LParen)) {
      return RejectMissingCleanupSugarOpenParen();
    }
    if (!At(TokenKind::Identifier)) {
      return RejectMissingCleanupSugarFunctionIdentifier();
    }
    stmt.cleanup_function_symbol = Advance().text;
    if (!Match(TokenKind::RParen)) {
      return RejectMissingCleanupSugarCloseParen();
    }
    PublishCleanupLocalStorageSugarProfile(stmt);
    return true;
  }
