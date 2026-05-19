      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::DoWhile;
      stmt->do_while_stmt = std::make_unique<DoWhileStmt>();
      stmt->line = do_token.line;
      stmt->column = do_token.column;
      stmt->do_while_stmt->line = do_token.line;
      stmt->do_while_stmt->column = do_token.column;
      stmt->do_while_stmt->body = std::move(body);

      if (!Match(TokenKind::KwWhile)) {
        const Token &token = Peek();
        emit_do_for_diagnostic(token, "O3P100", "missing 'while' after do block");
        return nullptr;
      }
      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        emit_do_for_diagnostic(token, "O3P106", "missing '(' after while");
        return nullptr;
      }
      stmt->do_while_stmt->condition = ParseExpression();
      if (stmt->do_while_stmt->condition == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        emit_do_for_diagnostic(token, "O3P109", "missing ')' after do-while condition");
        return nullptr;
      }
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        emit_do_for_diagnostic(token, "O3P104", "missing ';' after do-while");
        return nullptr;
      }
      return stmt;
