  std::unique_ptr<FunctionDecl> ParseFunction() {
    auto fn = std::make_unique<FunctionDecl>();
    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) ||
        At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' after 'fn'"
          : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' after 'fn'"
                                                  : "unexpected qualifier 'async' after 'fn'";
      diagnostics_.push_back(MakeDiag(
          qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(MakeDiag(
          name_token.line, name_token.column, "O3P101",
          "invalid function identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    fn->name = Previous().text;
    fn->line = Previous().line;
    fn->column = Previous().column;
    fn->scope_owner_symbol = "global";
    fn->scope_path_lexicographic = BuildScopePathLexicographic(
        fn->scope_owner_symbol, "function:" + fn->name);

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) ||
        At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' after function name"
          : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' after function name"
                                                  : "unexpected qualifier 'async' after function name";
      diagnostics_.push_back(MakeDiag(
          qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P106",
          "missing '(' after function name"));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!ParseFunctionParameters(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P109",
          "missing ')' after parameters"));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) ||
        At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' after parameter list"
          : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' after parameter list"
                                                  : "unexpected qualifier 'async' after parameter list";
      diagnostics_.push_back(MakeDiag(
          qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    bool has_return_annotation = false;
    if (!ParseOptionalAsyncClause(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }
    if (!ParseOptionalThrowsClause(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (Match(TokenKind::Minus)) {
      const Token arrow_start = Previous();
      if (!Match(TokenKind::Greater)) {
        diagnostics_.push_back(MakeDiag(
            arrow_start.line, arrow_start.column, "O3P114",
            "missing '>' in function return annotation"));
        SynchronizeFunctionTail();
        return nullptr;
      }
      if (!ParseFunctionReturnType(*fn)) {
        SynchronizeFunctionTail();
        return nullptr;
      }
      has_return_annotation = true;
    }

    if (!ParseOptionalThrowsClause(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!ParseOptionalAsyncClause(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!ParseOptionalCallableBridgeAttributes(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }
    fn->returns_borrowed_profile = BuildReturnsBorrowedProfile(
        fn->objc_returns_borrowed_declared,
        fn->objc_returns_borrowed_owner_index,
        fn->return_borrowed_pointer_qualified);

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) ||
        At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' after function return annotation"
          : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' after function return annotation"
                                                  : "unexpected qualifier 'async' after function return annotation";
      diagnostics_.push_back(MakeDiag(
          qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (Match(TokenKind::Semicolon)) {
      fn->is_prototype = true;
      FinalizeFunctionSemanticProfiles(*fn, has_return_annotation);
      return fn;
    }

    if (!At(TokenKind::LBrace)) {
      const Token &token = Peek();
      if (At(TokenKind::KwModule) || At(TokenKind::KwLet) ||
          At(TokenKind::KwFn) || At(TokenKind::KwPure) ||
          At(TokenKind::KwExtern) || At(TokenKind::KwAsync) ||
          At(TokenKind::KwAtInterface) ||
          At(TokenKind::KwAtImplementation) ||
          At(TokenKind::KwAtProtocol) || At(TokenKind::KwAtProperty) ||
          At(TokenKind::Eof)) {
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P104",
            "missing ';' after function prototype declaration"));
      } else {
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P110",
            "missing '{' to start block"));
      }
      SynchronizeTopLevel();
      return nullptr;
    }

    fn->body = ParseBlock();
    if (block_failed_) {
      block_failed_ = false;
      SynchronizeTopLevel();
      return nullptr;
    }
    FinalizeFunctionSemanticProfiles(*fn, has_return_annotation);
    return fn;
  }
