    if (Match(TokenKind::KwDefer)) {
      const Token &token = Previous();
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Defer;
      stmt->block_stmt = std::make_unique<BlockStmt>();
      stmt->line = token.line;
      stmt->column = token.column;
      stmt->block_stmt->line = token.line;
      stmt->block_stmt->column = token.column;
      if (Peek().kind != TokenKind::LBrace) {
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P110", "defer requires braced body"));
        return nullptr;
      }
      stmt->block_stmt->body = ParseBlock();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwThrow)) {
      const Token &token = Previous();
      auto payload = ParseExpressionWithBlockLiteralSourceUse(
          BlockLiteralSourceUseKind::ExpressionSite);
      if (payload == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::Semicolon)) {
        const Token &semicolon_token = Peek();
        diagnostics_.push_back(MakeDiag(semicolon_token.line,
                                        semicolon_token.column, "O3P104",
                                        "missing ';' after throw statement"));
        return nullptr;
      }
      auto call = std::make_unique<Expr>();
      call->kind = Expr::Kind::Throw;
      call->line = token.line;
      call->column = token.column;
      call->throw_statement_enabled = true;
      call->throw_statement_is_normalized = true;
      call->args.push_back(std::move(payload));
      call->throw_statement_profile = BuildThrowStatementProfile(*call);

      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Expr;
      stmt->line = token.line;
      stmt->column = token.column;
      stmt->expr_stmt = std::make_unique<ExprStmt>();
      stmt->expr_stmt->line = token.line;
      stmt->expr_stmt->column = token.column;
      stmt->expr_stmt->value = std::move(call);
      return stmt;
    }
