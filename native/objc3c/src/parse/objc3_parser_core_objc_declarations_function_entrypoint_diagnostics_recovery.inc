  std::string BuildUnexpectedFunctionQualifierMessage(
      const Token &qualifier,
      const char *position) const {
    const std::string prefix =
        qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' "
        : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' "
                                                : "unexpected qualifier 'async' ";
    return prefix + position;
  }

  bool RejectUnexpectedFunctionDeclarationQualifier(const char *position) {
    if (!AtFunctionDeclarationQualifier()) {
      return true;
    }

    const Token qualifier = Advance();
    diagnostics_.push_back(MakeDiag(
        qualifier.line,
        qualifier.column,
        "O3P100",
        BuildUnexpectedFunctionQualifierMessage(qualifier, position)));
    SynchronizeTopLevel();
    return false;
  }

  bool DiagnoseMissingFunctionTerminatorOrBody() {
    const Token &token = Peek();
    if (AtFunctionTerminatorRecoveryLead()) {
      diagnostics_.push_back(MakeDiag(
          token.line,
          token.column,
          "O3P104",
          "missing ';' after function prototype declaration"));
    } else {
      diagnostics_.push_back(MakeDiag(
          token.line,
          token.column,
          "O3P110",
          "missing '{' to start block"));
    }
    SynchronizeTopLevel();
    return false;
  }
