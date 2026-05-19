  bool ParseFunctionNameAndScope(FunctionDecl &fn) {
    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(MakeDiag(
          name_token.line,
          name_token.column,
          "O3P101",
          "invalid function identifier"));
      SynchronizeTopLevel();
      return false;
    }

    fn.name = Previous().text;
    fn.line = Previous().line;
    fn.column = Previous().column;
    fn.scope_owner_symbol = "global";
    fn.scope_path_lexicographic = BuildScopePathLexicographic(
        fn.scope_owner_symbol, "function:" + fn.name);

    return RejectUnexpectedFunctionDeclarationQualifier("after function name");
  }
