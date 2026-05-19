  bool ParseLocalStorageAttribute(LetStmt &stmt) {
    if (!Match(TokenKind::LParen) || !Match(TokenKind::LParen)) {
      const Token &token = Peek();
      return RejectLocalStorageAttributeAt(
          token, "O3P314", "malformed __attribute__ local annotation");
    }
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      return RejectLocalStorageAttributeAt(
          token, "O3P315",
          "missing local annotation name after __attribute__((");
    }
    const Token attribute_name = Advance();
    if (attribute_name.text == "cleanup") {
      return ParseLocalCleanupAttribute(stmt, attribute_name);
    }
    if (attribute_name.text == "objc_resource") {
      return ParseLocalResourceAttribute(stmt, attribute_name);
    }
    return RejectLocalStorageAttributeAt(
        attribute_name, "O3P316",
        "only cleanup and objc_resource are supported on local let bindings in this tranche");
  }
