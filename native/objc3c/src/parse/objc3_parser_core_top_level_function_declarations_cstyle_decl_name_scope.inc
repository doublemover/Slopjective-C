  bool ParseTopLevelCStyleFunctionNameAndScope(FunctionDecl &fn) {
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P101",
                   "expected function identifier, found " + DescribeParserDiagnosticToken(token)));
      return false;
    }

    const Token name_token = Advance();
    fn.name = name_token.text;
    fn.line = name_token.line;
    fn.column = name_token.column;
    fn.scope_owner_symbol = "global";
    fn.scope_path_lexicographic =
        BuildScopePathLexicographic(fn.scope_owner_symbol, "function:" + fn.name);
    return true;
  }
