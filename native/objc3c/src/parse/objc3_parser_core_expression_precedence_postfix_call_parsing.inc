  bool ParsePostfixCallArguments(Expr &call) {
    if (At(TokenKind::RParen)) {
      return true;
    }

    while (true) {
      auto arg = ParseExpressionWithBlockLiteralSourceUse(
          BlockLiteralSourceUseKind::CallArgument);
      if (arg == nullptr) {
        return false;
      }
      call.args.push_back(std::move(arg));
      if (!Match(TokenKind::Comma)) {
        return true;
      }
    }
  }

  std::unique_ptr<Expr> ParsePostfixCallExpression(
      std::unique_ptr<Expr> callee) {
    const unsigned callee_line = callee->line;
    const unsigned callee_column = callee->column;
    auto call = std::make_unique<Expr>();
    call->kind = Expr::Kind::Call;
    call->line = callee_line;
    call->column = callee_column;
    if (callee->kind != Expr::Kind::Identifier) {
      diagnostics_.push_back(MakeDiag(
          callee->line, callee->column, "O3P112",
          "call target must be identifier"));
      return nullptr;
    }
    call->ident = callee->ident;
    if (!ParsePostfixCallArguments(*call)) {
      return nullptr;
    }
    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P109", "missing ')' after call"));
      return nullptr;
    }
    return call;
  }
