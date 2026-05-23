  void FinalizeObjcMethodProfiles(Objc3MethodDecl &method) {
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
  }

  bool FinishObjcMethodDeclaration(Objc3MethodDecl &method) {
    method.has_body = false;
    method.body.clear();
    FinalizeObjcMethodProfiles(method);
    return true;
  }

  bool ParseObjcMethodImplementationBody(Objc3MethodDecl &method) {
    method.body = ParseBlock();
    if (block_failed_) {
      block_failed_ = false;
      return false;
    }

    method.has_body = true;
    if (RejectValueOptionalExecutableMethod(method)) {
      return false;
    }
    FinalizeObjcMethodProfiles(method);
    return true;
  }

  bool ParseObjcMethodTerminatorOrBody(Objc3MethodDecl &method, bool allow_body) {
    if (Match(TokenKind::Semicolon)) {
      return FinishObjcMethodDeclaration(method);
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

    return ParseObjcMethodImplementationBody(method);
  }
