  template <typename TCallableDecl>
  bool ParseReturnsBorrowedAttributePayload(TCallableDecl &decl) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P296",
          "missing '(' after objc_returns_borrowed attribute"));
      return false;
    }
    if (!At(TokenKind::Identifier) || Peek().text != "owner_index") {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P297",
          "objc_returns_borrowed requires owner_index clause"));
      return false;
    }
    Advance();
    if (!Match(TokenKind::Equal)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P298",
          "missing '=' after owner_index in objc_returns_borrowed"));
      return false;
    }
    if (!At(TokenKind::Number)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P299",
          "objc_returns_borrowed owner_index requires integer literal"));
      return false;
    }
    decl.objc_returns_borrowed_owner_index =
        static_cast<std::size_t>(std::max(0, std::stoi(Advance().text)));
    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P300",
          "missing ')' after objc_returns_borrowed payload"));
      return false;
    }
    decl.objc_returns_borrowed_declared = true;
    return true;
  }
