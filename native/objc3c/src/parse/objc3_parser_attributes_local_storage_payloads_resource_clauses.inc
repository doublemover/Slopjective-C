  bool ApplyLocalResourceAttributeClause(
      LetStmt &stmt,
      LocalResourceAttributeClauseState &clauses,
      const Token &clause,
      const std::string &value_text) {
    if (clause.text == "close") {
      if (clauses.saw_close) {
        return RejectLocalStorageAttributeAt(
            clause, "O3P308", "duplicate objc_resource close clause");
      }
      stmt.resource_close_symbol = value_text;
      clauses.saw_close = true;
      return true;
    }
    if (clause.text == "invalid") {
      if (clauses.saw_invalid) {
        return RejectLocalStorageAttributeAt(
            clause, "O3P309", "duplicate objc_resource invalid clause");
      }
      stmt.resource_invalid_expression = value_text;
      clauses.saw_invalid = true;
      return true;
    }
    return RejectLocalStorageAttributeAt(
        clause, "O3P310",
        "unsupported objc_resource clause '" + clause.text + "'");
  }

  bool ParseLocalResourceAttributeClause(
      LetStmt &stmt,
      LocalResourceAttributeClauseState &clauses) {
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      return RejectLocalStorageAttributeAt(
          token, "O3P305", "invalid objc_resource clause label");
    }
    const Token clause = Advance();
    if (!Match(TokenKind::Equal)) {
      const Token &token = Peek();
      return RejectLocalStorageAttributeAt(
          token, "O3P306",
          "missing '=' after objc_resource clause label");
    }
    const std::string value_text = ParseAttributeArgumentText();
    if (value_text.empty()) {
      const Token &token = Peek();
      return RejectLocalStorageAttributeAt(
          token, "O3P307",
          "objc_resource clause value must not be empty");
    }
    return ApplyLocalResourceAttributeClause(
        stmt, clauses, clause, value_text);
  }

  bool ParseLocalResourceAttributeClauses(
      LetStmt &stmt,
      LocalResourceAttributeClauseState &clauses) {
    while (!At(TokenKind::Eof) && !At(TokenKind::RParen)) {
      if (!ParseLocalResourceAttributeClause(stmt, clauses)) {
        return false;
      }
      if (At(TokenKind::Comma)) {
        Advance();
      } else {
        break;
      }
    }
    return true;
  }
