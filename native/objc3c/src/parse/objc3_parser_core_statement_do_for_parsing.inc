    if (Match(TokenKind::KwDo)) {
      const Token do_token = Previous();
      auto body = ParseControlBody();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }

      if (Match(TokenKind::KwCatch)) {
        auto stmt = std::make_unique<Stmt>();
        stmt->kind = Stmt::Kind::Block;
        stmt->block_stmt = std::make_unique<BlockStmt>();
        stmt->line = do_token.line;
        stmt->column = do_token.column;
        stmt->block_stmt->line = do_token.line;
        stmt->block_stmt->column = do_token.column;
        stmt->block_stmt->is_do_catch_scope = true;
        stmt->block_stmt->body = std::move(body);
        do {
          BlockStmt::CatchClause clause;
          if (!ParseCatchClause(clause)) {
            return nullptr;
          }
          stmt->block_stmt->catch_clauses.push_back(std::move(clause));
        } while (Match(TokenKind::KwCatch));
        stmt->block_stmt->do_catch_is_normalized =
            !stmt->block_stmt->catch_clauses.empty();
        stmt->block_stmt->do_catch_profile =
            BuildDoCatchProfile(*stmt->block_stmt);
        return stmt;
      }

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
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P100", "missing 'while' after do block"));
        return nullptr;
      }
      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after while"));
        return nullptr;
      }
      stmt->do_while_stmt->condition = ParseExpression();
      if (stmt->do_while_stmt->condition == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after do-while condition"));
        return nullptr;
      }
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after do-while"));
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwFor)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::For;
      stmt->for_stmt = std::make_unique<ForStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->for_stmt->line = Previous().line;
      stmt->for_stmt->column = Previous().column;

      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after for"));
        return nullptr;
      }

      if (Match(TokenKind::Semicolon)) {
        stmt->for_stmt->init.kind = ForClause::Kind::None;
      } else {
        if (Match(TokenKind::KwLet)) {
          stmt->for_stmt->init.kind = ForClause::Kind::Let;
          const Token &name_token = Peek();
          if (!Match(TokenKind::Identifier)) {
            diagnostics_.push_back(MakeDiag(name_token.line, name_token.column, "O3P101",
                                            "invalid declaration identifier"));
            return nullptr;
          }
          stmt->for_stmt->init.name = Previous().text;
          stmt->for_stmt->init.line = Previous().line;
          stmt->for_stmt->init.column = Previous().column;

          if (!Match(TokenKind::Equal)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P102", "missing '='"));
            return nullptr;
          }

          stmt->for_stmt->init.value = ParseExpressionWithBlockLiteralSourceUse(
              BlockLiteralSourceUseKind::ExpressionSite);
          if (stmt->for_stmt->init.value == nullptr) {
            return nullptr;
          }
        } else if (AtIdentifierAssignment() || AtIdentifierUpdate()) {
          stmt->for_stmt->init.kind = ForClause::Kind::Assign;
          const Token name = Advance();
          std::string op = "=";
          if (!MatchAssignmentOperator(op)) {
            (void)MatchUpdateOperator(op);
          }
          stmt->for_stmt->init.name = name.text;
          stmt->for_stmt->init.op = op;
          stmt->for_stmt->init.line = name.line;
          stmt->for_stmt->init.column = name.column;
          if (op == "++" || op == "--") {
            stmt->for_stmt->init.value = nullptr;
          } else {
            stmt->for_stmt->init.value = ParseExpressionWithBlockLiteralSourceUse(
                BlockLiteralSourceUseKind::AssignmentValue);
            if (stmt->for_stmt->init.value == nullptr) {
              return nullptr;
            }
          }
        } else if (AtPrefixUpdate()) {
          stmt->for_stmt->init.kind = ForClause::Kind::Assign;
          std::string op = "++";
          (void)MatchUpdateOperator(op);
          const Token name = Peek();
          if (!Match(TokenKind::Identifier)) {
            diagnostics_.push_back(MakeDiag(name.line, name.column, "O3P101",
                                            "invalid assignment target"));
            return nullptr;
          }
          stmt->for_stmt->init.name = name.text;
          stmt->for_stmt->init.op = op;
          stmt->for_stmt->init.line = name.line;
          stmt->for_stmt->init.column = name.column;
          stmt->for_stmt->init.value = nullptr;
        } else {
          stmt->for_stmt->init.kind = ForClause::Kind::Expr;
          stmt->for_stmt->init.line = Peek().line;
          stmt->for_stmt->init.column = Peek().column;
          stmt->for_stmt->init.value = ParseExpressionWithBlockLiteralSourceUse(
              BlockLiteralSourceUseKind::ExpressionSite);
          if (stmt->for_stmt->init.value == nullptr) {
            return nullptr;
          }
        }
        if (!Match(TokenKind::Semicolon)) {
          const Token &token = Peek();
          diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after for init"));
          return nullptr;
        }
      }

      if (Match(TokenKind::Semicolon)) {
        stmt->for_stmt->condition = nullptr;
      } else {
        stmt->for_stmt->condition = ParseExpressionWithBlockLiteralSourceUse(
            BlockLiteralSourceUseKind::ExpressionSite);
        if (stmt->for_stmt->condition == nullptr) {
          return nullptr;
        }
        if (!Match(TokenKind::Semicolon)) {
          const Token &token = Peek();
          diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after for condition"));
          return nullptr;
        }
      }

      if (Match(TokenKind::RParen)) {
        stmt->for_stmt->step.kind = ForClause::Kind::None;
      } else {
        if (AtIdentifierAssignment() || AtIdentifierUpdate()) {
          stmt->for_stmt->step.kind = ForClause::Kind::Assign;
          const Token name = Advance();
          std::string op = "=";
          if (!MatchAssignmentOperator(op)) {
            (void)MatchUpdateOperator(op);
          }
          stmt->for_stmt->step.name = name.text;
          stmt->for_stmt->step.op = op;
          stmt->for_stmt->step.line = name.line;
          stmt->for_stmt->step.column = name.column;
          if (op == "++" || op == "--") {
            stmt->for_stmt->step.value = nullptr;
          } else {
            stmt->for_stmt->step.value = ParseExpressionWithBlockLiteralSourceUse(
                BlockLiteralSourceUseKind::AssignmentValue);
            if (stmt->for_stmt->step.value == nullptr) {
              return nullptr;
            }
          }
        } else if (AtPrefixUpdate()) {
          stmt->for_stmt->step.kind = ForClause::Kind::Assign;
          std::string op = "++";
          (void)MatchUpdateOperator(op);
          const Token name = Peek();
          if (!Match(TokenKind::Identifier)) {
            diagnostics_.push_back(MakeDiag(name.line, name.column, "O3P101",
                                            "invalid assignment target"));
            return nullptr;
          }
          stmt->for_stmt->step.name = name.text;
          stmt->for_stmt->step.op = op;
          stmt->for_stmt->step.line = name.line;
          stmt->for_stmt->step.column = name.column;
          stmt->for_stmt->step.value = nullptr;
        } else {
          stmt->for_stmt->step.kind = ForClause::Kind::Expr;
          stmt->for_stmt->step.line = Peek().line;
          stmt->for_stmt->step.column = Peek().column;
          stmt->for_stmt->step.value = ParseExpressionWithBlockLiteralSourceUse(
              BlockLiteralSourceUseKind::ExpressionSite);
          if (stmt->for_stmt->step.value == nullptr) {
            return nullptr;
          }
        }
        if (!Match(TokenKind::RParen)) {
          const Token &token = Peek();
          diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after for clauses"));
          return nullptr;
        }
      }

      stmt->for_stmt->body = ParseControlBody();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }
      return stmt;
    }
