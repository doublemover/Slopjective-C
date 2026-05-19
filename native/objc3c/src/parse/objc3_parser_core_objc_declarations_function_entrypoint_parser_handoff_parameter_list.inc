  bool ParseFunctionParameterList(FunctionDecl &fn) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line,
          token.column,
          "O3P106",
          "missing '(' after function name"));
      SynchronizeTopLevel();
      return false;
    }

    if (!ParseFunctionParameters(fn)) {
      SynchronizeTopLevel();
      return false;
    }

    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line,
          token.column,
          "O3P109",
          "missing ')' after parameters"));
      SynchronizeTopLevel();
      return false;
    }

    return RejectUnexpectedFunctionDeclarationQualifier("after parameter list");
  }
