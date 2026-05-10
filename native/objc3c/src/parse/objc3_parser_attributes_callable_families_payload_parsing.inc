  template <typename TCallableDecl>
  bool ParseRetainableCFamilyCallablePayload(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P327",
          "missing '(' after retainable C-family callable attribute"));
      return false;
    }
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P328",
          "retainable C-family callable attribute requires family identifier"));
      return false;
    }
    decl.retainable_c_family_callable_attributes.push_back(
        attribute_name.text);
    decl.retainable_c_family_names.push_back(Advance().text);
    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P329",
          "missing ')' after retainable C-family attribute payload"));
      return false;
    }
    PublishRetainableCFamilyCallableProfile(decl);
    return true;
  }
