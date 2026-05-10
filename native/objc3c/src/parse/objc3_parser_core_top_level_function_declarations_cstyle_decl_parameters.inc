  bool ParseTopLevelCStyleFunctionParameterList(FunctionDecl &fn) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P106",
                   "missing '(' after function name; found " + DescribeParserDiagnosticToken(token)));
      return false;
    }

    if (!ParseCStyleFunctionParameters(fn)) {
      return false;
    }

    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P109",
                   "missing ')' after parameters; found " + DescribeParserDiagnosticToken(token)));
      return false;
    }

    return true;
  }
