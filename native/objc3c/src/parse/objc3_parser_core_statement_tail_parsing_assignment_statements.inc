    if (At(TokenKind::Identifier) && index_ + 1 < tokens_.size() &&
        tokens_[index_ + 1].kind == TokenKind::LBracket) {
      const Token name = Advance();
      const Token open = Advance();
      auto key_or_index = ParseExpressionWithBlockLiteralSourceUse(
          BlockLiteralSourceUseKind::ExpressionSite);
      if (key_or_index == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RBracket)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P215",
            "missing ']' after collection mutation index"));
        return nullptr;
      }
      if (!Match(TokenKind::Equal)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P215",
            "collection index mutation requires '='"));
        return nullptr;
      }
      auto value = ParseExpressionWithBlockLiteralSourceUse(
          BlockLiteralSourceUseKind::AssignmentValue);
      if (value == nullptr) {
        return nullptr;
      }
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::CollectionMutation;
      stmt->line = open.line;
      stmt->column = open.column;
      stmt->collection_mutation_stmt =
          std::make_unique<CollectionMutationStmt>();
      stmt->collection_mutation_stmt->kind =
          CollectionMutationStmt::Kind::IndexSet;
      stmt->collection_mutation_stmt->collection_name = name.text;
      stmt->collection_mutation_stmt->key_or_index = std::move(key_or_index);
      stmt->collection_mutation_stmt->value = std::move(value);
      stmt->collection_mutation_stmt->line = open.line;
      stmt->collection_mutation_stmt->column = open.column;
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            BuildObjc3MissingSemicolonDiagnostic(token, Previous(), "collection mutation"));
        return nullptr;
      }
      return stmt;
    }

    if (At(TokenKind::Identifier) && Peek().text == "delete" &&
        index_ + 3 < tokens_.size() &&
        tokens_[index_ + 1].kind == TokenKind::Identifier &&
        tokens_[index_ + 2].kind == TokenKind::LBracket) {
      const Token delete_token = Advance();
      const Token name = Advance();
      (void)Advance();
      auto key_or_index = ParseExpressionWithBlockLiteralSourceUse(
          BlockLiteralSourceUseKind::ExpressionSite);
      if (key_or_index == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RBracket)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P215",
            "missing ']' after collection delete index"));
        return nullptr;
      }
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::CollectionMutation;
      stmt->line = delete_token.line;
      stmt->column = delete_token.column;
      stmt->collection_mutation_stmt =
          std::make_unique<CollectionMutationStmt>();
      stmt->collection_mutation_stmt->kind = CollectionMutationStmt::Kind::Delete;
      stmt->collection_mutation_stmt->collection_name = name.text;
      stmt->collection_mutation_stmt->key_or_index = std::move(key_or_index);
      stmt->collection_mutation_stmt->line = delete_token.line;
      stmt->collection_mutation_stmt->column = delete_token.column;
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            BuildObjc3MissingSemicolonDiagnostic(token, Previous(), "collection delete"));
        return nullptr;
      }
      return stmt;
    }

    if (AtIdentifierAssignment() || AtIdentifierUpdate()) {
      const Token name = Advance();
      std::string op = "=";
      if (!MatchAssignmentOperator(op)) {
        (void)MatchUpdateOperator(op);
      }
      std::unique_ptr<Expr> value;
      if (op == "++" || op == "--") {
        value = nullptr;
      } else {
        value = ParseExpressionWithBlockLiteralSourceUse(
            BlockLiteralSourceUseKind::AssignmentValue);
        if (value == nullptr) {
          return nullptr;
        }
      }
      const Token insertion_anchor = Previous();
      auto stmt = BuildObjc3AssignmentStatement(name, op, std::move(value));
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            BuildObjc3MissingSemicolonDiagnostic(token, insertion_anchor, "assignment"));
        return nullptr;
      }
      return stmt;
    }

    if (AtPrefixUpdate()) {
      std::string op = "++";
      const Token op_token = Peek();
      (void)MatchUpdateOperator(op);
      const Token name = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(MakeDiag(op_token.line, op_token.column, "O3P101", "invalid assignment target"));
        return nullptr;
      }
      const Token insertion_anchor = Previous();
      auto stmt = BuildObjc3AssignmentStatement(name, op, nullptr);
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            BuildObjc3MissingSemicolonDiagnostic(token, insertion_anchor, "assignment"));
        return nullptr;
      }
      return stmt;
    }
