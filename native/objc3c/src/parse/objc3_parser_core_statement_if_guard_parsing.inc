    if (Match(TokenKind::KwIf)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::If;
      stmt->if_stmt = std::make_unique<IfStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->if_stmt->line = Previous().line;
      stmt->if_stmt->column = Previous().column;

      if (At(TokenKind::KwLet) || At(TokenKind::KwVar)) {
        auto clauses = ParseOptionalBindingClauses();
        if (clauses.empty()) {
          return nullptr;
        }
        stmt->if_stmt->optional_binding_surface_enabled = true;
        stmt->if_stmt->optional_binding_clause_count = clauses.size();
        stmt->if_stmt->condition =
            BuildObjc3BoolLiteralExprAt(true, stmt->line, stmt->column);
        auto user_then_body = ParseControlBody();
        if (block_failed_) {
          block_failed_ = false;
          return nullptr;
        }
        for (auto &clause : clauses) {
          auto synthetic = std::make_unique<Stmt>();
          synthetic->kind = Stmt::Kind::Let;
          synthetic->line = clause.line;
          synthetic->column = clause.column;
          synthetic->let_stmt = std::make_unique<LetStmt>();
          synthetic->let_stmt->name = clause.name;
          synthetic->let_stmt->line = clause.line;
          synthetic->let_stmt->column = clause.column;
          synthetic->let_stmt->value = std::move(clause.value);
          stmt->if_stmt->then_body.push_back(std::move(synthetic));
        }
        for (auto &nested : user_then_body) {
          stmt->if_stmt->then_body.push_back(std::move(nested));
        }
        if (Match(TokenKind::KwElse)) {
          stmt->if_stmt->else_body = ParseControlBody();
          if (block_failed_) {
            block_failed_ = false;
            return nullptr;
          }
        }
        return stmt;
      }

      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after if"));
        return nullptr;
      }
      stmt->if_stmt->condition = ParseExpression();
      if (stmt->if_stmt->condition == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after if condition"));
        return nullptr;
      }

      stmt->if_stmt->then_body = ParseControlBody();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }
      if (Match(TokenKind::KwElse)) {
        stmt->if_stmt->else_body = ParseControlBody();
        if (block_failed_) {
          block_failed_ = false;
          return nullptr;
        }
      }
      return stmt;
    }

    if (Match(TokenKind::KwGuard)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::If;
      stmt->if_stmt = std::make_unique<IfStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->if_stmt->line = Previous().line;
      stmt->if_stmt->column = Previous().column;
      auto clauses = ParseGuardConditionClauses();
      if (clauses.empty()) {
        return nullptr;
      }
      stmt->if_stmt->guard_condition_list_surface_enabled = true;
      stmt->if_stmt->condition =
          BuildObjc3BoolLiteralExprAt(false, stmt->line, stmt->column);
      for (auto &clause : clauses) {
        if (clause.kind == ParsedGuardConditionClause::Kind::OptionalBinding) {
          stmt->if_stmt->optional_binding_surface_enabled = true;
          stmt->if_stmt->guard_binding_surface_enabled = true;
          ++stmt->if_stmt->optional_binding_clause_count;
          auto synthetic = std::make_unique<Stmt>();
          synthetic->kind = Stmt::Kind::Let;
          synthetic->line = clause.binding.line;
          synthetic->column = clause.binding.column;
          synthetic->let_stmt = std::make_unique<LetStmt>();
          synthetic->let_stmt->name = clause.binding.name;
          synthetic->let_stmt->line = clause.binding.line;
          synthetic->let_stmt->column = clause.binding.column;
          synthetic->let_stmt->value = std::move(clause.binding.value);
          stmt->if_stmt->then_body.push_back(std::move(synthetic));
        } else {
          ++stmt->if_stmt->guard_boolean_condition_clause_count;
          stmt->if_stmt->guard_condition_exprs.push_back(std::move(clause.condition));
        }
      }
      if (!Match(TokenKind::KwElse)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P100", "missing 'else' after guard condition list"));
        return nullptr;
      }
      stmt->if_stmt->else_body = ParseControlBody();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }
      return stmt;
    }
