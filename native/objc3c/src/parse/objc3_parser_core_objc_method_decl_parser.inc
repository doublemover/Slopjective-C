  bool ParseObjcMethodDecl(Objc3MethodDecl &method, bool allow_body) {
    if (Match(TokenKind::Minus)) {
      method.is_class_method = false;
    } else if (Match(TokenKind::Plus)) {
      method.is_class_method = true;
    } else {
      return false;
    }
    const Token method_marker = Previous();
    method.line = method_marker.line;
    method.column = method_marker.column;

    if (At(TokenKind::Less)) {
      const Token generic_clause = Advance();
      int depth = 1;
      while (depth > 0 && !At(TokenKind::Eof)) {
        if (Match(TokenKind::Less)) {
          ++depth;
          continue;
        }
        if (Match(TokenKind::Greater)) {
          --depth;
          continue;
        }
        (void)Advance();
      }
      diagnostics_.push_back(MakeDiag(
          generic_clause.line, generic_clause.column, "O3P114",
          "generic Objective-C method declarations are reserved for a future Objective-C 3 revision"));
      return false;
    }

    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P106", "missing '(' after Objective-C method marker"));
      return false;
    }

    FunctionDecl synthetic_fn;
    if (!ParseFunctionReturnType(synthetic_fn)) {
      return false;
    }
    CopyMethodReturnTypeFromFunctionDecl(synthetic_fn, method);

    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P109", "missing ')' after Objective-C method return type"));
      return false;
    }

    const Token &selector_head = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(selector_head.line, selector_head.column, "O3P101", "invalid Objective-C selector identifier"));
      return false;
    }

    Objc3MethodDecl::SelectorPiece head_piece;
    head_piece.keyword = Previous().text;
    head_piece.line = Previous().line;
    head_piece.column = Previous().column;
    if (Match(TokenKind::Colon)) {
      head_piece.has_parameter = true;
      FuncParam first_param;
      if (!ParseObjcMethodParameterClause(first_param)) {
        return false;
      }
      head_piece.parameter_name = first_param.name;
      method.params.push_back(std::move(first_param));
      method.selector_pieces.push_back(std::move(head_piece));

      while (At(TokenKind::Identifier) && (index_ + 1 < tokens_.size()) && tokens_[index_ + 1].kind == TokenKind::Colon) {
        const Token keyword = Advance();
        (void)Match(TokenKind::Colon);
        Objc3MethodDecl::SelectorPiece keyword_piece;
        keyword_piece.keyword = keyword.text;
        keyword_piece.has_parameter = true;
        keyword_piece.line = keyword.line;
        keyword_piece.column = keyword.column;

        FuncParam keyword_param;
        if (!ParseObjcMethodParameterClause(keyword_param)) {
          return false;
        }
        keyword_piece.parameter_name = keyword_param.name;
        method.params.push_back(std::move(keyword_param));
        method.selector_pieces.push_back(std::move(keyword_piece));
      }
    } else {
      method.selector_pieces.push_back(std::move(head_piece));
    }

    method.selector = BuildNormalizedObjcSelector(method.selector_pieces);
    method.selector_is_normalized = true;

    if (!ParseOptionalAsyncClause(method)) {
      return false;
    }

    if (!ParseOptionalThrowsClause(method)) {
      return false;
    }

    if (!ParseOptionalCallableBridgeAttributes(method)) {
      return false;
    }
    method.returns_borrowed_profile = BuildReturnsBorrowedProfile(
        method.objc_returns_borrowed_declared,
        method.objc_returns_borrowed_owner_index,
        method.return_borrowed_pointer_qualified);

    if (Match(TokenKind::Semicolon)) {
      method.has_body = false;
      method.body.clear();
      FinalizeThrowsDeclarationProfile(method);
      FinalizeResultLikeProfile(method);
      FinalizeNSErrorBridgingProfile(method);
      FinalizeErrorBridgeMarkerProfile(method);
      FinalizeUnwindCleanupProfile(method);
      FinalizeErrorDiagnosticsRecoveryProfile(method);
      FinalizeAsyncContinuationProfile(method);
      FinalizeAwaitSuspensionProfile(method);
      FinalizeActorIsolationSendabilityProfile(method);
      FinalizeTaskRuntimeCancellationProfile(method);
      FinalizeConcurrencyReplayRaceGuardProfile(method);
      FinalizeUnsafePointerExtensionProfile(method);
      FinalizeInlineAsmIntrinsicGovernanceProfile(method);
      return true;
    }

    if (!allow_body) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104",
                                      "missing ';' after Objective-C interface method declaration"));
      return false;
    }

    if (!At(TokenKind::LBrace)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110",
                                      "missing '{' or ';' after Objective-C implementation method declaration"));
      return false;
    }
    method.body = ParseBlock();
    if (block_failed_) {
      block_failed_ = false;
      return false;
    }
    method.has_body = true;
    FinalizeThrowsDeclarationProfile(method);
    FinalizeResultLikeProfile(method);
    FinalizeNSErrorBridgingProfile(method);
    FinalizeErrorBridgeMarkerProfile(method);
    FinalizeUnwindCleanupProfile(method);
    FinalizeErrorDiagnosticsRecoveryProfile(method);
    FinalizeAsyncContinuationProfile(method);
    FinalizeAwaitSuspensionProfile(method);
    FinalizeActorIsolationSendabilityProfile(method);
    FinalizeTaskRuntimeCancellationProfile(method);
    FinalizeConcurrencyReplayRaceGuardProfile(method);
    FinalizeUnsafePointerExtensionProfile(method);
    FinalizeInlineAsmIntrinsicGovernanceProfile(method);
    return true;
  }
