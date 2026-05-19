  std::unique_ptr<Expr> ParseKeyPathLiteralExpression(const Token &keypath_token) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after @keypath"));
      return nullptr;
    }
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "expected key-path root identifier"));
      return nullptr;
    }
    const Token root = Advance();
    if (!Match(TokenKind::Comma)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "missing ',' after key-path root"));
      return nullptr;
    }
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "expected key-path component"));
      return nullptr;
    }
    std::vector<std::string> components;
    components.push_back(Advance().text);
    while (Match(TokenKind::Dot)) {
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "expected identifier after '.' in key path"));
        return nullptr;
      }
      components.push_back(Advance().text);
    }
    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after @keypath"));
      return nullptr;
    }
    return BuildObjc3TypedKeyPathLiteralExpr(
        keypath_token, root, std::move(components));
  }
