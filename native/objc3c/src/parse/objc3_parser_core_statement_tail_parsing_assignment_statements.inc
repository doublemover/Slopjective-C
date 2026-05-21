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
