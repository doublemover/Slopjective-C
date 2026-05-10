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

  bool ParseFunctionReturnAnnotation(
      FunctionDecl &fn,
      bool &has_return_annotation) {
    if (!Match(TokenKind::Minus)) {
      return true;
    }

    const Token arrow_start = Previous();
    if (!Match(TokenKind::Greater)) {
      diagnostics_.push_back(MakeDiag(
          arrow_start.line,
          arrow_start.column,
          "O3P114",
          "missing '>' in function return annotation"));
      SynchronizeFunctionTail();
      return false;
    }

    if (!ParseFunctionReturnType(fn)) {
      SynchronizeFunctionTail();
      return false;
    }
    has_return_annotation = true;
    return true;
  }

  bool ParseFunctionCallableTail(
      FunctionDecl &fn,
      bool &has_return_annotation) {
    if (!ParseOptionalAsyncClause(fn)) {
      SynchronizeTopLevel();
      return false;
    }

    if (!ParseOptionalThrowsClause(fn)) {
      SynchronizeTopLevel();
      return false;
    }

    if (!ParseFunctionReturnAnnotation(fn, has_return_annotation)) {
      return false;
    }

    if (!ParseOptionalThrowsClause(fn)) {
      SynchronizeTopLevel();
      return false;
    }

    if (!ParseOptionalAsyncClause(fn)) {
      SynchronizeTopLevel();
      return false;
    }

    if (!ParseOptionalCallableBridgeAttributes(fn)) {
      SynchronizeTopLevel();
      return false;
    }

    fn.returns_borrowed_profile = BuildReturnsBorrowedProfile(
        fn.objc_returns_borrowed_declared,
        fn.objc_returns_borrowed_owner_index,
        fn.return_borrowed_pointer_qualified);

    return RejectUnexpectedFunctionDeclarationQualifier(
        "after function return annotation");
  }
