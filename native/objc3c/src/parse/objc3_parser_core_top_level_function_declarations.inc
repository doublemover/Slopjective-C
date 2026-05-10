  void ParseTopLevelCStyleFunctionDecl(Objc3ParsedProgram &program) {
    auto fn = std::make_unique<FunctionDecl>();
    const Token &return_type_token = Peek();
    const Objc3CStyleFunctionTypeParseResult parsed_return_type =
        ParseObjc3CStyleFunctionType(tokens_, index_, true,
                                     return_type_token.line,
                                     return_type_token.column,
                                     "function return", diagnostics_);
    if (!parsed_return_type.ok) {
      SynchronizeTopLevel();
      return;
    }
    fn->return_type = parsed_return_type.type;
    fn->return_id_spelling = parsed_return_type.id_spelling;
    fn->return_class_spelling = parsed_return_type.class_spelling;
    fn->return_sel_spelling = parsed_return_type.sel_spelling;
    fn->return_instancetype_spelling = parsed_return_type.instancetype_spelling;
    fn->return_object_pointer_type_spelling =
        parsed_return_type.object_pointer_spelling;
    fn->return_object_pointer_type_name =
        parsed_return_type.object_pointer_name;
    fn->has_return_pointer_declarator = parsed_return_type.pointer_depth > 0u;
    fn->return_pointer_declarator_depth = parsed_return_type.pointer_depth;
    fn->return_typecheck_family_symbol = BuildObjcTypecheckReturnFamilySymbol(*fn);

    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P101",
                   "expected function identifier, found " + DescribeParserDiagnosticToken(token)));
      SynchronizeTopLevel();
      return;
    }
    const Token name_token = Advance();
    fn->name = name_token.text;
    fn->line = name_token.line;
    fn->column = name_token.column;
    fn->scope_owner_symbol = "global";
    fn->scope_path_lexicographic =
        BuildScopePathLexicographic(fn->scope_owner_symbol, "function:" + fn->name);

    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P106",
                   "missing '(' after function name; found " + DescribeParserDiagnosticToken(token)));
      SynchronizeTopLevel();
      return;
    }

    if (!ParseCStyleFunctionParameters(*fn)) {
      SynchronizeTopLevel();
      return;
    }

    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P109",
                   "missing ')' after parameters; found " + DescribeParserDiagnosticToken(token)));
      SynchronizeTopLevel();
      return;
    }

    if (Match(TokenKind::Semicolon)) {
      fn->is_prototype = true;
      FinalizeThrowsDeclarationProfile(*fn, true);
      FinalizeResultLikeProfile(*fn);
      FinalizeNSErrorBridgingProfile(*fn);
      FinalizeUnwindCleanupProfile(*fn);
      FinalizeErrorDiagnosticsRecoveryProfile(*fn);
      FinalizeAsyncContinuationProfile(*fn);
      FinalizeAwaitSuspensionProfile(*fn);
      FinalizeActorIsolationSendabilityProfile(*fn);
      FinalizeTaskRuntimeCancellationProfile(*fn);
      FinalizeConcurrencyReplayRaceGuardProfile(*fn);
      FinalizeUnsafePointerExtensionProfile(*fn);
      FinalizeInlineAsmIntrinsicGovernanceProfile(*fn);
      ast_builder_.AddFunctionDecl(program, std::move(*fn));
      return;
    }

    if (!At(TokenKind::LBrace)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110", "missing '{' to start block"));
      SynchronizeTopLevel();
      return;
    }

    fn->body = ParseBlock();
    if (block_failed_) {
      block_failed_ = false;
      SynchronizeTopLevel();
      return;
    }

    FinalizeThrowsDeclarationProfile(*fn, true);
    FinalizeResultLikeProfile(*fn);
    FinalizeNSErrorBridgingProfile(*fn);
    FinalizeUnwindCleanupProfile(*fn);
    FinalizeErrorDiagnosticsRecoveryProfile(*fn);
    FinalizeAsyncContinuationProfile(*fn);
    FinalizeAwaitSuspensionProfile(*fn);
    FinalizeActorIsolationSendabilityProfile(*fn);
    FinalizeTaskRuntimeCancellationProfile(*fn);
    FinalizeConcurrencyReplayRaceGuardProfile(*fn);
    FinalizeUnsafePointerExtensionProfile(*fn);
    FinalizeInlineAsmIntrinsicGovernanceProfile(*fn);
    ast_builder_.AddFunctionDecl(program, std::move(*fn));
  }

  void ParseTopLevelFunctionDecl(Objc3ParsedProgram &program) {
    bool is_pure = false;
    bool is_extern = false;
    bool is_async = false;
    std::optional<TokenKind> trailing_qualifier = std::nullopt;

    while (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
      if (Match(TokenKind::KwPure)) {
        if (is_pure) {
          const Token &token = Previous();
          diagnostics_.push_back(
              MakeDiag(token.line, token.column, "O3P100", "duplicate 'pure' qualifier in function declaration"));
          SynchronizeTopLevel();
          return;
        }
        is_pure = true;
        trailing_qualifier = TokenKind::KwPure;
        continue;
      }

      if (Match(TokenKind::KwExtern)) {
        if (is_extern) {
          const Token &token = Previous();
          diagnostics_.push_back(
              MakeDiag(token.line, token.column, "O3P100", "duplicate 'extern' qualifier in function declaration"));
          SynchronizeTopLevel();
          return;
        }
        is_extern = true;
        trailing_qualifier = TokenKind::KwExtern;
        continue;
      }

      if (Match(TokenKind::KwAsync)) {
        if (is_async) {
          const Token &token = Previous();
          diagnostics_.push_back(
              MakeDiag(token.line, token.column, "O3P100", "duplicate 'async' qualifier in function declaration"));
          SynchronizeTopLevel();
          return;
        }
        is_async = true;
        trailing_qualifier = TokenKind::KwAsync;
      }
    }

    if (!Match(TokenKind::KwFn)) {
      const Token &token = Peek();
      const std::string message = (trailing_qualifier.has_value() && trailing_qualifier.value() == TokenKind::KwExtern)
                                      ? "expected 'fn' after 'extern'"
                                      : "expected 'fn' after 'pure'";
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P100", message));
      SynchronizeTopLevel();
      return;
    }

    auto fn = ParseFunction();
    if (fn == nullptr) {
      return;
    }

    fn->is_pure = is_pure;
    fn->async_declared = is_async;
    // ParseFunction finalizes semantic profiles before top-level qualifiers are
    // copied onto the declaration, so async callables need one final pass after
    // the real async bit is attached.
    FinalizeAsyncContinuationProfile(*fn);
    FinalizeAwaitSuspensionProfile(*fn);
    if (is_extern && !fn->is_prototype) {
      diagnostics_.push_back(MakeDiag(fn->line, fn->column, "O3P104", "missing ';' after extern function declaration"));
      return;
    }

    ast_builder_.AddFunctionDecl(program, std::move(*fn));
  }
